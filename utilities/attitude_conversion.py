__all__ = ["quaternion_to_rotation_matrix", "quaternion_to_euler"]

import math
import numpy as np


def quaternion_to_rotation_matrix(q):
    """
    Convert quaternion to rotation matrix.
    :param q: Quaternion in the form [q1, q2, q3, q4]
    :return: Rotation matrix
    """
    q1, q2, q3, q4 = q
    R = np.array([
        [q1 ** 2 - q2 ** 2 - q3 ** 2 + q4 ** 2, 2 * (q1 * q2 + q3 * q4), 2 * (q1 * q3 - q2 * q4)],
        [2 * (q1 * q2 - q3 * q4), -q1 ** 2 + q2 ** 2 - q3 ** 2 + q4 ** 2, 2 * (q2 * q3 + q1 * q4)],
        [2 * (q1 * q3 + q2 * q4), 2 * (q2 * q3 - q1 * q4), -q1 ** 2 - q2 ** 2 + q3 ** 2 + q4 ** 2]
    ])
    return R


def quaternion_to_euler(q):
    """
    Convert quaternion to Euler angles (roll, pitch, yaw).
    :param q: Quaternion in the form [q1, q2, q3, q4]
    :return: Euler angles in radians as a tuple (roll, pitch, yaw)
    """

    (x, y, z, w) = q

    # Yaw
    t0 = +2.0 * (w * x + y * z)
    t1 = +1.0 - 2.0 * (x * x + y * y)
    yaw = math.atan2(t0, t1)

    # Pitch
    t2 = +2.0 * (w * y - z * x)
    t2 = +1.0 if t2 > +1.0 else t2
    t2 = -1.0 if t2 < -1.0 else t2
    pitch = math.asin(t2)

    # Roll
    t3 = +2.0 * (w * z + x * y)
    t4 = +1.0 - 2.0 * (y * y + z * z)
    roll = math.atan2(t3, t4)

    return np.array([yaw, pitch, roll]).reshape(3, 1)
