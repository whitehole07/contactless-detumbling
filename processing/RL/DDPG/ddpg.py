import numpy as np
import torch
from matplotlib import pyplot as plt
import torch.nn.functional as F
import torch.optim as optim
import copy

from processing.RL.DDPG.networks.actor import Actor
from processing.RL.DDPG.networks.critic import Critic
from processing.RL.DDPG.replay_buffer import ReplayBuffer
from processing.RL.hyperparameters import actor_lr, critic_lr

device = 'cuda' if torch.cuda.is_available() else 'cpu'


class DDPG(object):
    def __init__(self, state_dim, action_dim, *, max_action, hidden_actor, hidden_critic, discount=0.99, tau=0.001):
        # Initialize nets
        self.actor = Actor(state_dim, action_dim, hidden_actor, max_action).to(device)
        self.actor_target = copy.deepcopy(self.actor)
        self.actor_optimizer = torch.optim.Adam(self.actor.parameters(), lr=actor_lr)

        self.critic = Critic(state_dim, action_dim, hidden_critic).to(device)
        self.critic_target = copy.deepcopy(self.critic)
        self.critic_optimizer = torch.optim.Adam(self.critic.parameters(), lr=critic_lr, weight_decay=1e-2)

        # Save params
        self.action_dim = action_dim
        self.max_action = max_action

        # Hyperparameters
        self.discount = discount
        self.tau = tau

        # Replay buffers
        self.replay_buffer_1 = ReplayBuffer(state_dim, action_dim)
        self.replay_buffer_2 = ReplayBuffer(state_dim, action_dim)

        # Losses history
        self.actor_losses = []
        self.critic_losses = []

    def select_action(self, state):
        state = torch.FloatTensor(state.reshape(1, -1)).to(device)
        return self.actor(state).cpu().data.numpy().flatten()

    def sample(self):
        return self.max_action * np.random.uniform(-1, 1, size=self.action_dim)

    def train(self, w_hp, batch_size):
        # Sample improved replay buffer
        state, action, next_state, reward, not_done = self.replay_buffer_2.improved_sample(batch_size, w_hp=w_hp)

        # Compute the target Q value
        target_Q = self.critic_target(next_state, self.actor_target(next_state))
        target_Q = reward + (not_done * self.discount * target_Q).detach()

        # Get current Q estimate
        current_Q = self.critic(state, action)

        # Compute critic loss
        critic_loss = F.mse_loss(current_Q, target_Q)

        # Optimize the critic
        self.critic_optimizer.zero_grad()
        critic_loss.backward()
        self.critic_optimizer.step()

        # Compute actor loss
        actor_loss = -self.critic(state, self.actor(state)).mean()

        # Optimize the actor
        self.actor_optimizer.zero_grad()
        actor_loss.backward()
        self.actor_optimizer.step()

        # Store the loss values
        self.critic_losses.append(critic_loss.item())
        self.actor_losses.append(actor_loss.item())

        # Update the frozen target models
        for param, target_param in zip(self.critic.parameters(), self.critic_target.parameters()):
            target_param.data.copy_(self.tau * param.data + (1 - self.tau) * target_param.data)

        for param, target_param in zip(self.actor.parameters(), self.actor_target.parameters()):
            target_param.data.copy_(self.tau * param.data + (1 - self.tau) * target_param.data)

    def save(self, filename):
        torch.save(self.critic.state_dict(), filename + "_critic")
        torch.save(self.critic_optimizer.state_dict(), filename + "_critic_optimizer")

        torch.save(self.actor.state_dict(), filename + "_actor")
        torch.save(self.actor_optimizer.state_dict(), filename + "_actor_optimizer")

    def load(self, filename):
        self.critic.load_state_dict(torch.load(filename + "_critic"))
        self.critic_optimizer.load_state_dict(torch.load(filename + "_critic_optimizer"))
        self.critic_target = copy.deepcopy(self.critic)

        self.actor.load_state_dict(torch.load(filename + "_actor"))
        self.actor_optimizer.load_state_dict(torch.load(filename + "_actor_optimizer"))
        self.actor_target = copy.deepcopy(self.actor)

    def plot_losses(self):
        # Create subplots
        fig, axs = plt.subplots(2, 1, figsize=(10, 10))

        # Plot the critic loss
        axs[0].plot(self.critic_losses, label='Critic Loss')
        axs[0].set_xlabel('Training Steps')
        axs[0].set_ylabel('Loss')
        axs[0].set_title('Critic Loss over Time')
        axs[0].legend()

        # Plot the actor loss
        axs[1].plot(self.actor_losses, label='Actor Loss')
        axs[1].set_xlabel('Training Steps')
        axs[1].set_ylabel('Loss')
        axs[1].set_title('Actor Loss over Time')
        axs[1].legend()

        # Show the plot
        plt.tight_layout()
        plt.show()
