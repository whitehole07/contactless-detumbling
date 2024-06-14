import numpy as np


def state_extractor(to_be_extracted: list):
    return np.array(to_be_extracted[19:22] + to_be_extracted[31:37] + to_be_extracted[12:15])


def evaluate_step(prev_state, global_step, ik_success, attitude, detumbling_threshold, safe_sphere):
    # Convert to numpy
    state = state_extractor(global_step)

    # Init "done" and "reward"
    done = 0  # "done" normally negative
    reward = 0

    # Distance reward function
    """distance = np.linalg.norm(state[:3]) - safe_sphere - 5
    reward += 2*(np.exp(-0.99*abs(distance)) - 1)

    # Sparse reward function
    if not ik_success:
        reward -= 2

    # Directional reward function
    pre_distance = np.linalg.norm(prev_state[:3]) - safe_sphere - 5
    if distance > pre_distance:
        reward -= 1
    elif distance == pre_distance:
        reward += 0
    elif distance < pre_distance:
        reward += 1

    # Regional reward function
    if 4 < distance < 6:
        reward += 2"""

    # Check: collision with safe sphere
    # for i in range(len(arm.joints) + 1):
    #    pos: np.ndarray = arm.get_joint_position(global_step[:6], i)
    #    if np.linalg.norm(pos) < safe_sphere:
    #        reward -= 10

    # Energy reward
    w: np.ndarray = state[9:13]
    II = attitude._entity.inertia_matrix
    E = np.dot(np.dot(w, II), w.T)
    # reward += 2*(np.exp(-0.99*E) - 1)

    # Directional
    prev_w = prev_state[9:13]
    II = attitude._entity.inertia_matrix
    prev_E = np.dot(np.dot(prev_w, II), prev_w.T)
    dE = (E - prev_E) / 50
    reward += -dE  * 100

    # Check if detumbling is over
    if E <= detumbling_threshold:
        done = 1
        reward += 10
        print("Detumbling!!", reward)
        return state, reward, done # Detumbling completed, simulation over
    return state, reward, done
