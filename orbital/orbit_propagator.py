import numpy as np


def cartesian_eom(t, y, mu):
    # Evaluate perturbing acceleration
    # TODO: Consider adding orbital perturbations

    # Position vector and velocity into single variables
    rv = y[0:3]
    vv = y[3:6]

    # Distance from origin
    r = np.linalg.norm(rv)

    # Equations of motion
    dr = vv
    dv = -(mu / (r ** 3)) * rv  # + sum of perturbation accelerations
    return [dr, dv]
