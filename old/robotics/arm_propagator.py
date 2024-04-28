import numpy as np


class IndexesNotSet(BaseException):
    pass


class ElectromagnetEndEffector(object):  # TODO: Create base parent class for any kind of end effector (not urgent)
    mu0: float = 4 * np.pi * 1e-7

    def __init__(self, n_turns: int, radius: float, current: float, pose_sign):
        # Save physical properties
        self.current = current  # TODO: transform in terms of power (research electromagnets)
        self.n_turns = n_turns
        self.radius = radius
        self.area = np.pi * radius ** 2
        self.thickness = 0.0005
        self.height = self.thickness * self.n_turns

        # Propagation indexes
        self.indexes: slice = None

        # Evolving parameters
        self._timestamps = None  # Propagation timestamps
        self.locations = None  # Propagation solution
        self.poses = None
        self.pose_sign = pose_sign

    def save(self, timestamps, solution) -> None:
        # Save solution
        self._timestamps = timestamps
        self.locations = solution

        # Generate moments
        norm = np.linalg.norm(self.locations, axis=0)
        self.poses = self.pose_sign * self.locations / norm  # TODO: to adapt if introduced robotic arms constraints

    def magnetic_field(self, _, y) -> np.ndarray:
        """
        Compute magnetic field

        Returns:
        :return: magnetic field vector
        """
        # Compute location and pose
        if self.indexes is not None:
            # Retrieve location
            location: np.ndarray = y[self.indexes]
            r: float = np.linalg.norm(location)

            # Compute moment
            moment: np.ndarray = self.pose_sign * location / r  # Normalized moment
        else:
            raise IndexesNotSet("ElectromagnetEndEffector propagation indexes not set")

        # Magnitude
        mag: float = (self.mu0 * self.n_turns * self.current * self.area) / (4 * np.pi)

        # Return magnetic field
        return mag * ((np.dot(3 * location, np.dot(moment, location))) / (r ** 5) - moment / (r ** 3))


class Joint(object):
    def __init__(self, theta: float, a: float, d: float, alpha: float, mass: float, com: np.ndarray) -> None:
        """
        Initialize the joint object

        :param theta: angle about previous z from old x to new x
        :param a: length of the common normal. Assuming a revolute joint, this is the radius about previous z.
        :param d: offset along previous z to the common normal
        :param alpha: angle about common normal, from old z axis to new z axis
        :param mass: mass of the link
        :param com: location of the center of mass of the link
        """
        # Save Denavit–Hartenberg parameters
        self.theta = theta
        self.a = a
        self.d = d
        self.alpha = alpha
        self.mass = mass
        self.com = com

    def dh_transform(self):
        """
        Compute the Denavit-Hartenberg transformation matrix.
        """
        return np.array([
            [np.cos(self.theta), -np.sin(self.theta) * np.cos(self.alpha), np.sin(self.theta) * np.sin(self.alpha), self.a * np.cos(self.theta)],
            [np.sin(self.theta), np.cos(self.theta) * np.cos(self.alpha), -np.cos(self.theta) * np.sin(self.alpha), self.a * np.sin(self.theta)],
            [0, np.sin(self.alpha), np.cos(self.alpha), self.d],
            [0, 0, 0, 1]
        ])


class Arm(object):
    def __init__(self, joints: list, end_effector: ElectromagnetEndEffector):
        self.joints = joints
        self.end_effector = end_effector


class ArmPropagator(object):
    _count: int = 0

    def __new__(cls, *args, **kwargs):
        cls._count += 1
        return super().__new__(cls)

    def __init__(self, *, end_effector, y0: np.ndarray, p0: np.ndarray) -> None:
        """
        Initialize the robotic arm end effector.

        Parameters:
        - y0: initial location of the end effector
        - p0: initial pose of the end effector
        - end_effector: end effector object
        """
        # Save parameters
        self.y0 = y0
        self.p0 = p0
        self.end_effector = end_effector

        # Propagation indexes
        self.indexes = slice(13 + ((self._count - 1) * 3), 16 + ((self._count - 1) * 3))
        self.end_effector.indexes = self.indexes

        # Evolving parameters
        self._timestamps = None  # Propagation timestamps
        self._prop_sol = None  # Propagation solution

    def save(self, timestamps, solution) -> None:
        # Save at arm level
        self._timestamps = timestamps
        self._prop_sol = solution

        # Save at end effector level
        self.end_effector.save(timestamps, solution)

    def propagate_function(self, t, _) -> list:
        w = 0 #0.005  # Angular velocity
        r = 8

        if self.end_effector.pose_sign == 1:
            # End effector's location (pose fixed)
            dx = -r * w * np.sin(w * t)
            dy = r * w * np.cos(w * t)
        else:
            # End effector's location (pose fixed)
            dx = r * w * np.sin(w * t)
            dy = -r * w * np.cos(w * t)
        dz = 0
        return [dx, dy, dz]
