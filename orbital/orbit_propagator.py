import numpy as np

from orbital.orbit_conversion import keplerian_to_cartesian


class OrbitPropagator(object):
    def __init__(self, a0: float, e0: float, i0: float, OM0: float, om0: float, f0: float, mu: float = 3.98600433e+5):
        # Gravitational constant, standard is Earth's
        self.mu = mu

        # Initial state
        self.r0, self.v0 = keplerian_to_cartesian(
            [a0, e0, np.rad2deg(i0), np.rad2deg(OM0), np.rad2deg(om0), np.rad2deg(f0)], mu)  # Convert initial keplerian to cartesian
        self.y0 = np.concatenate((self.r0, self.v0))                                    # Overall initial conditions

        # Evolving parameters
        self._timestamps = None  # Propagation timestamps
        self._prop_sol = None  # Propagation solution

    @property
    def t(self) -> np.ndarray:
        return self._timestamps

    @property
    def r(self) -> np.ndarray:
        return self._prop_sol[:3, :]

    @property
    def v(self) -> np.ndarray:
        return self._prop_sol[3:, :]

    def propagate_function(self, t, y):
        # Evaluate perturbing acceleration
        # TODO: Consider adding orbital perturbations

        # Position vector and velocity into single variables
        rv = y[:3]
        vv = y[3:]

        # Distance from origin
        r = np.linalg.norm(rv)

        # Equations of motion
        dr = vv
        dv = -(self.mu/(r ** 3)) * rv  # + sum of perturbation accelerations
        return [*dr, *dv]


