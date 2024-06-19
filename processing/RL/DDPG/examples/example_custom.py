# Initialize environment, agent and hyperparameters
import gym

from processing.RL.DDPG.ddpg import DDPG
from processing.RL.DDPG.noise.ou_noise import OU_Noise

env = gym.make('Pendulum-v1', render_mode='human')
state_dim = env.observation_space.shape[0]
action_dim = env.action_space.shape[0]
max_action = float(env.action_space.high[0])

agent = DDPG(
    state_dim,
    action_dim,
    max_action=max_action,
    hidden_actor=256,
    hidden_critic=256,
    discount=0.99,
    tau=0.005
)

# Training loop
num_episodes = 50
batch_size = 256
iterations = 200

# Create OU noise
ou_noise = OU_Noise(action_dim, 0)

for episode in range(num_episodes):
    state = env.reset()
    episode_reward = 0
    state = state[0]

    for t in range(iterations):
        noise = ou_noise.sample()
        not_noisy_action = agent.select_action(state)
        action = (not_noisy_action + noise).clip(-max_action, max_action)

        next_state, reward, done, _, _ = env.step(action)

        # Update replay buffers
        agent.replay_buffer_1.add(state, action, next_state, reward, float(done))
        agent.replay_buffer_2.copy(agent.replay_buffer_1)

        state = next_state
        episode_reward += reward

        if agent.replay_buffer_2.size > batch_size:
            agent.train(w_hp=0.05, batch_size=batch_size)

        if done:
            break

        ou_noise.reset()
        ou_noise.decay_noise(num_episodes - 25, episode)

    print(f"Episode: {episode}, Reward: {episode_reward}")

agent.plot_losses()

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
