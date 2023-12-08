import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation

from attitude.attitude_conversion import quaternion_to_rotation_matrix
from attitude.torques.eddy_current import Electromagnet
from utilities.ui import LoadingBarWithTime


def rotate_cylinder(longitudinal_axis_m):
    # Normalize the input axis
    m = np.array(longitudinal_axis_m)
    m /= np.linalg.norm(m)

    # Define the initial axis of the cylinder
    initial_axis = np.array([0, 0, 1])

    # Calculate the rotation axis and angle using the cross product
    v = np.cross(initial_axis, m)
    s = np.linalg.norm(v)

    if s < 1e-12:
        # If the norm is very close to zero, handle it to avoid division by zero
        rotation_matrix = np.eye(3)
    else:
        # Continue with the Rodriguez formula
        c = np.dot(initial_axis, m)
        cross_product_matrix = np.array([[0, -v[2], v[1]],
                                        [v[2], 0, -v[0]],
                                        [-v[1], v[0], 0]])
        rotation_matrix = np.eye(3) + cross_product_matrix + np.dot(cross_product_matrix, cross_product_matrix) * ((1 - c) / (s ** 2))

    return rotation_matrix


def animate_attitude(t, q, eu, h, r, dpi, magnet: Electromagnet = None):
    # Initialize the figure and 3D axis
    fig = plt.figure(figsize=(8, 6))
    ax = fig.add_subplot(111, projection='3d')

    # Print
    print("- Animation rendering progress -")

    # Create the cylinder vertices
    theta = np.linspace(0, 2 * np.pi, 100)
    z = np.linspace(-h/2, h/2, 2)
    Z, Theta = np.meshgrid(z, theta)
    X = r * np.cos(Theta)
    Y = r * np.sin(Theta)

    # Magnet
    if magnet is not None:
        X_m_s, Y_m_s, Z_m_s = [], [], []
        for loc in magnet.locations:
            # Compute magnet meshgrid
            z_m = np.linspace(loc[2] - magnet.height/2, loc[2] + magnet.height/2, 2)
            X_m = loc[0] + magnet.radius*np.cos(theta)
            Y_m = loc[1] + magnet.radius*np.sin(theta)
            X_m, Z_m = np.meshgrid(X_m, z_m)
            Y_m, Z_m = np.meshgrid(Y_m, z_m)

            # Save
            X_m_s.append(X_m)
            Y_m_s.append(Y_m)
            Z_m_s.append(Z_m)

    # Function to update the plot for each time step
    def update(frame):
        # Update loading bar
        LoadingBarWithTime("- Animation rendering progress -").update(frame+1, len(t))

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
        ax.plot_surface(rotated_points[0, :, :], rotated_points[1, :, :], rotated_points[2, :, :],
                        color='b', alpha=0.8, label="Debris")

        # Magnet
        if magnet is not None:
            for i, (location, moment) in enumerate(zip(magnet.locations, magnet.moments)):
                # Rotate the cylinder based on the given axis_vector
                R_m = rotate_cylinder(moment)

                # Rotate Electromagnet
                rotated_magnet = np.dot(R_m, np.array([X_m_s[i], Y_m_s[i], Z_m_s[i]]).reshape(3, -1) - location.reshape(-1, 1)) + location.reshape(-1, 1)
                rotated_magnet = rotated_magnet.reshape(3, 2, 100)

                # Plot magnetic moment
                ax.quiver(*location, *(3*moment), color="orange")

                # Cylinder
                ax.plot_surface(rotated_magnet[0, :, :], rotated_magnet[1, :, :], rotated_magnet[2, :, :], color='orange',
                                alpha=0.8, label="Electromagnet")

                # Mark Center
                ax.scatter(*location, color="orange")

        # Options
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_zlabel('Z')
        ax.set_title(f'Time: {t[frame]:.1f}s')

        # Add euler angles
        ax.text2D(0.95, 0.9,
                        f"Roll:   {euler[0]:.2f}°\n"
                        f"Pitch: {euler[1]:.2f}°\n"
                        f"Yaw:   {euler[2]:.2f}°", transform=ax.transAxes)

        # Set fixed ticks for the X, Y, and Z axes
        max_magn = (max([x.max() for x in magnet.locations]) + magnet.radius) if magnet is not None else 0
        max_dim_lim = int((max([r, h, max_magn]) + 20)/2)

        # Set ticks
        step = 2.5
        ax.set_xticks(np.arange(-max_dim_lim, max_dim_lim+1, step))
        ax.set_yticks(np.arange(-max_dim_lim, max_dim_lim+1, step))
        ax.set_zticks(np.arange(-max_dim_lim, max_dim_lim+1, step))

        # Arrow for the positive x direction
        arrow_length: float = 5
        ax.quiver(*(0, 0, 0), arrow_length, 0, 0, color='b', label='X-axis')

        # Arrow for the positive y direction
        ax.quiver(*(0, 0, 0), 0, arrow_length, 0, color='g', label='Y-axis')

        # Arrow for the positive z direction
        ax.quiver(*(0, 0, 0), 0, 0, arrow_length, color='r', label='Z-axis')

        # Remove tick labels
        ax.set_xticklabels([])
        ax.set_yticklabels([])
        ax.set_zticklabels([])

        # Equal axis
        ax.axis("equal")

        # Legend
        # ax.legend()

        # Adjust the box aspect to control the size
        ax.set_box_aspect([1, 1, 0.85])  # Adjust the values as needed

        # Set view angle to ensure arrows are visible
        ax.view_init(elev=35+10*np.sin(frame/500), azim=45+frame/50)

    # Create the animation
    anim = FuncAnimation(fig, update, frames=len(t), repeat=False)

    # Close loading bar
    LoadingBarWithTime().end()

    # Save the animation with lower quality
    anim.save('rotation_animation.mp4', writer='ffmpeg', fps=30, dpi=dpi)
