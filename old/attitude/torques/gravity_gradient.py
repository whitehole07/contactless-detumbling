import numpy as np

from old.attitude.attitude_conversion import quaternion_to_rotation_matrix
from old.attitude.torques.base import TorqueObject


class GravityGradientTorque(object):
    def __new__(cls, *args, **kwargs) -> TorqueObject:
        # Create this torque's instance
        this_torque_instance = super().__new__(cls)

        # Return TorqueObject
        return TorqueObject(this_torque_instance, this_torque_instance.eval_torque, *args, **kwargs)

    def __init__(self, *, entity, planet):
        """
        Initializes the eddy current torque class.

        Parameters:
        - entity: The target entity considered.
        - planet: Planet orbited.

        Returns:
        None
        """
        # Torque name
        self.name = "Gravity Gradient"

        # Entities
        self.entity = entity
        self.planet = planet

        # Propagation history
        self.history = None

    def eval_torque(self, t, y) -> np.ndarray:
        # Get inertia
        Ix, Iy, Iz = np.diag(self.entity.inertia_matrix)

        # Get distance
        r = y[7:10]
        R = np.linalg.norm(r)

        # Direction cosines
        r_body = np.dot(quaternion_to_rotation_matrix(y[3:7]), r)
        r_body_norm = r_body / R
        c1, c2, c3 = r_body_norm

        # Compute torque
        return 3*(self.planet.mu/(R**3)) * np.array([(Iz - Iy)*c2*c3, (Ix - Iz)*c1*c3, (Iy - Ix)*c1*c2])
