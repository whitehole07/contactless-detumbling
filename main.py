from attitude.attitude_propagator import AttitudePropagator
from utilities.entities import Cylinder
from attitude.torques import TorqueObject
from attitude.torques import EddyCurrentTorque

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
    chaser_init_omega=[0.0, 0.0, 0.0],
    magnetic_field=[2e-4, 2e-4, 2e-4]
)

# Instantiate propagator
debris_prop = AttitudePropagator(
    entity=debris,
    init_omega=[0.5, 0.5, 0.5],
    init_quaternions=[1.0, 0.0, 0.0, 0.0]
)

# Propagate
debris_prop.propagate(ext_torque=eddy, t_span=[0, 700], eval_points=700)

# Plot evolution
debris_prop.plot_evolution()
# debris_prop.plot_quaternions()

# Animate evolution
debris_prop.animate_evolution()
