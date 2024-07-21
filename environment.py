# Environment
import cppyy
import numpy as np

from processing.attitude.attitude_propagator import AttitudePropagator
from processing.attitude.torques.base import TorqueObject
from processing.attitude.torques.eddy_current import EddyCurrentTorque
from processing.robotics.arm_propagator import ElectromagnetEndEffector, ArmPropagator, RevoluteJoint
from processing.utilities.entities import Cylinder

# Add include paths
cppyy.add_include_path('/opt/sundials/include')
cppyy.add_include_path('./propagator/lib/symengine/symengine')
cppyy.add_include_path('./propagator/lib/rapidjson/include')
cppyy.add_include_path('./propagator/lib/Eigen')
cppyy.add_include_path('/usr/include/python3.8')
cppyy.add_include_path('/home/whitehole/.local/lib/python3.8/site-packages/pybind11/include')

# Add library paths
cppyy.add_library_path('/opt/sundials/lib')

# Load libraries
cppyy.load_library('sundials_cvode')
cppyy.load_library('sundials_nvecserial')
cppyy.load_library('sundials_sunmatrixdense')
cppyy.load_library('sundials_sunlinsoldense')
cppyy.load_library('sundials_core')
cppyy.load_library('gmp')
cppyy.load_library('python3.8')

# Include local headers
cppyy.include("./propagator/propagator.h")

# Load local library
cppyy.load_library("./propagator/bin/libpropagator.so")

# Retrieve Class
Environment = cppyy.gbl.Environment

def save(tf, prop):
    # to numpy
    prop = np.array(prop)

    # save
    arm.save_new(tf, prop=prop)
    attitude.save_new(tf, prop=prop)

def random_y0():
    # Random joint angle
    ja = list(np.random.uniform(0, 2*np.pi, size=6))
    dw = list(np.random.uniform(-0.5, 0.2, size=3))
    y0r = ja + [0, 0, 0, 0, 0, 0] + dw + [0, 0, 0, 1]

    return y0r

# Generate debris
debris = Cylinder(
    mass=700.0,
    radius=2.6/2,
    height=7.372,
    thickness=0.1,
    sigma=35000000.0,
    Ix=28000,
    Iy=28000,
    Iz=3000,
    Mx=5.908e6,  # Magnetic tensor
    My=5.908e6,
    Mz=1.951e6
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
    current=60.0
)

# External moments
# Eddy current
eddy: TorqueObject = EddyCurrentTorque(
    electromagnets=[electromagnet]
)

# Save attitude results
attitude = AttitudePropagator(entity=debris, M_ext=eddy)

# Save robotic arm results
base_offset = np.array([7, 0, 0])
max_torques = np.array([.1, .1, .1, .1, .1, .1])
arm = ArmPropagator(joints=joints, com=com, end_effector=electromagnet, base_offset=base_offset, max_torques=max_torques)

# Set propagation settings
t_step = .1  # Propagation time step [s]

# Set initial conditions
y0_arm = [
    5, 3.14, 5, 1.5, 1.5, 1.5,      # Initial joint angles
    0.02, 0.0, 0.0, 0.0, 0.0, 0.0   # Initial joint velocities
]

y0_debris = [
    0.5, 0.5, 0.5,                    # Initial debris angular velocity
    0.0, 0.0, 0.0, 1.0                # Initial debris quaternions
]

y0_orbit = [
    7138, 0.0, 0.0,
    0.0, 7.46, 0.0
]

y0 = y0_arm + y0_debris + y0_orbit
# y0 = random_y0()

# Initialize environment
env = Environment(
    cppyy.gbl.std.vector[float](y0),                               # Initial conditions
    debris.Ixx,                                                    # Debris Ixx
    debris.Iyy,                                                    # Debris Iyy
    debris.Izz,                                                    # Debris Izz
    debris.Mxx,                                                    # Magnetic tensor xx
    debris.Myy,                                                    # Magnetic tensor yy
    debris.Mzz,                                                    # Magnetic tensor zz
    electromagnet.n_turns,                                         # Coil number of turns
    electromagnet.current,                                         # Coil current
    electromagnet.radius,                                          # Coil radius
    arm.base_offset_x,                                             # Arm base x-offset
    arm.base_offset_y,                                             # Arm base y-offset
    arm.base_offset_z,                                             # Arm base z-offset
    cppyy.gbl.std.vector[float](arm.dh_a),                         # Arm joint a parameters
    cppyy.gbl.std.vector[float](arm.dh_d),                         # Arm joint d parameters
    cppyy.gbl.std.vector[float](arm.dh_alpha),                     # Arm joint alpha parameters
    cppyy.gbl.std.vector[float](arm.max_torques),                  # Arm joint max torques
    cppyy.gbl.std.vector[cppyy.gbl.std.vector[float]]
    ([cppyy.gbl.std.vector[float](row) for row in arm.com])        # Arm link CoMs
)
