__all__ = ["cylinder_inertia"]

import numpy as np


def cylinder_inertia(mass: float, radius: float, height: float) -> np.ndarray:
    Ix = (1 / 12) * mass * (height**2 + 3 * radius**2)
    Iy = (1 / 12) * mass * (height**2 + 3 * radius**2)
    Iz = (1 / 2) * mass * radius**2

    return np.diag([Ix, Iy, Iz])
