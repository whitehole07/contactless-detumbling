from matplotlib import pyplot as plt
from scipy.integrate import solve_ivp
import numpy as np

from attitude.attitude_conversion import quaternion_to_euler
from attitude.attitude_animation import animate_attitude


class AttitudePropagator(object):
    def __init__(self, *, entity, w0: np.ndarray, q0: np.ndarray, M_ext) -> None:
        # Entity
        self.__entity = entity
        self.__I = self.__entity.inertia_matrix

        # Initial parameters
        self.w0: np.ndarray = np.array(w0)                                           # Initial angular velocity
        self.q0: np.ndarray = np.array(q0) / np.linalg.norm(q0)  # Initial quaternions (normalized)
        self.y0: np.ndarray = np.concatenate((self.w0, self.q0))                             # Overall initial conditions

        # External torque
        self.__ext_torque = M_ext

        # Evolving parameters
        self._timestamps = None  # Propagation timestamps
        self._prop_sol = None  # Propagation solution

    @property
    def t(self) -> np.ndarray:
        return self._timestamps

    @property
    def w(self) -> np.ndarray:
        return self._prop_sol[:3, :]

    @property
    def q(self) -> np.ndarray:
        return self._prop_sol[3:, :]

    @property
    def euler_angles(self) -> np.ndarray:
        converted = np.array([])
        for quaternion in self.q.T:
            # Convert quaternion to euler angles
            euler = np.rad2deg(quaternion_to_euler(quaternion))

            # Check if converted is still empty
            if not converted.size:
                converted = euler
            else:
                converted = np.hstack((converted, euler))
        return converted

    def propagate_function(self, t, y) -> list:
        M = self.__ext_torque(t, y)  # Evaluate external moments at timestep t
        w = y[:3]                    # Extract angular velocities
        q = y[3:]                    # Extract quaternions

        # Normalize quaternions
        q = q / np.linalg.norm(q)

        dwx = (M[0] - (self.__I[2, 2] - self.__I[1, 1]) * w[2] * w[1]) / self.__I[0, 0]
        dwy = (M[1] - (self.__I[0, 0] - self.__I[2, 2]) * w[0] * w[2]) / self.__I[1, 1]
        dwz = (M[2] - (self.__I[1, 1] - self.__I[0, 0]) * w[1] * w[0]) / self.__I[2, 2]
        dq1 = 0.5 * (w[2] * q[1] - w[1] * q[2] + w[0] * q[3])
        dq2 = 0.5 * (-w[2] * q[0] + w[0] * q[2] + w[1] * q[3])
        dq3 = 0.5 * (w[1] * q[0] - w[0] * q[1] + w[2] * q[3])
        dq4 = 0.5 * (-w[0] * q[0] - w[1] * q[1] - w[2] * q[2])
        return [dwx, dwy, dwz, dq1, dq2, dq3, dq4]

    def plot_evolution(self) -> None:
        # Retrieve results
        t = self.t
        w = self.w
        euler = self.euler_angles

        # Print result
        # Create a figure and axis with custom style
        fig, ax = plt.subplots(1, 2, figsize=(10, 5))
        ax[0].set_xlabel('Time [s]')
        ax[0].set_ylabel('Angular Velocity [rad/s]')

        # Plot the vectors with customized appearance
        ax[0].plot(t, w[0, :], label='$\omega_x$', color='blue', linestyle='-')
        ax[0].plot(t, w[1, :], label='$\omega_y$', color='green', linestyle='--')
        ax[0].plot(t, w[2, :], label='$\omega_z$', color='red', linestyle='-.')

        # Add a legend
        ax[0].legend()

        # Add a title and grid
        ax[0].set_title('Angular Velocities')
        ax[0].grid(True)

        # Angles plot
        ax[1].set_xlabel('Time [s]')
        ax[1].set_ylabel('Angle [deg]')

        # Plot the vectors with customized appearance
        ax[1].plot(t, euler[0, :], label=r'$\theta_{roll}$', color='blue', linestyle='-')
        ax[1].plot(t, euler[1, :], label=r'$\theta_{pitch}$', color='green', linestyle='--')
        ax[1].plot(t, euler[2, :], label=r'$\theta_{yaw}$', color='red', linestyle='-.')

        # Add a legend
        ax[1].legend()

        # Add a title and grid
        ax[1].set_title('Euler Angles')
        ax[1].grid(True)

        # Show the plot
        plt.tight_layout()  # Improve spacing
        plt.show()

    def plot_quaternions(self) -> None:
        # Retrieve results
        t = self.t
        q = self.q

        # Print result
        # Create a figure and axis with custom style
        fig, ax = plt.subplots(figsize=(10, 5))
        ax.set_xlabel('Time [s]')
        ax.set_ylabel('Quaternion Magnitude')

        # Plot the vectors with customized appearance
        ax.plot(t, q[0, :], label='$q_1$', color='blue', linestyle='-')
        ax.plot(t, q[1, :], label='$q_2$', color='green', linestyle='--')
        ax.plot(t, q[2, :], label='$q_3$', color='red', linestyle='-.')
        ax.plot(t, q[3, :], label='$q_4$', color='purple', linestyle=':')

        # Add a legend
        ax.legend()

        # Add a title and grid
        ax.set_title('Quaternions')
        ax.grid(True)

        # Show the plot
        plt.tight_layout()  # Improve spacing
        plt.show()

    def animate_evolution(self) -> None:
        # Plot Cylinder attitude
        animate_attitude(
            self.t,
            self.q,
            self.euler_angles,
            self.__entity.height,
            self.__entity.radius
        )
