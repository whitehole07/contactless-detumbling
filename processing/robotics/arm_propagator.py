import numpy as np

from matplotlib import pyplot as plt

class IndexesNotSet(BaseException):
    pass


class ElectromagnetEndEffector(object):
    mu0: float = 4 * np.pi * 1e-7

    def __init__(self, n_turns: int, radius: float, current: float):
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


class ArmPropagator(object):
    _count: int = 0

    def __new__(cls, *args, **kwargs):
        cls._count += 1
        return super().__new__(cls)

    def __init__(self, *, joints: np.ndarray, end_effector: ElectromagnetEndEffector, base_offset: np.ndarray) -> None:
        """
        Initialize the robotic arm end effector.

        Parameters:
        - y0: initial location of the end effector
        - p0: initial pose of the end effector
        - end_effector: end effector object
        """
        # Save parameters
        self.joints = joints
        self.end_effector = end_effector
        self.base_offset = base_offset

        # Evolving parameters
        self._timestamps = None  # Propagation timestamps
        self._prop_sol = None  # Propagation solution

    def plot(self) -> None:
        # Get size of the problem
        n: int = len(self.joints)

        # Create figure
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))

        # Plot angular evolution
        ax1.plot(self._timestamps, np.rad2deg(self._prop_sol[:n, :].T), linewidth=2)
        ax1.set_xlabel("Time [s]")
        ax1.set_ylabel("Joint angle [deg]")
        ax1.legend([r"$\theta_1$", r"$\theta_2$", r"$\theta_3$", r"$\theta_4$", r"$\theta_5$", r"$\theta_6$"],
                   loc="upper right", fontsize="small")
        ax1.grid(True)
        ax1.set_title("Evolution of Joint Angles")

        # Plot joint velocities
        ax2.plot(self._timestamps, np.rad2deg(self._prop_sol[n:, :].T), linewidth=2)
        ax2.set_xlabel("Time [s]")
        ax2.set_ylabel("Joint angular velocity [deg/s]")
        ax2.legend(
            [r"$\dot{\theta}_1$", r"$\dot{\theta}_2$", r"$\dot{\theta}_3$", r"$\dot{\theta}_4$", r"$\dot{\theta}_5$",
             r"$\dot{\theta}_6$"], loc="upper right", fontsize="small")
        ax2.grid(True)
        ax2.set_title("Evolution of Joint Angular Velocities")

        plt.tight_layout()  # Improve spacing
        plt.show()
