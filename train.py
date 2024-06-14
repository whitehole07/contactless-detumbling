# Entities
import cppyy
import numpy as np
import matplotlib.pyplot as plt

# DDPG modules
from processing.RL.DDPG.ddpg import DDPG
from processing.RL.DDPG.noise import ou_noise

# Environment
from environment import env, arm, t_step, attitude
from processing.RL.hyperparameters import max_action, state_dim, action_dim, actor_layers, critic_layers, \
    detumbling_threshold, safe_sphere, gamma, tau, batch_size
from processing.RL.reward_function import state_extractor, evaluate_step

# Transformation matrix utility
from processing.utilities.rotations import construct_rotation_matrix
from processing.utilities.transformations import spherical_to_cartesian, cartesian_to_spherical

# Define different parameters for training the agent
epochs = 100
max_time_steps = 10000
total_step = 0
score_hist = []

# For reproducibility
# env.seed(0)
# torch.manual_seed(0)
# np.random.seed(0)

# Create a DDPG instance
agent = DDPG(
    state_dim,
    action_dim,
    hidden_actor=actor_layers,
    hidden_critic=critic_layers,
    max_action=max_action,
    discount=gamma,
    tau=tau
)

# Create OU noise
ou_noise = ou_noise.OU_Noise(action_dim, 0)

# Train the agent for max_episodes
for i in range(epochs):
    total_reward = 0
    step = 0
    retval = 0

    # Reset environment
    t, global_state = env.current_state()
    state = state_extractor(list(global_state))  # discard time

    while t <= max_time_steps and retval == 0:
        # Get action, random if replay buffer is not full
        if agent.replay_buffer_1.size >= 15000:
            noise = ou_noise.sample()
            # noise = agent.sample()
            not_noisy_action = agent.select_action(state)
            action = (not_noisy_action + noise).clip(-max_action, max_action)
            printa = f"{not_noisy_action} + {noise}"
            # TODO: normalize the action, last layer tanh, see how to deal with noise in terms of action boundaries
            # TODO: Consider using uniform noise
        else:
            action = agent.sample()
            printa = f"{action}"

        # Inverse kinematics: build desired transformation matrix
        TD = arm.get_transformation(global_state[:6], 6)

        p_glob_spherical = cartesian_to_spherical(*(TD[:3, -1] + arm.base_offset))
        p_glob_spherical[1:3] += action
        p_glob_cartesian = spherical_to_cartesian(*p_glob_spherical)

        TD[:3, -1] = p_glob_cartesian - arm.base_offset
        TD[:3, :3] = construct_rotation_matrix(-(TD[:3, -1] + arm.base_offset))

        # Inverse kinematics: compute new joint angles
        success, yD_arm = arm.inverse_kinematics(TD, np.array(global_state[:6]), 1e-3, 5, 5)

        # DEBUG: past time
        tp = t

        if success:
            # Set control parameters
            env.set_control_torque(yD=cppyy.gbl.std.vector[float](list(yD_arm)))

        # Perform step until convergence or time over
        # while t <= max_time_steps and np.any(np.abs(np.array(global_state[:6]) - yD_arm) > 1e-5):
        while t - tp <= 50:
            retval, t, global_state = env.step(t_step=t_step)

            if retval != 0:
                print(f"INTEGRATION ERROR, return value is non-zero: {retval}")
                break

        # Check for integration errors
        w: np.ndarray = np.array(list(global_state[12:15]))
        II = attitude._entity.inertia_matrix
        E = np.dot(np.dot(w, II), w.T)
        if retval != 0:
            break

        # Evaluate reward
        next_state, reward, done = evaluate_step(state, list(global_state), success, attitude,
                                                 detumbling_threshold, safe_sphere)
        total_reward += reward

        print(f"#{i+1}-{step+1}[{agent.replay_buffer_1.size}] - T: {t:.2f}, IT: {t - tp:.2f}, E: {E:.2f}, d: {np.linalg.norm(np.array(list(global_state[19:22]))):.2f},"
              f" R: {reward:.2f}, TR: {total_reward:.2f}, ik_success: {success}, action: {printa}, p_des: {TD[:3, -1]}")

        # Update replay buffers
        agent.replay_buffer_1.add(state, action, next_state, reward, done)
        agent.replay_buffer_2.copy(agent.replay_buffer_1)

        # Train if buffer B1 is full
        if agent.replay_buffer_1.size >= 15000:
            agent.train(w_hp=0.7, batch_size=batch_size)

        # Update state
        state = next_state

        # Check if done
        if done:
            break

        # Update step count
        step += 1

    # Reset environment
    env.reset()
    ou_noise.reset()

    # Decay noise
    # ou_noise.decay_noise(max_episode, i+1)

    # Update history
    score_hist.append(total_reward)
    total_step += step + 1
    print("\nEpisode: {} \t Time: {:0.2f}s \t Total Reward: {:0.2f}".format(i, t, total_reward))
    if i % 10 == 0:
        agent.save("./processing/RL/")

# Plot history
x = [i + 1 for i in range(len(score_hist))]
running_avg = np.zeros(len(score_hist))

for i in range(len(running_avg)):
    running_avg[i] = np.mean(score_hist[max(0, i - 10):(i + 1)])

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 6))
ax1.plot(x, running_avg)
ax2.plot(x, score_hist)
ax1.set_title(f'Running average of previous {epochs} scores')
ax2.set_title(f'Total reward for each episode')
ax1.grid(True)
ax2.grid(True)
plt.tight_layout()
plt.show()

agent.plot_losses()