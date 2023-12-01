import numpy as np

from attitude.attitude_conversion import quaternion_to_euler
from attitude.attitude_animation import animate_attitude
from attitude.attitude_plot import plot_evolution
from attitude.torques.base import TorqueObject



class AttitudePropagator(object):
    def __init__(self, *, entity, w0: np.ndarray, q0: np.ndarray, M_ext) -> None:
        # Entity
        self._entity = entity
        self.__I = self._entity.inertia_matrix

        # Initial parameters
        self.w0: np.ndarray = np.array(w0)                                           # Initial angular velocity
        self.q0: np.ndarray = np.array(q0) / np.linalg.norm(q0)  # Initial quaternions (normalized)
        self.y0: np.ndarray = np.concatenate((self.w0, self.q0))                             # Overall initial conditions

        # External torque
        self._ext_torque: TorqueObject = M_ext

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
        return self._prop_sol[3:7, :]

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
        M = self._ext_torque(t, y)  # Evaluate external moments at timestep t
        w = y[0:3]                    # Extract angular velocities
        q = y[3:7]                   # Extract quaternions

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

    def plot(self, quantities: list, ncols: int = 2) -> None:
        plot_evolution(self, quantities, ncols)

    def animate(self, dpi: int = 300) -> None:
        # Plot Cylinder attitude
        animate_attitude(
            self.t,
            self.q,
            self.euler_angles,
            self._entity.height,
            self._entity.radius,
            dpi
        )
