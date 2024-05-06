import numpy as np

from processing.attitude.attitude_conversion import quaternion_to_euler
from processing.attitude.attitude_plot import plot_evolution
from processing.attitude.torques.base import TorqueObject


class AttitudePropagator(object):
    def __init__(self, *, entity, M_ext) -> None:
        # Entity
        self._entity = entity

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

    def save_new(self, t: float, prop: np.ndarray) -> None:
        # Save attitude
        if self._prop_sol is None:
            self._timestamps = np.array([t])
            self._prop_sol = prop[12:19].reshape(-1, 1)
        else:
            self._timestamps = np.hstack((self._timestamps, np.array([t])))
            self._prop_sol = np.hstack((self._prop_sol, prop[12:19].reshape(-1, 1)))

    def plot(self, quantities: list, ncols: int = 2) -> None:
        plot_evolution(self, quantities, ncols)
