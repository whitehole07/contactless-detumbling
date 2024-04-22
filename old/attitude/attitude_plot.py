import numpy as np
from matplotlib import pyplot as plt


def plot_evolution(attitude_propagator, quantities: list, ncols: int = 2) -> None:
    # Retrieve results
    t = attitude_propagator.t

    # Create subplots
    n = len(quantities)
    nrows = int(n / ncols + (ncols - n) % ncols)
    fig, ax = plt.subplots(nrows, ncols if n > 1 else 1, figsize=(10, 7))

    # Check ax
    ax = np.array(ax).reshape((-1,))

    # Go through the quantities
    index = 0
    for quantity in quantities:
        if "angular_velocity" == quantity:
            # Retrieve angular velocities
            w = attitude_propagator.w

            # Set plot
            ax[index].set_xlabel('Time [s]')
            ax[index].set_ylabel('Angular Velocity [rad/s]')

            # Plot the vectors with customized appearance
            ax[index].plot(t, w[0, :], label='$\omega_x$', color='blue', linestyle='-')
            ax[index].plot(t, w[1, :], label='$\omega_y$', color='green', linestyle='--')
            ax[index].plot(t, w[2, :], label='$\omega_z$', color='red', linestyle='-.')

            # Add a legend
            ax[index].legend()

            # Add a title and grid
            ax[index].set_title('Angular Velocities')
            ax[index].grid(True)

            # Increase counter
            index += 1
        if "euler_angles" == quantity:
            # Retrieve angles
            euler = attitude_propagator.euler_angles

            # Angles plot
            ax[index].set_xlabel('Time [s]')
            ax[index].set_ylabel('Angle [deg]')

            # Plot the vectors with customized appearance
            ax[index].plot(t, euler[0, :], label=r'$\theta_{roll}$', color='blue', linestyle='-')
            ax[index].plot(t, euler[1, :], label=r'$\theta_{pitch}$', color='green', linestyle='--')
            ax[index].plot(t, euler[2, :], label=r'$\theta_{yaw}$', color='red', linestyle='-.')

            # Add a legend
            ax[index].legend()

            # Add a title and grid
            ax[index].set_title('Euler Angles')
            ax[index].grid(True)

            # Increase counter
            index += 1
        if "quaternions" == quantity:
            # Retrieve quaternions
            q = attitude_propagator.q

            # Quaternion plot
            ax[index].set_xlabel('Time [s]')
            ax[index].set_ylabel('Quaternion Magnitude')

            # Plot the vectors with customized appearance
            ax[index].plot(t, q[0, :], label='$q_1$', color='blue', linestyle='-')
            ax[index].plot(t, q[1, :], label='$q_2$', color='green', linestyle='--')
            ax[index].plot(t, q[2, :], label='$q_3$', color='red', linestyle='-.')
            ax[index].plot(t, q[3, :], label='$q_4$', color='purple', linestyle=':')

            # Add a legend
            ax[index].legend()

            # Add a title and grid
            ax[index].set_title('Quaternions')
            ax[index].grid(True)

            # Increase counter
            index += 1
        if "energy" == quantity:
            # Retrieve quantities
            ww = attitude_propagator.w
            II = attitude_propagator._entity.inertia_matrix

            # Compute energy
            E = []
            for w in ww.T:
                E.append(np.dot(np.dot(w, II), w.T))

            # Plot
            ax[index].set_xlabel('Time [s]')
            ax[index].set_ylabel('Energy [J]')

            # Plot the vectors with customized appearance
            ax[index].plot(t, E, label=r'$E_{rot}$', color='blue', linestyle='-')

            # Add a title and grid
            ax[index].set_title('Rotational Energy')
            ax[index].grid(True)

            # Increase counter
            index += 1
        if "torques" == quantity:
            # Plot
            ax[index].set_xlabel('Time [s]')
            ax[index].set_ylabel('Torque [Nm]')

            # Generate torque history
            attitude_propagator._ext_torque.generate_history(t, attitude_propagator._prop_sol)

            # Plot every torque
            for torque in attitude_propagator._ext_torque:
                # Plot the vectors with customized appearance
                ax[index].plot(t, torque.history[0, :], linestyle='-', label=r'$%s_{(x)}$' % torque.name)
                ax[index].plot(t, torque.history[1, :], linestyle='--', label=r'$%s_{(y)}$' % torque.name)
                ax[index].plot(t, torque.history[2, :], linestyle='-.', label=r'$%s_{(z)}$' % torque.name)

            # Add a title and grid
            ax[index].legend()
            ax[index].set_title('Torques')
            ax[index].grid(True)

            # Increase counter
            index += 1

    # Show the plot
    plt.tight_layout()  # Improve spacing
    plt.show()
