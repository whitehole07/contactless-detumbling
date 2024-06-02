import math

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


class RevoluteJoint(object):
    def __init__(self, a: float, d: float, alpha: float) -> None:
        """
        Initialize the revolute joint object

        :param a: length of the common normal. Assuming a revolute joint, this is the radius about previous z.
        :param d: offset along previous z to the common normal
        :param alpha: angle about common normal, from old z axis to new z axis
        """
        # Save Denavit–Hartenberg parameters
        self.a = a
        self.d = d
        self.alpha = alpha

    def dh_transform(self, theta: float) -> np.ndarray:
        """
        Compute the Denavit-Hartenberg transformation matrix.
        """
        return np.array([
            [np.cos(theta), -np.sin(theta) * np.cos(self.alpha), np.sin(theta) * np.sin(self.alpha), self.a * np.cos(theta)],
            [np.sin(theta), np.cos(theta) * np.cos(self.alpha), -np.cos(theta) * np.sin(self.alpha), self.a * np.sin(theta)],
            [0, np.sin(self.alpha), np.cos(self.alpha), self.d],
            [0, 0, 0, 1]
        ])


class ArmPropagator(object):
    _count: int = 0

    def __new__(cls, *args, **kwargs):
        cls._count += 1
        return super().__new__(cls)

    def __init__(self, *, joints: list, com:list, end_effector: ElectromagnetEndEffector, base_offset: np.ndarray, max_torques: np.ndarray) -> None:
        """
        Initialize the robotic arm end effector.

        Parameters:
        - y0: initial location of the end effector
        - p0: initial pose of the end effector
        - end_effector: end effector object
        """
        # Save parameters
        self.joints: list = joints
        self.com: list = com
        self.end_effector = end_effector
        self.base_offset: np.ndarray = base_offset

        # Max torque
        self.__max_torques: np.ndarray = max_torques

        # Evolving parameters
        self._timestamps = None  # Propagation timestamps
        self._prop_sol = None  # Propagation solution
        self.joint_torques = None

    @property
    def dh_a(self) -> list:
        return [joint.a for joint in self.joints]

    @property
    def dh_d(self) -> list:
        return [joint.d for joint in self.joints]

    @property
    def dh_alpha(self) -> list:
        return [joint.alpha for joint in self.joints]

    @property
    def base_offset_x(self) -> float:
        return float(self.base_offset[0])

    @property
    def base_offset_y(self) -> float:
        return float(self.base_offset[1])

    @property
    def base_offset_z(self) -> float:
        return float(self.base_offset[2])

    @property
    def max_torques(self) -> list:
        return list(self.__max_torques)

    def save_new(self, t: float, prop: np.ndarray) -> None:
        if self._timestamps is None:
            self._timestamps = np.array([t])
            self._prop_sol = prop[0:12].reshape(-1, 1)
            self.joint_torques = prop[25:31].reshape(-1, 1)
            self.end_effector.locations = prop[19:22].reshape(-1, 1)
            self.end_effector.poses = prop[22:25].reshape(-1, 1)
        else:
            self._timestamps = np.hstack((self._timestamps, np.array([t])))
            self._prop_sol = np.hstack((self._prop_sol, prop[0:12].reshape(-1, 1)))
            self.joint_torques = np.hstack((self.joint_torques, prop[25:31].reshape(-1, 1)))
            self.end_effector.locations = np.hstack((self.end_effector.locations, prop[19:22].reshape(-1, 1)))
            self.end_effector.poses = np.hstack((self.end_effector.poses, prop[22:25].reshape(-1, 1)))

    def get_transformation(self, thetas: np.ndarray, joint_number: int) -> np.ndarray:
        """ Compute the transformation matrix for the i-th joint. """
        T: np.ndarray = np.eye(4, 4)
        for i in range(joint_number):
            T = T @ self.joints[i].dh_transform(thetas[i])
        return T

    def get_joint_position(self, thetas: np.ndarray, joint_number: int) -> np.ndarray:
        # Get transformation matrix
        T: np.ndarray = self.get_transformation(thetas, joint_number)

        # Extract joint position
        return self.base_offset + T[:3, -1]

    def inverse_kinematics(self, T_desired, q_initial, threshold, max_iter):
        # Initialize joint angles
        q = q_initial

        # Iterate until convergence or maximum iterations reached
        for iter in range(max_iter):
            # Compute end-effector transformation matrix based on current joint angles
            T_current = self.get_transformation(q, 6)

            # Compute error in end-effector pose
            error = self.compute_error(T_desired, T_current)

            # Check if error is below threshold
            if np.linalg.norm(error) < threshold:
                print('Converged: ', error)
                break

            # Compute Jacobian matrix
            J_current = self.compute_jacobian(q)

            # Compute pseudo-inverse of Jacobian matrix
            J_pseudo_inv = np.linalg.pinv(J_current)

            # Compute joint angle increments
            delta_q = J_pseudo_inv @ error

            # Update joint angles
            q = np.mod(q + delta_q, 2*np.pi)
        else:
            raise ArithmeticError("Inverse kinematics did not converge within max_iter iterations.")
        return q

    def compute_error(self, T_desired, T_current):
        R_current = T_current[:3, :3]
        R_desired = T_desired[:3, :3]

        R_error = R_desired.T @ R_current
        axis, angle = self.custom_rotm2axang(R_error)

        r_error = T_desired[:3, 3] - T_current[:3, 3]

        error = np.concatenate((r_error, angle * axis))

        return error

    @staticmethod
    def custom_rotm2axang(R):
        assert R.shape == (3, 3), 'Input must be a 3x3 matrix'

        trace_R = np.trace(R)

        if trace_R > 2 - np.finfo(float).eps:
            angle = 0
            axis = np.array([0, 0, 1])
        elif trace_R < -1 + np.finfo(float).eps:
            i = np.argmax([R[0, 0], R[1, 1], R[2, 2]])
            v = np.sqrt((R[i, i] + 1) / 2) * np.array([R[0, i], R[1, i], R[2, i]]) / 2
            angle = np.pi
            axis = v / np.linalg.norm(v)
        else:
            angle = np.arccos((trace_R - 1) / 2)
            axis = 1 / (2 * np.sin(angle)) * np.array([R[2, 1] - R[1, 2], R[0, 2] - R[2, 0], R[1, 0] - R[0, 1]])

        return axis, angle

    def compute_jacobian(self, q):
        n = len(q)

        JPi = np.zeros((3, n))
        JOi = np.zeros((3, n))

        T0i = self.get_transformation(q, 6)
        pli = T0i[:3, 3] + (T0i[:3, :3] @ self.com[n - 1])

        T0jp = np.eye(4)
        for j in range(n):
            JPi[:, j] = np.cross(T0jp[:3, 2], pli - T0jp[:3, 3])
            JOi[:, j] = T0jp[:3, 2]
            T0jp = self.get_transformation(q, j+1)

        J = np.vstack((JPi, JOi))

        return J

    def compute_end_effector_linang_velocity(self, q, q_dot) -> np.ndarray:
        # Compute Jacobian
        J = self.compute_jacobian(q)

        # Compute velocities
        return np.dot(J, q_dot)

    def plot(self) -> None:
        # Get size of the problem
        n: int = len(self.joints)

        # Create figure
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(10, 10))

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

        # Plot joint torques
        ax3.plot(self._timestamps, self.joint_torques.T, linewidth=2)
        ax3.set_xlabel("Time [s]")
        ax3.set_ylabel("Joint torques [Nm]")
        ax3.legend(
            [r"$\tau_1$", r"$\tau_2$", r"$\tau_3$", r"$\tau_4$", r"$\tau_5$",
             r"$\tau_6$"], loc="upper right", fontsize="small")
        ax3.grid(True)
        ax3.set_title("Evolution of Joint Torques")

        # Plot end effector position
        ax4.plot(self._timestamps, self.end_effector.locations.T, linewidth=2)
        ax4.set_xlabel("Time [s]")
        ax4.set_ylabel("End effector position [m]")
        ax4.legend([r"$x_e$", r"$y_e$", r"$z_e$"], loc="upper right", fontsize="small")
        ax4.grid(True)
        ax4.set_title("Evolution of End effector position")

        plt.tight_layout()  # Improve spacing
        plt.show()
