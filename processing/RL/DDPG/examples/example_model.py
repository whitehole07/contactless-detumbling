import gym
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F

from processing.RL.DDPG.noise.ou_noise import OU_Noise


class Actor(nn.Module):
    """
    The Actor model takes in a state observation as input and
    outputs an action, which is a continuous value.

    It consists of four fully coonected linear layers with ReLU activation functions and
    a final output layer selects one single optimized action for the state
    """

    def __init__(self, n_states, action_dim, hidden1, max_action):
        super(Actor, self).__init__()
        self.net = nn.Sequential(
            nn.Linear(n_states, hidden1),
            nn.ReLU(),
            nn.Linear(hidden1, hidden1),
            nn.ReLU(),
            nn.Linear(hidden1, hidden1),
            nn.ReLU(),
            nn.Linear(hidden1, action_dim),
            nn.Tanh()
        )

        # Save max action
        self.max_action = torch.from_numpy(np.array(max_action)).float().to(device)

    def forward(self, state):
        return self.max_action * self.net(state)

class Critic(nn.Module):
    """
    The Critic model takes in both a state observation and an action as input and
    outputs a Q-value, which estimates the expected total reward for the current state-action pair.

    It consists of four linear layers with ReLU activation functions,
    State and action inputs are concatenated before being fed into the first linear layer.

    The output layer has a single output, representing the Q-value
    """

    def __init__(self, n_states, action_dim, hidden2):
        super(Critic, self).__init__()
        self.net = nn.Sequential(
            nn.Linear(n_states + action_dim, hidden2),
            nn.ReLU(),
            nn.Linear(hidden2, hidden2),
            nn.ReLU(),
            nn.Linear(hidden2, hidden2),
            nn.ReLU(),
            nn.Linear(hidden2, 1)
        )

    def forward(self, state, action):
        return self.net(torch.cat((state, action), 1))

# Replay buffer to store experience
class ReplayBuffer:
    def __init__(self, max_size):
        self.storage = []
        self.max_size = max_size
        self.ptr = 0

    def add(self, transition):
        if len(self.storage) == self.max_size:
            self.storage[int(self.ptr)] = transition
            self.ptr = (self.ptr + 1) % self.max_size
        else:
            self.storage.append(transition)

    def sample(self, batch_size):
        ind = np.random.randint(0, len(self.storage), size=batch_size)
        states, next_states, actions, rewards, dones = [], [], [], [], []

        for i in ind:
            state, next_state, action, reward, done = self.storage[i]
            states.append(np.array(state, copy=False))
            next_states.append(np.array(next_state, copy=False))
            actions.append(np.array(action, copy=False))
            rewards.append(np.array(reward, copy=False))
            dones.append(np.array(done, copy=False))

        return (
            np.array(states),
            np.array(next_states),
            np.array(actions),
            np.array(rewards).reshape(-1, 1),
            np.array(dones).reshape(-1, 1)
        )

# DDPG Agent
class DDPG:
    def __init__(self, state_dim, action_dim, max_action):
        self.actor = Actor(state_dim, action_dim, 400, max_action).to(device)
        self.actor_target = Actor(state_dim, action_dim, 400, max_action).to(device)
        self.actor_target.load_state_dict(self.actor.state_dict())
        self.actor_optimizer = optim.Adam(self.actor.parameters(), lr=1e-3)

        self.critic = Critic(state_dim, action_dim, 400).to(device)
        self.critic_target = Critic(state_dim, action_dim, 400).to(device)
        self.critic_target.load_state_dict(self.critic.state_dict())
        self.critic_optimizer = optim.Adam(self.critic.parameters(), lr=1e-3)

        self.replay_buffer = ReplayBuffer(max_size=1e6)
        self.max_action = max_action

    def select_action(self, state):
        state = torch.FloatTensor(state.reshape(1, -1)).to(device)
        return self.actor(state).cpu().data.numpy().flatten()

    def train(self, iterations, batch_size=64, discount=0.99, tau=0.005):
        for it in range(iterations):
            # Sample a batch of transitions from replay buffer
            state, next_state, action, reward, done = self.replay_buffer.sample(batch_size)

            state = torch.FloatTensor(state).to(device)
            next_state = torch.FloatTensor(next_state).to(device)
            action = torch.FloatTensor(action).to(device)
            reward = torch.FloatTensor(reward).to(device)
            done = torch.FloatTensor(done).to(device)

            # Compute the target Q value
            target_Q = self.critic_target(next_state, self.actor_target(next_state))
            target_Q = reward + ((1 - done) * discount * target_Q).detach()

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

            # Update the frozen target models
            for param, target_param in zip(self.critic.parameters(), self.critic_target.parameters()):
                target_param.data.copy_(tau * param.data + (1 - tau) * target_param.data)

            for param, target_param in zip(self.actor.parameters(), self.actor_target.parameters()):
                target_param.data.copy_(tau * param.data + (1 - tau) * target_param.data)

# Initialize environment, agent and hyperparameters
env = gym.make('Pendulum-v1')
state_dim = env.observation_space.shape[0]
action_dim = env.action_space.shape[0]
max_action = float(env.action_space.high[0])
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

agent = DDPG(state_dim, action_dim, max_action)

# Create OU noise
ou_noise = OU_Noise(action_dim, 0)

# Training loop
num_episodes = 500
batch_size = 64
iterations = 200

for episode in range(num_episodes):
    state = env.reset()
    episode_reward = 0
    state = state[0]

    for t in range(iterations):
        noise = ou_noise.sample()
        not_noisy_action = agent.select_action(state)
        action = (not_noisy_action + noise).clip(-max_action, max_action)

        next_state, reward, done, _, _ = env.step(action)
        agent.replay_buffer.add((state, next_state, action, reward, float(done)))
        state = next_state
        episode_reward += reward

        if len(agent.replay_buffer.storage) > batch_size:
            agent.train(iterations=1, batch_size=batch_size)

        if done:
            break

    print(f"Episode: {episode}, Reward: {episode_reward}")

    ou_noise.reset()
    ou_noise.decay_noise(num_episodes-100, episode)

# Save the trained model
torch.save(agent.actor.state_dict(), 'ddpg_actor.pth')
torch.save(agent.critic.state_dict(), 'ddpg_critic.pth')

# Optionally, you can test the trained agent after training
state = env.reset()
state = state[0]
for t in range(200):
    env.render()
    action = agent.select_action(state)
    state, _, done, _, _ = env.step(action)
    if done:
        break
env.close()
