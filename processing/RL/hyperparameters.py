import numpy as np

from environment import debris

# Environment-related
max_action = 0.5                                                  # Action maximum range [rad]
safe_sphere: float = max(debris.radius, debris.height/2) + 0.5    # Safe sphere for collision check  [m]
detumbling_threshold: float = 1                                   # Detumbling threshold in energy terms [J]


# Environment action and states
# States:
# pos_EE (3), pose_EE (3), om_DEB (3)
#
# Actions:
# d_EE (3)
#

def state_extractor(to_be_extracted: list):
    return np.array(to_be_extracted[19:22] + to_be_extracted[22:25] + to_be_extracted[12:15])


state_dim = 9
action_dim = 3

# Model-related
actor_layers = 400
critic_layers = 400

# Learning rates
actor_lr = 0.0001
critic_lr = 0.0002

# Stop random sampling after x timesteps
batch_size = 256
sample_max_timestep = batch_size

# Soft update rate
tau = 0.005

# Discount factor
gamma = 0.99
