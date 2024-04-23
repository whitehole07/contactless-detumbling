import numpy as np

from post_processing.attitude.attitude_conversion import quaternion_to_euler
from post_processing.attitude.attitude_animation import animate_attitude
from post_processing.attitude.attitude_plot import plot_evolution
from post_processing.attitude.torques.base import TorqueObject
from post_processing.robotics.arm_propagator import ArmPropagator


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

    def plot(self, quantities: list, ncols: int = 2) -> None:
        plot_evolution(self, quantities, ncols)

    def animate(self, *, dpi: int = 300, arms: list[ArmPropagator] = ()) -> None:
        # Plot Cylinder attitude #TODO: Remove from here
        animate_attitude(
            self.t,
            self.q,
            self.euler_angles,
            self._entity.height,
            self._entity.radius,
            dpi,
            arms=arms
        )
