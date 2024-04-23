# Entities
import numpy as np

from post_processing.robotics.arm_propagator import ArmPropagator, Joint
from post_processing.utilities.entities import Cylinder

# Attitude
from post_processing.attitude.attitude_propagator import AttitudePropagator
from post_processing.attitude.torques.base import TorqueObject
from post_processing.attitude.torques.eddy_current import EddyCurrentTorque, ElectromagnetEndEffector


# Generate debris
debris = Cylinder(
    mass=950,
    radius=2.5,
    height=5,
    thickness=0.1,
    sigma=35000000
)

# Generate robotic arms (UR10 standard)
# Joints
joints = [
    Joint(0, 0, 0.1273, np.pi/2, 7.1, [0.021, 0.000, 0.027]),
    Joint(0, -0.612, 0, 0, 12.7, [0.38, 0.000, 0.158]),
    Joint(0, -0.5723, 0, 0, 4.27, [0.24, 0.000, 0.068]),
    Joint(0, 0, 0.163941, np.pi/2, 2, [0.000, 0.007, 0.018]),
    Joint(0, 0, 0.1157, -np.pi/2, 2, [0.000, 0.007, 0.018]),
    Joint(0, 0, 0.0922, 0, 0.365, [0, 0, -0.026])
]  # Joints first robotic arm

# End effector
electromagnet: ElectromagnetEndEffector = ElectromagnetEndEffector(
    n_turns=500,
    radius=2,
    current=50,
    pose_sign=-1
)
electromagnets = [electromagnet]

# External moments
# Eddy current
eddy: TorqueObject = EddyCurrentTorque(
    entity=debris,
    chaser_w0=[0.0, 0.0, 0.0],
    electromagnets=electromagnets
)

# Load propagation results
csv = np.genfromtxt("./propagator/prop_result.csv", delimiter=',', skip_header=1)[:, :-1]

# Save attitude results
attitude = AttitudePropagator(entity=debris, M_ext=eddy)
attitude._timestamps = csv[:, 0]
attitude._prop_sol = csv[:, 13:20].T

# Save robotic arm results
robotics = ArmPropagator(joints=joints, end_effector=electromagnet)
robotics._timestamps = csv[:, 0]
robotics._prop_sol = csv[:, 1:13].T

# Plots
attitude.plot(["angular_velocity", "quaternions", "energy", "euler_angles"])

# Animate
attitude.animate(dpi=300, arms=electromagnets)
