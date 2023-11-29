from matplotlib import pyplot as plt
from scipy.integrate import solve_ivp
import numpy as np

from attitude.attitude_conversion import quaternion_to_euler
from attitude.attitude_anim import animate_attitude


class QuaternionError(BaseException):
    pass


class AttitudePropagator(object):
    def __init__(self, *, entity, init_omega: np.ndarray, init_quaternions: np.ndarray) -> None:
        # Entity
        self.__entity = entity
        self.__I = self.__entity.inertia_matrix

        # Initial parameters
        self.__w0: np.ndarray = np.array(init_omega)                       # Initial angular velocity
        self.__q0: np.ndarray = np.array(init_quaternions) / np.linalg.norm(init_quaternions)  # Initial quaternions (normalized)

        # Evolving parameters
        self.__prop_sol = None  # Propagation solution

    @property
    def t(self) -> np.ndarray:
        return self.__prop_sol.t

    @property
    def w(self) -> np.ndarray:
        return self.__prop_sol.y[:3, :]

    @property
    def q(self) -> np.ndarray:
        return self.__prop_sol.y[3:, :]

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

    def propagate(self, *, ext_torque, t_span: np.ndarray, eval_points: int = 1000) -> None:
        # Save local inertia matrix
        In = self.__I

        # Define Euler equations
        def euler_equations(t, y):
            M = ext_torque(t, y)  # Evaluate external moments at timestep t
            w = y[:3]             # Extract angular velocities
            q = y[3:]             # Extract quaternions

            # Normalize quaternions
            q = q / np.linalg.norm(q)

            dwx = (M[0] - (In[2, 2] - In[1, 1]) * w[2] * w[1]) / In[0, 0]
            dwy = (M[1] - (In[0, 0] - In[2, 2]) * w[0] * w[2]) / In[1, 1]
            dwz = (M[2] - (In[1, 1] - In[0, 0]) * w[1] * w[0]) / In[2, 2]
            dq1 = 0.5 * (w[2] * q[1] - w[1] * q[2] + w[0] * q[3])
            dq2 = 0.5 * (-w[2] * q[0] + w[0] * q[2] + w[1] * q[3])
            dq3 = 0.5 * (w[1] * q[0] - w[0] * q[1] + w[2] * q[3])
            dq4 = 0.5 * (-w[0] * q[0] - w[1] * q[1] - w[2] * q[2])
            return [dwx, dwy, dwz, dq1, dq2, dq3, dq4]

        # Retrieve angular velocities
        self.__prop_sol = solve_ivp(euler_equations, t_span, [*self.__w0, *self.__q0],
                                    t_eval=np.linspace(*t_span, eval_points), method="Radau")

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
