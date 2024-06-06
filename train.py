# Entities
import numpy as np
import matplotlib.pyplot as plt
import torch

from processing.RL.DDPG.ddpg import DDPG
from processing.robotics.arm_propagator import ArmPropagator, ElectromagnetEndEffector, RevoluteJoint
from processing.system_animation import animate_system
from processing.utilities.entities import Cylinder

# Attitude
from processing.attitude.attitude_propagator import AttitudePropagator
from processing.attitude.torques.base import TorqueObject
from processing.attitude.torques.eddy_current import EddyCurrentTorque

# Environment
from propagator.bin.environment import Environment

# Functions
def plot_learning_curve(x, scores):
    running_avg = np.zeros(len(scores))
    for i in range(len(running_avg)):
        running_avg[i] = np.mean(scores[max(0, i - 100):(i + 1)])
    plt.plot(x, running_avg)
    plt.title(f'Running average of previous {max_episode} scores')


def save(tf, prop):
    # to numpy
    prop = np.array(prop)

    # save
    arm.save_new(tf, prop=prop)
    attitude.save_new(tf, prop=prop)


def evaluate_step(step_ret, prev_state):
    # Convert to numpy
    step_ret = np.array(step_ret)
    state = np.array([*step_ret[:15], *step_ret[19:25]])

    # Init "done" and "reward"
    done = 0  # "done" normally negative
    reward = 0

    # Check: collision with safe sphere
    for i in range(len(arm.joints) + 1):
        pos: np.ndarray = arm.get_joint_position(step_ret[0:6], i)
        if np.linalg.norm(pos) <= safe_sphere:
            reward -= 10

    # Check: tumbling rates below certain threshold
    ang_vels: np.ndarray = step_ret[12:15]
    if np.all(np.abs(ang_vels) <= detumbling_threshold):
        done = 1
        # reward += 25
        print("Detumbling", reward)
        return state, reward, done  # Detumbling completed, simulation over

    # Penalty: end effector pointing away
    ee_loc: np.ndarray = -step_ret[19:22]
    ee_pos: np.ndarray = step_ret[22:25]
    angle = np.arccos(np.dot(ee_loc / np.linalg.norm(ee_loc), ee_pos / np.linalg.norm(ee_pos)))
    reward -= abs(angle)

    return state, reward, done


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
joints = [
    RevoluteJoint(0, 0.5, np.pi/2),
    RevoluteJoint(2.5, 0, 0),
    RevoluteJoint(2.5, 0, 0),
    RevoluteJoint(0, 0.5, np.pi/2),
    RevoluteJoint(0, 0.25, -np.pi/2),
    RevoluteJoint(0, 0.25, 0)
]

# CoM
com = [
    [0, -joints[0].d/2, 0],
    [joints[1].a/2, 0, 0],
    [joints[2].a/2, 0, 0],
    [0, -joints[3].d/2, 0],
    [0, joints[4].d/2, 0],
    [0, 0, -joints[5].d/2]
]

# End effector
electromagnet: ElectromagnetEndEffector = ElectromagnetEndEffector(
    n_turns=500.0,
    radius=1.0,
    current=50.0
)

# External moments
# Eddy current
eddy: TorqueObject = EddyCurrentTorque(
    entity=debris,
    chaser_w0=[0.0, 0.0, 0.0],
    electromagnets=[electromagnet]
)

# Save attitude results
attitude = AttitudePropagator(entity=debris, M_ext=eddy)

# Save robotic arm results
base_offset = np.array([10, 0, 5])
max_torques = np.array([.1, .1, .1, .1, .1, .1])
arm = ArmPropagator(joints=joints, com=com, end_effector=electromagnet, base_offset=base_offset, max_torques=max_torques)

# Set propagation settings
t_step = .1  # Propagation time step [s]

# Set initial conditions
y0_arm = [
    5, 3.14, 5, 1.5, 1.5, 1.5,   # Initial joint angles
    0.02, 0.0, 0.0, 0.0, 0.0, 0.0   # Initial joint velocities
]

y0_debris = [
    0.1, 0.2, 0.0,                    # Initial debris angular velocity
    0.0, 0.0, 0.0, 1.0                # Initial debris quaternions
]

# Initialize environment
env = Environment(
    y0_arm + y0_debris,                   # Initial conditions
    debris.Ixx,                           # Debris Ixx
    debris.Iyy,                           # Debris Iyy
    debris.Izz,                           # Debris Izz
    debris.radius,                        # Debris cylinder radius
    debris.height,                        # Debris cylinder height
    debris.thickness,                     # Debris cylinder thickness
    debris.sigma,                         # Debris conductivity
    electromagnet.n_turns,                # Coil number of turns
    electromagnet.current,                # Coil current
    electromagnet.radius,                 # Coil radius
    arm.base_offset_x,                    # Arm base x-offset
    arm.base_offset_y,                    # Arm base y-offset
    arm.base_offset_z,                    # Arm base z-offset
    arm.dh_a,                             # Arm joint a parameters
    arm.dh_d,                             # Arm joint d parameters
    arm.dh_alpha,                         # Arm joint alpha parameters
    arm.max_torques,                      # Arm joint max torques
    arm.com                               # Arm link CoMs
)


# Get initial state
t, s = env.current_state()
save(t, s)

# Define safe sphere for collision check
safe_sphere: float = max(debris.height/2, debris.radius) + 0.5  # [m]

# Define detumbling threshold
detumbling_threshold: float = 0.01  # [rad/s]

# Init torch
device = 'cuda' if torch.cuda.is_available() else 'cpu'

# Define different parameters for training the agent
max_episode = 50
max_time_steps = 500
ep_r = 0
total_step = 0
score_hist = []

# For reproducibility
# env.seed(0)
# torch.manual_seed(0)
# np.random.seed(0)

# Environment action and states
# States:
# pos_EE (3), vel_EE (3), om_EE (3), om_DEB (3)
#
# Actions:
# d_EE (3)
#
def state_extractor(to_be_extracted):
    return to_be_extracted[20:23] + to_be_extracted[32:38] + to_be_extracted[13:16]

state_dim = 12
action_dim = 3
max_range = np.array([0.1, 0.1, 0.1])

# Create a DDPG instance
agent = DDPG(state_dim, action_dim, hidden_actor=256, hidden_critic=256)

# Train the agent for max_episodes
for i in range(max_episode):
    total_reward = 0
    step = 0

    # Reset environment
    t, global_state = env.current_state()
    state = state_extractor(global_state)  # discard time
    for _ in range(max_time_steps):
        # Get action
        noise = max_range * np.random.normal(0, 1, size=action_dim)
        action = (agent.select_action(state) + noise).clip(-max_range, max_range)  # Noise and clip
        # action += ou_noise.sample()

        # Inverse kinematics


        # Set control parameters
        env.set_control_torque(yD=list(action) + [0.0, 0.0, 0.0, 0.0, 0.0, 0.0])

        # Perform step
        t, step_ret = env.step(t_step=t_step)
        next_state, reward, done = evaluate_step(step_ret, state)

        # Retrieve reward
        total_reward += reward
        agent.replay_buffer.push((state, next_state, action, reward, done))
        state = next_state
        if done:
            break
        step += 1

    del env
    env = Environment(
        y0_arm + y0_debris,  # Initial conditions
        debris.Ixx,  # Debris Ixx
        debris.Iyy,  # Debris Iyy
        debris.Izz,  # Debris Izz
        debris.radius,  # Debris cylinder radius
        debris.height,  # Debris cylinder height
        debris.thickness,  # Debris cylinder thickness
        debris.sigma,  # Debris conductivity
        electromagnet.n_turns,  # Coil number of turns
        electromagnet.current,  # Coil current
        electromagnet.radius,  # Coil radius
        arm.base_offset_x,  # Arm base x-offset
        arm.base_offset_y,  # Arm base y-offset
        arm.base_offset_z,  # Arm base z-offset
        arm.dh_a,  # Arm joint a parameters
        arm.dh_d,  # Arm joint d parameters
        arm.dh_alpha,  # Arm joint alpha parameters
        arm.max_torques  # Arm joint max torques
    )

    score_hist.append(total_reward)
    total_step += step + 1
    print("\nEpisode: {} \t Time: {:0.2f}s \t Total Reward: {:0.2f}".format(i, t, total_reward))
    agent.update(update_iteration=200, gamma=0.99, tau=0.001, batch_size=32)
    if i % 10 == 0:
        agent.save("./")

x = [i + 1 for i in range(len(score_hist))]
plot_learning_curve(x, score_hist)