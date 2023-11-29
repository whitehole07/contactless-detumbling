import numpy as np

from torques.base import TorqueObject


class EddyCurrentTorque(object):
    def __new__(cls, *args, **kwargs) -> TorqueObject:
        # Create Eddy Currents instance
        eddy_current_instance = super(EddyCurrentTorque, cls).__new__(cls)

        # Return TorqueObject
        return TorqueObject(eddy_current_instance.eval_torque)

    def __init__(self, *, entity, w0c: np.ndarray, B: np.ndarray):
        """
        Initializes the eddy current torque class.

        Parameters:
        - entity: The target entity considered
        - w0c: The initial angular velocity vector of the chaser spacecraft.

        Returns:
        None
        """

        # Chaser attitude and magnetic field
        # TODO: to be considered:
        # - create object also for chaser (?)
        # - propagate its attitude (consider interaction with target) (?)
        # - model magnetic field with higher accuracy (!)
        self.w0c: np.ndarray = np.array(w0c)  # In the target's body reference frame
        self.B: np.ndarray = np.array(B)

        # Compute gamma
        gamma: float = 1 - (2*entity.radius/entity.height) * np.tanh(entity.height/(2*entity.radius))

        # Compute magnetic tensor
        self.M_magn: np.ndarray = (
                np.pi * entity.sigma * entity.radius**3 * entity.thickness * entity.height
        ) * np.diag([gamma, gamma, 1/2])

    def eval_torque(self, t, y) -> np.ndarray:
        # Relative angular velocity
        wr: np.ndarray = y[:3] - self.w0c

        # Compute torque
        return np.cross(np.dot(self.M_magn, np.cross(wr, self.B)), self.B)
