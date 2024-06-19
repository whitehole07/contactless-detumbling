import numpy as np

from processing.RL.hyperparameters import state_extractor


def evaluate_step(prev_state, global_step, ik_success, attitude, detumbling_threshold, safe_sphere):
    # Convert to numpy
    state = state_extractor(global_step)

    # Init "done" and "reward"
    done = 0  # "done" normally negative
    reward = 0

    # Distance area reward function
    distance = np.linalg.norm(state[:3]) - safe_sphere
    if distance <= 2:
        reward += 0.5
    # reward += (np.exp(-0.99*abs(distance)) - 1)

    # Sparse reward function
    if not ik_success:
        reward -= 1.0

    # Compute energy
    w: np.ndarray = state[6:9]
    II = attitude._entity.inertia_matrix
    E = np.dot(np.dot(w, II), w.T)

    # Angle reward
    ang_vel = w / np.linalg.norm(w)
    ee_pos = state[3:6] / np.linalg.norm(state[3:6])
    dot_prod = np.clip(np.dot(ee_pos, ang_vel), -1.0, 1.0)
    angle = np.arccos(dot_prod)
    reward -= 2 * (abs(angle - np.pi/2) / (np.pi/2))

    # Directional
    prev_w = prev_state[6:9]
    prev_ang_vel = prev_w / np.linalg.norm(prev_w)
    prev_ee_pos = prev_state[3:6] / np.linalg.norm(prev_state[3:6])
    prev_dot_prod = np.clip(np.dot(prev_ee_pos, prev_ang_vel), -1.0, 1.0)
    prev_angle = abs(np.arccos(prev_dot_prod) - np.pi/2)
    if angle - prev_angle < 0:
        reward += 0.25
    elif angle - prev_angle > 0:
        reward -= 0.25

    # Check if detumbling is over
    if E <= detumbling_threshold:
        done = 1
        print("Detumbling!!")
        return state, reward, done # Detumbling completed, simulation over
    return state, reward, done
