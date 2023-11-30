# Entities
from utilities.entities import Cylinder, Planet

# Attitude
from attitude.attitude_propagator import AttitudePropagator
from attitude.torques.base import TorqueObject
from attitude.torques.eddy_current import EddyCurrentTorque

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
eddy: TorqueObject = EddyCurrentTorque(
    entity=debris,
    chaser_w0=[0.0, 0.0, 0.0],
    magnetic_field=[2e-4, 2e-4, 2e-4]
)

# Instantiate propagator
debris_prop = Propagator(
    attitude=AttitudePropagator(entity=debris, w0=[0.5, 0.5, 0.5], q0=[1.0, 0.0, 0.0, 0.0], M_ext=eddy),
    orbit=OrbitPropagator(planet=earth, a0=(earth.radius+2000), e0=0.0, i0=10.0, OM0=0.0, om0=0.0, f0=0.0)
)

# Propagate
debris_prop.propagate(t_span=[0, 500], eval_points=500)

# Plot evolution
debris_prop.attitude.plot_evolution()
# debris_prop.attitude.plot_quaternions()
debris_prop.orbit.plot_orbit()

# Animate evolution
# debris_prop.attitude.animate_evolution()
