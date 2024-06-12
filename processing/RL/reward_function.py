import numpy as np


def state_extractor(to_be_extracted: list):
    return np.array(to_be_extracted[19:22] + to_be_extracted[31:37] + to_be_extracted[12:15])


def evaluate_step(global_step, ik_success, arm, attitude, detumbling_threshold, safe_sphere):
    # Convert to numpy
    state = state_extractor(global_step)

    # Init "done" and "reward"
    done = 0  # "done" normally negative
    reward = 0

    # Check inverse kinematics
    #if not ik_success:
    #    reward -= 100

    # Check: collision with safe sphere
    for i in range(len(arm.joints) + 1):
        pos: np.ndarray = arm.get_joint_position(global_step[:6], i)
        if np.linalg.norm(pos) < safe_sphere:
            reward -= 10

    # Distance reward
    distance = np.linalg.norm(state[:3])
    if distance > 0:
        reward -= 0.5*distance**2

    # Energy reward
    w: np.ndarray = state[9:13]
    II = attitude._entity.inertia_matrix
    E = np.dot(np.dot(w, II), w.T)
    # reward -= E

    # Check if detumbling is over
    if E <= detumbling_threshold:
        done = 1
        print("Detumbling", reward)
        return state, reward, done # Detumbling completed, simulation over
    return state, reward, done
