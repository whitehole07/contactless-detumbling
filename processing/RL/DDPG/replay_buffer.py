import numpy as np
import torch


class ReplayBuffer(object):
	def __init__(self, state_dim, action_dim, max_size=int(1e6)):
		self.max_size = max_size
		self.ptr = 0
		self.size = 0

		self.state = np.zeros((max_size, state_dim))
		self.action = np.zeros((max_size, action_dim))
		self.next_state = np.zeros((max_size, state_dim))
		self.reward = np.zeros((max_size, 1))
		self.not_done = np.zeros((max_size, 1))

		self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")


	def add(self, state, action, next_state, reward, done):
		self.state[self.ptr] = state
		self.action[self.ptr] = action
		self.next_state[self.ptr] = next_state
		self.reward[self.ptr] = reward
		self.not_done[self.ptr] = 1. - done

		self.ptr = (self.ptr + 1) % self.max_size
		self.size = min(self.size + 1, self.max_size)

	def copy(self, replay_buffer):
		self.size = replay_buffer.size
		self.state = replay_buffer.state.copy()
		self.action = replay_buffer.action.copy()
		self.next_state = replay_buffer.next_state.copy()
		self.reward = replay_buffer.reward.copy()
		self.not_done = replay_buffer.not_done.copy()

		# Sort by reward for improved experience replay
		self.sort_by_reward()

	def sort_by_reward(self):
		# Get the indices that would sort the rewards in descending order
		sorted_indices = np.argsort(self.reward[:self.size].flatten())[::-1]

		# Reorder all arrays based on these sorted indices
		self.state[:self.size] = self.state[sorted_indices]
		self.action[:self.size] = self.action[sorted_indices]
		self.next_state[:self.size] = self.next_state[sorted_indices]
		self.reward[:self.size] = self.reward[sorted_indices]
		self.not_done[:self.size] = self.not_done[sorted_indices]

	def sample(self, batch_size):
		un_ind = np.random.randint(0, self.size, size=batch_size)

		return (
			torch.FloatTensor(self.state[un_ind]).to(self.device),
			torch.FloatTensor(self.action[un_ind]).to(self.device),
			torch.FloatTensor(self.next_state[un_ind]).to(self.device),
			torch.FloatTensor(self.reward[un_ind]).to(self.device),
			torch.FloatTensor(self.not_done[un_ind]).to(self.device)
		)

	def improved_sample(self, batch_size, w_hp):
		""" w_hp, weight for high priority (higher rewards) sampling """
		num_hp_samples = int(w_hp * batch_size)
		num_uniform_samples = batch_size - num_hp_samples

		# High-priority indices (assuming buffer is already sorted by reward)
		hp_ind = np.arange(num_hp_samples)

		# Uniformly sampled indices (excluding the high-priority range)
		un_ind = np.random.randint(num_hp_samples, self.size, size=num_uniform_samples)

		# Combine high-priority and uniformly sampled indices
		indices = np.concatenate((hp_ind, un_ind))

		return (
			torch.FloatTensor(self.state[indices]).to(self.device),
			torch.FloatTensor(self.action[indices]).to(self.device),
			torch.FloatTensor(self.next_state[indices]).to(self.device),
			torch.FloatTensor(self.reward[indices]).to(self.device),
			torch.FloatTensor(self.not_done[indices]).to(self.device)
		)
