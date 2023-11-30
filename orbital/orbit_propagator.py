import numpy as np
from matplotlib import pyplot as plt
from scipy.integrate import solve_ivp

from orbital.orbit_conversion import keplerian_to_cartesian
from utilities.entities import Planet


class OrbitPropagator(object):
    def __init__(self, planet: Planet, a0: float, e0: float, i0: float, OM0: float, om0: float, f0: float):
        # Gravitational constant, standard is Earth's
        self.planet = planet

        # Initial state
        self.kep0 = [a0, e0, np.deg2rad(i0), np.deg2rad(OM0), np.deg2rad(om0), np.deg2rad(f0)]
        self.r0, self.v0 = keplerian_to_cartesian(self.kep0, self.planet.mu)  # Convert initial keplerian to cartesian
        self.y0 = np.concatenate((self.r0, self.v0))                          # Overall initial conditions

        # Evolving parameters
        self._timestamps = None  # Propagation timestamps
        self._prop_sol = None  # Propagation solution

    @property
    def t(self) -> np.ndarray:
        return self._timestamps

    @property
    def T(self) -> float:
        return 2*np.pi * np.sqrt((self.kep0[0] ** 3)/self.planet.mu)

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
        dv = -(self.planet.mu/(r ** 3)) * rv  # + sum of perturbation accelerations
        return [*dr, *dv]

    def plot_orbit(self):
        """ Plot 3D orbit """
        # Create a 3D plot
        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')

        # Plot the central sphere representing Earth
        u = np.linspace(0, 2*np.pi, 100)
        v = np.linspace(0, np.pi, 100)
        x_earth = self.planet.radius * np.outer(np.cos(u), np.sin(v))
        y_earth = self.planet.radius * np.outer(np.sin(u), np.sin(v))
        z_earth = self.planet.radius * np.outer(np.ones(np.size(u)), np.cos(v))
        ax.plot_surface(x_earth, y_earth, z_earth, color='b', alpha=0.2, label='Earth')

        # Plot the orbit
        ax.plot(self.r[0, :], self.r[1, :], self.r[2, :], label='Track')

        if self.t[-1] < self.T:
            # Propagate the rest of the orbit
            t_span = [self.t[-1], self.T]
            rest = solve_ivp(
                self.propagate_function,
                t_span,
                [*self.r[:, -1], *self.v[:, -1]],
                t_eval=np.linspace(*t_span, 1000),
                method="Radau")
            ax.plot(rest.y[0, :], rest.y[1, :], rest.y[2, :], 'g-.', label='Rest')

        # Mark the last point in the time series
        ax.scatter(self.r[0, -1], self.r[1, -1], self.r[2, -1], color='r', s=50, label='Last Position')

        # Customize the plot
        ax.set_xlabel('X-axis')
        ax.set_ylabel('Y-axis')
        ax.set_zlabel('Z-axis')
        ax.set_title('Orbit')
        ax.legend()

        # Show the plot
        ax.axis("equal")
        plt.tight_layout()
        plt.show()
