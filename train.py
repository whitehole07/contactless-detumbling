# Entities
import numpy as np
import torch

from processing.RL.DDPG.ddpg import DDPG
from processing.robotics.arm_propagator import ArmPropagator, ElectromagnetEndEffector
from processing.system_animation import animate_system
from processing.utilities.entities import Cylinder

# Attitude
from processing.attitude.attitude_propagator import AttitudePropagator
from processing.attitude.torques.base import TorqueObject
from processing.attitude.torques.eddy_current import EddyCurrentTorque

# Environment
from propagator.bin.environment import Environment

def save_step(t: float, prop: list, at: AttitudePropagator, ar: ArmPropagator):
    # Convert to numpy
    prop = np.array(prop)

    # Save arm
    if ar._timestamps is None:
        ar._timestamps = np.array([t])
        ar._prop_sol = prop[0:12].reshape(-1, 1)
    else:
        ar._timestamps = np.hstack((ar._timestamps, np.array([t])))
        ar._prop_sol = np.hstack((ar._prop_sol, prop[0:12].reshape(-1, 1)))

    # Save end effector results
    ar.end_effector._timestamps = ar._timestamps
    if ar.end_effector.locations is None:
        ar.end_effector.locations = prop[19:22].reshape(-1, 1)
        ar.end_effector.poses = prop[22:25].reshape(-1, 1)
    else:
        ar.end_effector.locations = np.hstack((ar.end_effector.locations, prop[19:22].reshape(-1, 1)))
        ar.end_effector.poses = np.hstack((ar.end_effector.poses, prop[22:25].reshape(-1, 1)))

    # Save attitude
    at._timestamps = ar._timestamps
    if at._prop_sol is None:
        at._prop_sol = prop[12:19].reshape(-1, 1)
    else:
        at._prop_sol = np.hstack((at._prop_sol, prop[12:19].reshape(-1, 1)))

    return


def evaluate_step(t, step_ret, state):
    # Compute done (collision + tumbling rate)
    done = 0

    # Compute reward
    reward = 0

    return step_ret, reward, done

# Propagation settings
t_step = 1
t_stop = 200

# Generate debris
debris = Cylinder(
    mass=950.0,
    radius=2.5,
    height=5.0,
    thickness=0.1,
    sigma=35000000.0
)

# Generate robotic arms (UR10 standard)
# Joints
# TODO: Consider adjusting the scaling and the inertia matrices by considering a cylindrical simplified model
scale = 10
joints = np.array([
    [0, 0, scale*0.1807, np.pi/2],
    [0, scale*-0.6127, 0, 0],
    [0, scale*-0.57155, 0, 0],
    [0, 0, scale*0.17415, np.pi/2],
    [0, 0, scale*0.11985, -np.pi/2],
    [0, 0, scale*0.11655, 0]
])

# End effector
electromagnet: ElectromagnetEndEffector = ElectromagnetEndEffector(
    n_turns=500.0,
    radius=1.0,
    current=50.0
)
electromagnets = [electromagnet]

# External moments
# Eddy current
eddy: TorqueObject = EddyCurrentTorque(
    entity=debris,
    chaser_w0=[0.0, 0.0, 0.0],
    electromagnets=electromagnets
)

# Save attitude results
attitude = AttitudePropagator(entity=debris, M_ext=eddy)

# Save robotic arm results
base_offset = np.array([10, 0, 5])
arm = ArmPropagator(joints=joints, end_effector=electromagnet, base_offset=base_offset)

# Generate environment
# Initial conditions
y0_arm = [0.0, -0.7, -0.3, 0.0, 0.0, 0.0, -0.02, 0.0, 0.0, 0.0, 0.0, 0.0]
y0_debris = [0.1, 0.2, 0.0, 0.0, 0.0, 0.0, 1.0]
env = Environment(
    y0_arm + y0_debris,
    float(debris.inertia_matrix[0, 0]), float(debris.inertia_matrix[1, 1]), float(debris.inertia_matrix[2, 2]),
    debris.radius, debris.height, debris.thickness, debris.sigma,
    electromagnet.n_turns, electromagnet.current, electromagnet.radius,
    float(arm.base_offset[0]), float(arm.base_offset[1]), float(arm.base_offset[2]),
    scale, list(map(float, arm.joints[:, 1])), list(map(float, arm.joints[:, 2])), list(map(float, arm.joints[:, 3]))
    )

# Init torch
"""device = 'cpu'

# Define different parameters for training the agent
max_episode = 100
max_time_steps = 5000
ep_r = 0
total_step = 0
score_hist = []

# For rensering the environmnet
render = True
render_interval = 10

# For reproducibility
# env.seed(0)
torch.manual_seed(0)
np.random.seed(0)

# Environment action ans states
state_dim = 19
action_dim = 6
max_action = 0.1  # max torque
min_Val = torch.tensor(1e-7).float().to(device)

# Exploration Noise
exploration_noise = 0.1 * max_action

# Create a DDPG instance
agent = DDPG(state_dim, action_dim, hidden_actor=20, hidden_critic=64)

# Train the agent for max_episodes
for i in range(max_episode):
    total_reward = 0
    step = 0

    # Reset environment
    env.reset()
    state = env.current_state()
    for _ in range(max_time_steps):
        # Get action
        action = agent.select_action(state)
        action = (action + np.random.normal(0, 1, size=action_dim)).clip(-max_action, max_action) # Noise and clip
        # action += ou_noise.sample()

        # Perform step
        t, step_ret = env.step(t_step=t_step, action=action)
        next_state, reward, done = evaluate_step(t, step_ret, state)

        # Retrieve reward
        total_reward += reward
        agent.replay_buffer.push((state, next_state, action, reward, done))
        state = next_state
        if done:
            break
        step += 1

    score_hist.append(total_reward)
    total_step += step + 1
    print("Episode: \t{}  Total Reward: \t{:0.2f}".format(i, total_reward))
    agent.update(update_iteration=200, gamma=0.99, tau=0.001, batch_size=64)
    if i % 10 == 0:
        agent.save("./")
"""
# Save initial conditions
t, step_ret = env.current_state()
save_step(t, step_ret, attitude, arm)

# Perform step
while t < t_stop:
    t, step_ret = env.step(t_step=t_step, action=[0,0,0,0,0,0])
    save_step(t, step_ret, attitude, arm)

# Test
attitude.plot(["angular_velocity", "quaternions", "energy", "euler_angles"])
arm.plot()

# Animate
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
