import numpy as np

from attitude.torques.base import TorqueObject


class Electromagnet(object):
    mu0: float = 4 * np.pi * 1e-7

    def __init__(self, n_turns: int, radius: float, current: float):
        # Save physical properties
        self.current = current  # TODO: transform in terms of power (research electromagnets)
        self.n_turns = n_turns
        self.radius = radius
        self.area = np.pi * radius**2
        self.thickness = 0.005
        self.height = self.thickness * self.n_turns

        # Temporary
        self.locations = []
        self.moments = []

    def add_magnet(self, location: np.ndarray, moment: np.ndarray) -> None:
        # Add to lists
        self.locations.append(np.array(location))
        self.moments.append(np.array(moment) / np.linalg.norm(np.array(moment)))

    def magnetic_field(self) -> np.ndarray:
        """
        Compute magnetic field

        Returns:
        :return: magnetic field vector
        """
        # Init magnetic field
        B = np.zeros(3)

        # Magnitude
        mag: float = (self.mu0 * self.n_turns * self.current * self.area) / (4 * np.pi)

        # Loop through electromagnets
        for location, moment in zip(self.locations, self.moments):
            # Calculate distance
            r: float = np.linalg.norm(location)

            # Update field
            B += mag * ((np.dot(3*location, np.dot(moment, location)))/(r**5) - moment/(r**3))

        return B


class EddyCurrentTorque(object):
    def __new__(cls, *args, **kwargs) -> TorqueObject:
        # Create this torque's instance
        this_torque_instance = super().__new__(cls)

        # Return TorqueObject
        return TorqueObject(this_torque_instance, this_torque_instance.eval_torque, *args, **kwargs)

    def __init__(self, *, entity, chaser_w0: np.ndarray, electromagnet: Electromagnet):
        """
        Initializes the eddy current torque class.

        Parameters:
        :param entity: The target entity considered.
        :param chaser_w0: The initial angular velocity vector of the chaser spacecraft.
        :param electromagnet: electromagnet object to characterize the magnetic field produced.
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
        self.electromagnet = electromagnet

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
        B: np.ndarray = self.electromagnet.magnetic_field()

        return np.cross(np.dot(self.M_magn, np.cross(wr, B)), B)
