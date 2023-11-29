from attitude_propagator import AttitudePropagator
from entities import Cylinder
from torques.eddy_current import EddyCurrentTorque

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
ect = EddyCurrentTorque(
    entity=debris,
    w0c=[0.0, 0.0, 0.0],
    B=[2e-4, 2e-4, 2e-4]
)

# Instantiate propagator
debris_prop = AttitudePropagator(
    entity=debris,
    w0=[0.5, 0.5, 0.5],
    q0=[1.0, 0.0, 0.0, 0.0]
)

# Propagate
debris_prop.propagate(M_ext=ect, t_span=[0, 700], eval_points=700)

# Plot evolution
debris_prop.plot_evolution()
# debris_prop.plot_quaternions()

# Animate evolution
debris_prop.animate_evolution()
