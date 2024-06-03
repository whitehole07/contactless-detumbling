import numpy as np

from processing.attitude.torques.base import TorqueObject
from processing.robotics.arm_propagator import ElectromagnetEndEffector


class EddyCurrentTorque(object):
    def __new__(cls, *args, **kwargs) -> TorqueObject:
        # Create this torque's instance
        this_torque_instance = super().__new__(cls)

        # Return TorqueObject
        return TorqueObject(this_torque_instance, None, *args, **kwargs)

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
        # Magnetic field

        self.electromagnets = electromagnets
        # Propagation history
        self.history = None