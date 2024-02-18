# Entities
from utilities.entities import Cylinder, Planet

# Attitude
from attitude.attitude_propagator import AttitudePropagator
from attitude.torques.base import TorqueObject
from attitude.torques.eddy_current import EddyCurrentTorque, Electromagnet
from attitude.torques.gravity_gradient import GravityGradientTorque

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

# External moments
# Eddy Currents
electromagnet: Electromagnet = Electromagnet(
    n_turns=100,
    radius=2,
    current=800,
)

electromagnet.add_magnet(location=[0, 0, 8], moment=[0, 1, 1])
electromagnet.add_magnet(location=[0, 0, -8], moment=[0, 1, 1])

eddy: TorqueObject = EddyCurrentTorque(
    entity=debris,
    chaser_w0=[0.0, 0.0, 0.0],
    electromagnet=electromagnet
)

# Gravity Gradient
gravity: TorqueObject = GravityGradientTorque(
    entity=debris,
    planet=earth
)

# Instantiate propagator
debris_prop = Propagator(
    attitude=AttitudePropagator(entity=debris, w0=[0.1, 0.05, 0.0], q0=[0, 0, 0, 1], M_ext=eddy+gravity),
    orbit=OrbitPropagator(planet=earth, a0=(earth.radius+2000), e0=0.0, i0=10.0, OM0=0.0, om0=0.0, f0=0.0)
)

# Propagate
debris_prop.propagate(t_span=[0, 150], eval_points=150)

# Plots
# debris_prop.orbit.plot_orbit()
debris_prop.attitude.plot(["angular_velocity", "torques", "energy", "euler_angles"])

# Animate
debris_prop.attitude.animate(dpi=300, magnet=electromagnet)
