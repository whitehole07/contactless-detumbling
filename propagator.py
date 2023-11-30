__all__ = ["Propagator"]

import numpy as np
from scipy.integrate import solve_ivp

from attitude.attitude_propagator import AttitudePropagator
from orbital.orbit_propagator import OrbitPropagator


class Propagator(object):
    def __init__(self, *, attitude: AttitudePropagator, orbit: OrbitPropagator):
        """ For simultaneous propagation of attitude and oribt """
        # Save attitude propagator
        self.attitude: AttitudePropagator = attitude

        # Save orbital propagator
        self.orbit: OrbitPropagator = orbit

        # Evolving parameters
        self.__prop_sol = None  # Propagation solution

    def __overall_propagate_function(self, t, y) -> list:
        """ Returns a list of overall dys """
        return [*self.attitude.propagate_function(t, y[:7]), *self.orbit.propagate_function(t, y[7:])]

    def propagate(self, *, t_span: np.ndarray, eval_points: int = 1000) -> None:
        """ Propagator """
        # Integrate
        self.__prop_sol = solve_ivp(self.__overall_propagate_function, t_span, [*self.attitude.y0, *self.orbit.y0],
                                    t_eval=np.linspace(*t_span, eval_points), method="Radau")

        # Save solutions
        self.attitude._timestamps = self.__prop_sol.t
        self.attitude._prop_sol = self.__prop_sol.y[:7, :]  # Attitude solution
        self.orbit._timestamps = self.__prop_sol.t
        self.orbit._prop_sol = self.__prop_sol.y[7:, :]   # Orbital solution
