# Entities
import numpy as np

from post_processing.robotics.arm_propagator import ArmPropagator, ElectromagnetEndEffector
from post_processing.system_animation import animate_system
from post_processing.utilities.entities import Cylinder

# Attitude
from post_processing.attitude.attitude_propagator import AttitudePropagator
from post_processing.attitude.torques.base import TorqueObject
from post_processing.attitude.torques.eddy_current import EddyCurrentTorque

# Load params from C++ settings
prop = np.genfromtxt("./propagator/csv/prop_result.csv", delimiter=',', skip_header=1)[:, :-1]
dh = np.genfromtxt("./propagator/csv/denavit_hartenberg.csv", delimiter=',', skip_header=1)[:, :-1]
sett = np.genfromtxt("./propagator/csv/settings.csv", delimiter=',', skip_header=1)[:-1]

# Generate debris
debris = Cylinder(
    mass=sett[0],
    radius=sett[1],
    height=sett[2],
    thickness=sett[3],
    sigma=sett[4]
)

# Generate robotic arms (UR10 standard)
# Joints
joints = np.hstack((np.zeros((dh.shape[0], 1)), dh,))

# End effector
electromagnet: ElectromagnetEndEffector = ElectromagnetEndEffector(
    n_turns=sett[5],
    radius=sett[6],
    current=sett[7]
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
attitude._timestamps = prop[:, 0]
attitude._prop_sol = prop[:, 13:20].T

# Save robotic arm results
arm = ArmPropagator(joints=joints, end_effector=electromagnet, base_offset=np.array([sett[8], sett[9], sett[10]]))
arm._timestamps = prop[:, 0]
arm._prop_sol = prop[:, 1:13].T

# Save end effector results
arm.end_effector._timestamps = prop[:, 0]
arm.end_effector.locations = prop[:, 20:23].T
arm.end_effector.poses = prop[:, 23:26].T

# Plots
attitude.plot(["angular_velocity", "quaternions", "energy", "euler_angles"])

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
