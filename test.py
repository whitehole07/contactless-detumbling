import cppyy
import numpy as np

from environment import attitude, arm, env, t_step, save, debris, joints
from processing.RL.DDPG.ddpg import DDPG
from processing.RL.hyperparameters import max_range, detumbling_threshold, safe_sphere, state_dim, action_dim, \
    actor_layers, critic_layers
from processing.RL.reward_function import state_extractor, evaluate_step
from processing.system_animation import animate_system
from processing.utilities.rotations import construct_rotation_matrix

# Test
total_reward = 0
step = 0
max_time_steps = 1000
retval = 0

# Import agent and load model
agent = DDPG(state_dim, action_dim, hidden_actor=actor_layers, hidden_critic=critic_layers)
agent.load("./processing/RL/")
agent.actor.eval()
agent.critic.eval()

# Reset environment
t, global_state = env.current_state()
state = state_extractor(list(global_state))  # discard time

while t <= max_time_steps and retval == 0:
    # Get action
    action = max_range * agent.select_action(state)  # Noise and clip

    # Inverse kinematics: build desired transformation matrix
    TD = arm.get_transformation(global_state[:6], 6)
    TD[:3, -1] += action
    TD[:3, :3] = construct_rotation_matrix(-(TD[:3, -1] + arm.base_offset))

    # Inverse kinematics: compute new joint angles
    success, yD_arm = arm.inverse_kinematics(TD, np.array(global_state[:6]), 1e-3, 5, 5)

    # DEBUG: time past
    tp = t

    if success:
        # Set control parameters
        env.set_control_torque(yD=cppyy.gbl.std.vector[float](list(yD_arm)))

        # Perform step until convergence or time over
        while t <= max_time_steps and np.any(np.abs(np.array(global_state[:6]) - yD_arm) > 1e-5):
            retval, t, global_state = env.step(t_step=t_step)
            save(t, global_state)
            if retval != 0:
                print(f"INTEGRATION ERROR, return value is non-zero: {retval}")
                break

    # Check for integration errors
    print(f"#{step + 1} - T: {t:.2f}, IT: {t - tp:.2f}, R: {total_reward}, retval: {retval}, ik_success: {success}, action: {action}, p_des: {TD[:3, -1]}")
    if retval != 0:
        break

    # Evaluate reward
    next_state, reward, done = evaluate_step(list(global_state), success, arm, attitude,
                                             detumbling_threshold, safe_sphere)
    total_reward += reward

    # Update state
    state = next_state

    # Check if done
    if done:
        break

    # Update step count
    step += 1

# Final reward
print("Final reward {}".format(total_reward))

# Close environment
del env

# Plots
attitude.plot(["angular_velocity", "torques", "energy", "euler_angles"])
arm.plot()

animate_system(
    t=attitude.t,
    q=attitude.q,
    eu=attitude.euler_angles,
    h=debris.height,
    r=debris.radius,
    dpi=300,
    arms=[arm],
    dh_par=joints
)
