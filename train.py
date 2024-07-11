# Entities
import cppyy
import numpy as np
import matplotlib.pyplot as plt

# DDPG modules
from processing.RL.DDPG.ddpg import DDPG
from processing.RL.DDPG.noise import ou_noise

# Environment
from environment import env, arm, t_step, attitude, random_y0, save, debris
from processing.RL.hyperparameters import max_action, state_dim, action_dim, actor_layers, critic_layers, \
    detumbling_threshold, safe_sphere, gamma, tau, batch_size, sample_max_timestep
from processing.RL.reward_function import state_extractor, evaluate_step
from processing.system_animation import animate_system

# Transformation matrix utility
from processing.utilities.rotations import construct_rotation_matrix

# Define different parameters for training the agent
epochs = 300
max_time_steps = 86400
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
ou_noise = ou_noise.OU_Noise(action_dim, 0, sigma=max_action*0.2)

# Train the agent for max_episodes
for i in range(epochs):
    total_reward = 0
    step = 0
    retval = 0

    # Reset environment
    t, global_state = env.current_state()
    state = state_extractor(list(global_state))  # discard time

    # Angle between ee and angular velocity
    angles = []
    done = False

    while not done and t <= max_time_steps and retval == 0:
        # Get action, random if replay buffer is not full
        noise = ou_noise.sample()
        # noise = agent.sample()
        not_noisy_action = agent.select_action(state)
        action = (not_noisy_action + noise).clip(-max_action, max_action)

        # Inverse kinematics: build desired transformation matrix
        TD = arm.get_transformation(global_state[:6], 6)
        TD[:3, -1] += action
        TD[:3, :3] = construct_rotation_matrix(-(TD[:3, -1] + arm.base_offset))

        # Inverse kinematics: compute new joint angles
        success, yD_arm = arm.inverse_kinematics(TD, np.array(global_state[:6]), 1e-3, 5, 5)

        # DEBUG: past time
        tp = t

        # Set control parameters
        if success:
            env.set_control_torque(yD=cppyy.gbl.std.vector[float](list(yD_arm)))

        # Perform step until convergence or time over
        while t - tp < 50:
            retval, t, global_state = env.step(t_step=t_step)

            # Check for integration errors
            if retval != 0:
                break
        save(t, global_state)

        # Compute angle
        global_state = np.array(list(global_state))
        w: np.ndarray = global_state[12:15]
        ang_vel = w / np.linalg.norm(w)
        ee_pos = global_state[22:25] / np.linalg.norm(global_state[22:25])
        dot_prod = np.clip(np.dot(ang_vel, ee_pos), -1.0, 1.0)
        angle = np.rad2deg(np.arccos(dot_prod))
        angles.append(angle)

        # Compute energy
        II = attitude._entity.inertia_matrix
        E = np.dot(np.dot(w, II), w.T)

        # Evaluate reward
        next_state, reward, done = evaluate_step(state, list(global_state), success, attitude,
                                                 detumbling_threshold, safe_sphere)
        total_reward += reward

        print(f"#{i+1}-{step+1}[{agent.replay_buffer_1.size}] - T: {t:.2f}, IT: {t - tp:.2f}, A: {angle:.2f}, E: {E:.2f}, d: {np.linalg.norm(global_state[19:22]) - safe_sphere:.2f},"
              f" R: {reward:.2f}, TR: {total_reward:.2f}, ik_success: {success}, action: {not_noisy_action} + {noise}")

        # Update replay buffers
        agent.replay_buffer_1.add(state, action, next_state, reward, done)
        agent.replay_buffer_2.copy(agent.replay_buffer_1)

        # Train if buffer B1 is full
        if agent.replay_buffer_1.size >= sample_max_timestep:
            agent.train(w_hp=0.1, batch_size=batch_size)

        # Update state
        state = next_state

        # Check if done
        if done:
            break

        # Update step count
        step += 1

    if i+1 >= epochs-10:
        fig, axs = plt.subplots(1, 1, figsize=(8, 6))

        # Plot the critic loss
        axs.axhline(y=0, color='r', linestyle='--', label='No-torque line')
        axs.axhline(y=180, color='b', linestyle='--', label='No-torque line')
        axs.axhline(y=90, color='green', linestyle='-.', label='Max-torque line')
        axs.plot(angles, label='Angle')
        axs.set_xlabel('Time [s]')
        axs.set_ylabel('Angle [deg]')
        axs.set_title('EE-AV angle')
        axs.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.show()

        attitude.plot(["angular_velocity", "torques", "energy", "euler_angles"])

        arm.plot()

        if input("Animate?") == "1":
            animate_system(
                t=attitude.t,
                q=attitude.q,
                eu=attitude.euler_angles,
                h=debris.height,
                r=debris.radius,
                dpi=300,
                att=attitude,
                arms=[arm]
            )

    # Reset environment
    env.reset_change_y0(cppyy.gbl.std.vector[float](random_y0()))
    # env.reset()
    ou_noise.reset()
    attitude.reset()
    arm.reset()

    # Decay noise
    ou_noise.decay_noise(int(0.7*epochs), i+1)

    # Update history
    score_hist.append(total_reward)
    total_step += step + 1
    print("\nEpisode: {} \t Time: {:0.2f}s \t Total Reward: {:0.2f}".format(i+1, t, total_reward))
    if i % 10 == 0:
        agent.save("./processing/RL/")

# Plot history
x = [i + 1 for i in range(len(score_hist))]
running_avg = np.zeros(len(score_hist))

for i in range(len(running_avg)):
    running_avg[i] = np.mean(score_hist[max(0, i - 25):(i + 1)])

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