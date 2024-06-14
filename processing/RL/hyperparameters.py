import numpy as np

from environment import debris

# Environment-related
max_action = 0.0174533                                            # Action maximum range [rad]
safe_sphere: float = max(debris.height/2, debris.radius) + 0.5    # Safe sphere for collision check  [m]
detumbling_threshold: float = 50                                  # Detumbling threshold in energy terms [J]


# Environment action and states
# States:
# pos_EE (3), vel_EE (3), om_EE (3), om_DEB (3)
#
# Actions:
# d_EE (2)
#

state_dim = 12
action_dim = 2

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

