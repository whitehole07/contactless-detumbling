import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation

from attitude.attitude_conversion import quaternion_to_rotation_matrix


def animate_attitude(t, q, eu, h, r):
    # Initialize the figure and 3D axis
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # Create the cylinder vertices
    theta = np.linspace(0, 2 * np.pi, 100)
    z = np.linspace(-h/2, h/2, 2)
    Z, Theta = np.meshgrid(z, theta)
    X = r * np.cos(Theta)
    Y = r * np.sin(Theta)

    # Function to update the plot for each time step
    def update(frame):
        # Recover quaternions
        ax.cla()
        quaternion = q[:, frame]
        euler = eu[:, frame]
        R = quaternion_to_rotation_matrix(quaternion)

        # Rotate cylinder
        rotated_points = np.dot(R, np.array([X, Y, Z]).reshape(3, -1))
        rotated_points = rotated_points.reshape(3, 100, 2)

        # Plot surfaces
        # Cylinder
        ax.plot_surface(rotated_points[0, :, :], rotated_points[1, :, :], rotated_points[2, :, :], color='b', alpha=0.8)

        # Options
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_zlabel('Z')
        ax.set_title(f'Time: {t[frame]:.1f}s')
        ax.axis("equal")

        # Add euler angles
        ax.text2D(0.95, 0.9,
                        f"Roll:   {euler[0]:.2f}°\n"
                        f"Pitch: {euler[1]:.2f}°\n"
                        f"Yaw:   {euler[2]:.2f}°", transform=ax.transAxes)

        # Set fixed ticks for the X, Y, and Z axes
        max_dim_lim = int((max([r, h]) + 10)/2)
        step = 2
        ax.set_xticks(np.arange(-max_dim_lim, max_dim_lim+1, step))
        ax.set_yticks(np.arange(-max_dim_lim, max_dim_lim+1, step))
        ax.set_zticks(np.arange(-max_dim_lim, max_dim_lim+1, step))

        # Arrow for the positive x direction
        arrow_length: float = 5
        ax.quiver(*(0, 0, 0), arrow_length, 0, 0, color='r', label='X-axis')

        # Arrow for the positive y direction
        ax.quiver(*(0, 0, 0), 0, arrow_length, 0, color='g', label='Y-axis')

        # Arrow for the positive z direction
        ax.quiver(*(0, 0, 0), 0, 0, arrow_length, color='b', label='Z-axis')

        # Remove tick labels
        ax.set_xticklabels([])
        ax.set_yticklabels([])
        ax.set_zticklabels([])

        # Set view angle to ensure arrows are visible
        ax.view_init(elev=35+10*np.sin(frame/500), azim=45+frame/50)

    # Create the animation
    anim = FuncAnimation(fig, update, frames=len(t), repeat=False)

    # Lower the DPI and resolution for faster rendering
    dpi = 300  # Adjust the DPI as needed

    # Save the animation with lower quality
    anim.save('rotation_animation.mp4', writer='ffmpeg', fps=30, dpi=dpi)
