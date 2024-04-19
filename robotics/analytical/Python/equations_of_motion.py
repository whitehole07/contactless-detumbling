import sympy as sp
import numpy as np


# Transformation matrix
def T(dhs: np.ndarray, index: int) -> sp.Matrix:
    return sp.Matrix([ # theta, a, d, alpha
        [sp.cos(dhs[index, 0]), -sp.sin(dhs[index, 0]) * sp.cos(dhs[index, 3]), sp.sin(dhs[index, 0]) * sp.sin(dhs[index, 3]), dhs[index, 1] * sp.cos(dhs[index, 0])],
        [sp.sin(dhs[index, 0]), sp.cos(dhs[index, 0]) * sp.cos(dhs[index, 3]), -sp.cos(dhs[index, 0]) * sp.sin(dhs[index, 3]), dhs[index, 1] * sp.sin(dhs[index, 0])],
        [0, sp.sin(dhs[index, 3]), sp.cos(dhs[index, 3]), dhs[index, 2]],
        [0, 0, 0, 1]
    ])


# Define parameters
t = sp.Symbol('t')
t1, t2, t3, t4, t5, t6 = sp.symbols('t1 t2 t3 t4 t5 t6', cls=sp.Function)

# Joint variables
q = [t1(t), t2(t), t3(t), t4(t), t5(t), t6(t)]

# Denavit-Hartenberg Parameters
dh = np.array([
    [t1(t), 0, 0.1807, sp.pi / 2],
    [t2(t), -0.6127, 0, 0],
    [t3(t), -0.57155, 0, 0],
    [t4(t), 0, 0.17415, sp.pi / 2],
    [t5(t), 0, 0.11985, -sp.pi / 2],
    [t6(t), 0, 0.11655, 0],
])

# Masses
m = np.array([7.369, 13.051, 3.989, 2.1, 1.98, 0.615])
com = np.array([
    [0.021, 0.000, 0.027],
    [0.38, 0.000, 0.158],
    [0.24, 0.000, 0.068],
    [0.000, 0.007, 0.018],
    [0.000, 0.007, 0.018],
    [0, 0, -0.026],
])

# Moment of inertia
Im = np.zeros((3, 3, 6))
Im[:, :, 0] = np.array([
    [0.0341, 0.0000, -0.0043],
    [0.0000, 0.0353, 0.0001],
    [-0.0043, 0.0001, 0.0216],
])

Im[:, :, 1] = np.array([
    [0.0281, 0.0001, -0.0156],
    [0.0001, 0.7707, 0.0000],
    [-0.0156, 0.0000, 0.7694],
])

# Moment of inertia
Im[:, :, 2] = np.array([
    [0.0101, 0.0001, 0.0092],
    [0.0001, 0.3093, 0.0000],
    [0.0092, 0.0000, 0.3065],
])

Im[:, :, 3] = np.array([
    [0.0030, -0.0000, -0.0000],
    [-0.0000, 0.0022, -0.0002],
    [-0.0000, -0.0002, 0.0026],
])

Im[:, :, 4] = np.array([
    [0.0030, -0.0000, -0.0000],
    [-0.0000, 0.0022, -0.0002],
    [-0.0000, -0.0002, 0.0026],
])

Im[:, :, 5] = np.array([
    [0.0000, 0.0000, -0.0000],
    [0.0000, 0.0004, 0.0000],
    [-0.0000, 0.0000, 0.0003],
])

# Degrees of freedom
n = 4 # dh.shape[0]

# COMPUTATION OF SYMBOLICAL INERTIA MATRIX
# Init inertia matrix
D = np.zeros((n, n))

# Loop
for i in range(n):
    # Jacobians link i
    JPi = sp.zeros(3, n)
    JOi = sp.zeros(3, n)

    # Position of CoM
    T0i = sp.eye(4)
    T0ip = sp.eye(4)
    for j in range(i + 1):
        T0ip = T0i
        T0i = T0i * T(dh, j)
    pli = T0ip[:3, 3] + T0i[:3, :3] * sp.Matrix(com[i, :])

    # Init transformation matrix
    T0jp = sp.eye(4)
    for j in range(i + 1):
        # Update Jacobians
        JPi[:, j] = T0jp[:3, 2].cross(pli - T0jp[:3, 3])
        JOi[:, j] = T0jp[:3, 2]

        # Update transformation matrix
        T0jp = T0jp * T(dh, j)

    # Sum contribution
    D += m[i] * JPi.T * JPi + JOi.T * T0i[:3, :3] * Im[:, :, i] * T0i[:3, :3].T * JOi

print(D.evalf(subs={t1(t): 1, t2(t): 1, t3(t): 1, t4(t): 1}))

# COMPUTATION OF C MATRIX
# Christoffel symbols
# Init C tensor
C = sp.zeros(n, n)

for j in range(n):
    for k in range(n):
        # Init value
        ckj = 0

        # Summation of christoffels
        for i in range(n):
            ckj = ckj + 0.5 * (sp.diff(D[k, j], q[i]) +
                               sp.diff(D[k, i], q[j]) -
                               sp.diff(D[i, j], q[k])) * sp.diff(q[i],t)

        # Assign value
        C[k, j] = ckj

# Save EoM matrices
np.save("D.npy", D)
np.save("C.npy", C)
