# Entities
import numpy as np

from robotics.arm_propagator import ArmPropagator, Joint
from utilities.entities import Cylinder, Planet

# Attitude
from attitude.attitude_propagator import AttitudePropagator
from attitude.torques.base import TorqueObject
from attitude.torques.eddy_current import EddyCurrentTorque, ElectromagnetEndEffector

# Orbit
from orbital.orbit_propagator import OrbitPropagator

# Propagator
from propagator import Propagator

# Generate Earth
earth = Planet(mu=3.98600433e+5, radius=6371.01)

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
joints_1 = [
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

# Instantiate propagator
debris_prop = Propagator(
    attitude=AttitudePropagator(entity=debris, w0=[0.2, 0.3, 0], q0=[0, 0, 0, 1], M_ext=eddy),
    orbit=OrbitPropagator(planet=earth, a0=(earth.radius+2000), e0=0.0, i0=10.0, OM0=0.0, om0=0.0, f0=0.0),
    robotics=[
        ArmPropagator(end_effector=electromagnet, y0=[0, 0, -7], p0=[-1, 0, 0])
    ]
)

# Propagate
debris_prop.load_from_csv("./propagator/prop_result.csv")

# Plots
# debris_prop.orbit.plot_orbit()
debris_prop.attitude.plot(["angular_velocity", "torques", "energy", "euler_angles"])

# Animate
# debris_prop.attitude.animate(dpi=300, arms=electromagnets)
