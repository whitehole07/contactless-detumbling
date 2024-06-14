import numpy as np


def spherical_to_cartesian(r, theta, phi):
    """
    Convert spherical coordinates to Cartesian coordinates.

    Parameters:
    - r (float or np.ndarray): radial distance
    - theta (float or np.ndarray): polar angle in radians
    - phi (float or np.ndarray): azimuthal angle in radians

    Returns:
    - tuple: (x, y, z) Cartesian coordinates
    """
    x = r * np.sin(theta) * np.cos(phi)
    y = r * np.sin(theta) * np.sin(phi)
    z = r * np.cos(theta)

    return np.array([x, y, z])


def cartesian_to_spherical(x, y, z):
    """
    Convert Cartesian coordinates to spherical coordinates.

    Parameters:
    - x (float or np.ndarray): x-coordinate
    - y (float or np.ndarray): y-coordinate
    - z (float or np.ndarray): z-coordinate

    Returns:
    - tuple: (r, theta, phi) spherical coordinates
    """
    r = np.sqrt(x ** 2 + y ** 2 + z ** 2)
    theta = np.arccos(z / r)
    phi = np.arctan2(y, x)

    return np.array([r, theta, phi])