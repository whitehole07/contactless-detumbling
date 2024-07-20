# Entities
import cppyy
import numpy as np
from matplotlib import pyplot as plt

from processing.system_animation import animate_system
from processing.utilities.rotations import construct_rotation_matrix

# Environment
from environment import env, arm, attitude, t_step, y0_arm, save, debris, joints

# Get initial state
t, s = env.current_state()
save(t, s)

# Solve inverse kinematics
TD = np.eye(4)
TD[:3, -1] = [-5, 0, 0]
TD[:3, :3] = construct_rotation_matrix(-(TD[:3, -1] + arm.base_offset))

_, yD_arm = arm.inverse_kinematics(TD, np.array(y0_arm[:6]), 1e-5, 100, 20)
T_c = arm.get_transformation(yD_arm, 6)


print("Final angles [deg]: ", np.rad2deg(yD_arm))
print("Final angles [rad]: ", yD_arm)
print("Final EE position: ", T_c[:3, -1] + arm.base_offset)

# Set control parameters
env.set_control_torque(yD=cppyy.gbl.std.vector[float](list(yD_arm)))

# Perform steps
orb = [[], [], []]
while t < 10000:
    # Perform step
    retval, t, s = env.step(t_step=t_step)
    save(t, s)
    orb[0].append(s[19])
    orb[1].append(s[20])
    orb[2].append(s[21])

attitude.plot(["angular_velocity", "torques", "energy", "euler_angles"])

arm.plot()

# Angle between ee and angular velocity
angles = []
for ang_vel, ee_pose in zip(attitude.w.T, arm.end_effector.poses.T):
    # Normalize
    ang_vel = ang_vel / np.linalg.norm(ang_vel)
    ee_pos = ee_pose / np.linalg.norm(ee_pose)

    # Dot prod
    dot_prod = np.clip(np.dot(ee_pos, ang_vel), -1.0, 1.0)

    # Angle
    angles.append(np.rad2deg(np.arccos(dot_prod)))

fig, axs = plt.subplots(1, 1, figsize=(8, 6))

# Plot the critic loss
axs.axhline(y=0, color='r', linestyle='--', label='No-torque line')
axs.axhline(y=180, color='b', linestyle='--', label='No-torque line')
axs.axhline(y=90, color='green', linestyle='-.', label='Max-torque line')
axs.plot(attitude.t, angles, label='Angle')
axs.set_xlabel('Time [s]')
axs.set_ylabel('Angle [deg]')
axs.set_title('EE-AV angle')
axs.legend()
plt.grid(True)
plt.tight_layout()
plt.show()

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Plot the 3D line
ax.plot(*orb, label='3D Line')
ax.set_xlabel('X axis')
ax.set_ylabel('Y axis')
ax.set_zlabel('Z axis')
ax.set_title('3D Line Plot')

# Add a legend
ax.legend()

# Show the plot
plt.show()

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
    att=attitude,
    arms=[arm]
)"""