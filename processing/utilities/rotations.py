__all__ = ["construct_rotation_matrix"]

import numpy as np


def construct_rotation_matrix(z):
    # Ensure the z vector is normalized
    z = z / np.linalg.norm(z)

    # Choose an arbitrary vector that is not parallel to z
    if np.abs(z[0]) < 0.9:
        arbitrary = np.array([1, 0, 0])
    else:
        arbitrary = np.array([0, 1, 0])

    # Ensure the arbitrary vector is orthogonal to z
    x = np.cross(arbitrary, z)
    x = x / np.linalg.norm(x)

    # Compute the y vector using the cross product
    y = np.cross(z, x)

    # Construct the rotation matrix
    R = np.column_stack((x, y, z))

    return R