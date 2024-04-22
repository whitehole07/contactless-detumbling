__all__ = ["keplerian_to_cartesian"]

import numpy as np


def keplerian_to_cartesian(kep, mu):
    a, e, i, OM, om, f = np.array(kep).flatten()

    # 1 - Position and velocity in perifocal
    p = a * (1 - e**2)
    r = p / (1 + e*np.cos(f))

    rr_PF = np.array([r * np.cos(f), r * np.sin(f), 0])
    vv_PF = np.sqrt(mu / p) * np.array([-np.sin(f), e + np.cos(f), 0])

    # 2 - Rotation matrix
    R_OM = np.array([[np.cos(OM), np.sin(OM), 0], [-np.sin(OM), np.cos(OM), 0], [0, 0, 1]])
    R_i = np.array([[1, 0, 0], [0, np.cos(i), np.sin(i)], [0, -np.sin(i), np.cos(i)]])
    R_om = np.array([[np.cos(om), np.sin(om), 0], [-np.sin(om), np.cos(om), 0], [0, 0, 1]])

    R_PF = np.dot(np.dot(R_om, R_i), R_OM)

    R_GE = np.transpose(R_PF)

    # 3 - Position and velocity in geocentric equatorial
    rr = np.dot(R_GE, rr_PF)
    vv = np.dot(R_GE, vv_PF)

    return rr, vv
