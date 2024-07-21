__all__ = ["Cylinder", "Planet"]

import numpy as np

from processing.utilities.inertia_matrix import cylinder_inertia


class Cylinder(object):
    def __init__(self, mass: float, radius: float, height: float, thickness: float, sigma: float,
                 Ix, Iy, Iz, Mx, My, Mz) -> None:
        # Physical properties
        self.mass: float = mass
        self.radius: float = radius
        self.height: float = height
        self.thickness: float = thickness
        self.sigma: float = sigma

        self.inertia_matrix = np.diag([Ix, Iy, Iz])
        self.magnetic_tensor = np.diag([Mx, My, Mz])
        # Generate inertia matrix
        # self.inertia_matrix: np.ndarray = cylinder_inertia(mass=mass, radius=radius, height=height)

    @property
    def Ixx(self) -> float:
        return float(self.inertia_matrix[0, 0])

    @property
    def Iyy(self) -> float:
        return float(self.inertia_matrix[1, 1])

    @property
    def Izz(self) -> float:
        return float(self.inertia_matrix[2, 2])

    @property
    def Mxx(self) -> float:
        return float(self.magnetic_tensor[0, 0])

    @property
    def Myy(self) -> float:
        return float(self.magnetic_tensor[1, 1])

    @property
    def Mzz(self) -> float:
        return float(self.magnetic_tensor[2, 2])


class Planet(object):
    def __init__(self, mu: float, radius: float):
        # Physical Properties
        self.mu: float = mu
        self.radius: float = radius
