# Entities
import cppyy
import numpy as np

from processing.utilities.rotations import construct_rotation_matrix

# Environment
from environment import env, arm, attitude, t_step, y0_arm, save

# Get initial state
t, s = env.current_state()
save(t, s)

# Solve inverse kinematics
TD = np.eye(4)
TD[:3, -1] = [-4, 0, 0]
TD[:3, :3] = construct_rotation_matrix(-(TD[:3, -1] + arm.base_offset))

_, yD_arm = arm.inverse_kinematics(TD, np.array(y0_arm[:6]), 1e-5, 100, 20)
T_c = arm.get_transformation(yD_arm, 6)

print("Final angles [deg]: ", np.rad2deg(yD_arm))
print("Final angles [rad]: ", yD_arm)
print("Final EE position: ", T_c[:3, -1] + arm.base_offset)

# Set control parameters
env.set_control_torque(yD=cppyy.gbl.std.vector[float](list(yD_arm)))

# Perform steps
while t < 1000:
    # Perform step
    retval, t, s = env.step(t_step=t_step)
    save(t, s)

attitude.plot(["angular_velocity", "torques", "energy", "euler_angles"])

arm.plot()

print("Supposed:\n", TD)
print("\nAfter IK:\n", T_c)
print("\nReached:\n", arm.get_transformation(s[:6], 6))

"""animate_system(
    t=attitude.t,
    q=attitude.q,
    eu=attitude.euler_angles,
    h=debris.height,
    r=debris.radius,
    dpi=300,
    arms=[arm],
    dh_par=joints
)"""
