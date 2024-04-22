__all__ = ["Propagator"]

import numpy as np
from scipy.integrate import solve_ivp

from attitude.attitude_propagator import AttitudePropagator
from orbital.orbit_propagator import OrbitPropagator
from robotics.arm_propagator import ArmPropagator


class Propagator(object):
    def __init__(self, *, attitude: AttitudePropagator, orbit: OrbitPropagator, robotics: list = ()):
        """ For simultaneous propagation of attitude and orbit """
        # Save attitude propagator
        self.attitude: AttitudePropagator = attitude

        # Save orbital propagator
        self.orbit: OrbitPropagator = orbit

        # Save robotic arms
        self.arms: list[ArmPropagator] = robotics

        # Evolving parameters
        self.__prop_sol = None  # Propagation solution

    def __overall_propagate_function(self, t, y) -> list:
        """ Returns a list of overall dys """
        return [
            *self.attitude.propagate_function(t, y),
            *self.orbit.propagate_function(t, y),
            *[yi for y in [arm.propagate_function(t, y) for arm in self.arms] for yi in y]
        ]

    def propagate(self, *, t_span: np.ndarray, eval_points: int = 1000) -> None:
        """ Propagator """
        # Integrate
        self.__prop_sol = solve_ivp(self.__overall_propagate_function, t_span,
                                    y0=[*self.attitude.y0, *self.orbit.y0, *[y0i for y0 in [arm.y0 for arm in self.arms] for y0i in y0]],
                                    t_eval=np.linspace(*t_span, eval_points), method="Radau")

        # Save solutions
        self.attitude._timestamps = self.__prop_sol.t
        self.attitude._prop_sol = self.__prop_sol.y    # Attitude solution [:7, :]
        self.orbit._timestamps = self.__prop_sol.t
        self.orbit._prop_sol = self.__prop_sol.y       # Orbital solution [7:, :]

        for arm in self.arms:                          # Arm solutions [13+(i*3):16+(i*3)]
            arm.save(self.__prop_sol.t, self.__prop_sol.y[arm.indexes])

    def load_from_csv(self, filename: str) -> None:
        """
        Load a CSV file into a NumPy array.

        Args:
            filename (str): The path to the CSV file.

        Returns:
            numpy.ndarray: The NumPy array containing the data from the CSV file.
        """
        try:
            # Load CSV file into a NumPy array as strings
            csv = np.genfromtxt(filename, delimiter=',', skip_header=1)[:, :-1]
            t = csv[:, 0]; y = csv[:, 1:]

            # Save solutions
            self.attitude._timestamps = t
            self.attitude._prop_sol = y  # Attitude solution [:7, :]
            self.orbit._timestamps = t
            self.orbit._prop_sol = y  # Orbital solution [7:, :]

            for arm in self.arms:  # Arm solutions [13+(i*3):16+(i*3)]
                arm.save(t, y[arm.indexes])
        except Exception as e:
            print(f"Error loading CSV file: {e}")
            return None
