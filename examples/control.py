# Entities
import numpy as np

from processing.robotics.arm_propagator import ArmPropagator, ElectromagnetEndEffector, RevoluteJoint
from processing.system_animation import animate_system
from processing.utilities.entities import Cylinder

# Attitude
from processing.attitude.attitude_propagator import AttitudePropagator
from processing.attitude.torques.base import TorqueObject
from processing.attitude.torques.eddy_current import EddyCurrentTorque

# Environment
from propagator.bin.environment import Environment

from processing.utilities.rotations import construct_rotation_matrix

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


def save(tf, prop):
    # to numpy
    prop = np.array(prop)

    # save
    arm.save_new(tf, prop=prop)
    attitude.save_new(tf, prop=prop)


# Get initial state
t, s = env.current_state()
save(t, s)

# Solve inverse kinematics
TD = np.eye(4)
TD[:3, -1] = [-4, 0, 3]
TD[:3, :3] = construct_rotation_matrix(-(TD[:3, -1] + arm.base_offset))

yD_arm = arm.inverse_kinematics(TD, np.array(y0_arm[:6]), 1e-5, 100, 20)
T_c = arm.get_transformation(yD_arm, 6)

print("Final angles [deg]: ", np.rad2deg(yD_arm))
print("Final angles [rad]: ", yD_arm)
print("Final EE position: ", T_c[:3, -1] + arm.base_offset)

# Set control parameters
env.set_control_torque(yD=list(yD_arm))

# Perform steps
while t < 35:
    # Perform step
    t, s = env.step(t_step=t_step)
    save(t, s)

attitude.plot(["angular_velocity", "torques", "energy", "euler_angles"])

arm.plot()

print("Supposed:\n", TD)
print("\nAfter IK:\n", T_c)
print("\nReached:\n", arm.get_transformation(s[:6], 6))

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

