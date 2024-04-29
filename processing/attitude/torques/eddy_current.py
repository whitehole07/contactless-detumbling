import numpy as np

from processing.attitude.torques.base import TorqueObject
from processing.robotics.arm_propagator import ElectromagnetEndEffector


class EddyCurrentTorque(object):
    def __new__(cls, *args, **kwargs) -> TorqueObject:
        # Create this torque's instance
        this_torque_instance = super().__new__(cls)

        # Return TorqueObject
        return TorqueObject(this_torque_instance, this_torque_instance.eval_torque, *args, **kwargs)

    def __init__(self, *, entity, chaser_w0: np.ndarray, electromagnets: list):
        """
        Initializes the eddy current torque class.

        Parameters:
        :param entity: The target entity considered.
        :param chaser_w0: The initial angular velocity vector of the chaser spacecraft.
        :param electromagnets: electromagnet object to characterize the magnetic field produced.
        """
        # Torque name
        self.name = "Eddy Current"

        # Chaser attitude and magnetic field
        # TODO: to be considered:
        # - create object also for chaser (?)
        # - propagate its attitude (consider interaction with target) (?)
        # - model magnetic field with higher accuracy (!)
        # - Consider effective magnetic tensor
        self.w0c: np.ndarray = np.array(chaser_w0)  # In the target's body reference frame

        # Magnetic field
        self.electromagnets = electromagnets

        # Compute gamma
        gamma: float = 1 - (2*entity.radius/entity.height) * np.tanh(entity.height/(2*entity.radius))

        # Compute magnetic tensor
        self.M_magn: np.ndarray = (
            np.pi * entity.sigma * entity.radius**3 * entity.thickness * entity.height
        ) * np.diag([gamma, gamma, 1/2])

        # Propagation history
        self.history = None

    def eval_torque(self, t, y) -> np.ndarray:
        # Relative angular velocity
        wr: np.ndarray = y[:3] - self.w0c

        # Evaluate magnetic field
        B: np.ndarray = sum([electromagnet.magnetic_field(t, y) for electromagnet in self.electromagnets])

        return np.cross(np.dot(self.M_magn, np.cross(wr, B)), B)
