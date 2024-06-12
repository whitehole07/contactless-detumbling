import numpy as np

from environment import debris

# Environment-related
max_range = np.array([0.15, 0.15, 0.15])                          # Action maximum range
safe_sphere: float = max(debris.height/2, debris.radius) + 0.5    # Safe sphere for collision check  [m]
detumbling_threshold: float = 50                                  # Detumbling threshold in energy terms [J]


# Environment action and states
# States:
# pos_EE (3), vel_EE (3), om_EE (3), om_DEB (3)
#
# Actions:
# d_EE (3)
#

state_dim = 12
action_dim = 3

# Model-related
actor_layers = 256
critic_layers = 256

# Learning rates
actor_lr = 1e-5
critic_lr = 1e-4

# Soft update rate
tau = 0.001

# Discount factor
gamma = 0.99

# Batch size
batch_size = 256

