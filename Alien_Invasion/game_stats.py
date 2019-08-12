class GameStats():
	"""跟踪游戏的统计信息"""

	def __init__(self, ai_settings):
		"""初始化统计信息"""
		self.ai_settings = ai_settings
		self.reset_stats()
		self.game_active = False
		# 在任何情况下都不应重置最高分
		self.high_score = self.load_high_score()

	def reset_stats(self):
		"""初始化在游戏运行期间可能变化的统计信息"""
		self.ships_left = self.ai_settings.ship_limit
		self.score = 0
		self.level = 1

	def load_high_score(self):
		"""开始时加载历史最高分"""
		with open('high_score.txt') as f_obj:
			high_score = int(f_obj.read())
		return high_score

	def store_high_score(self):
		"""结束时存储最高分"""
		with open('high_score.txt', 'w') as f_obj:
			f_obj.write(str(self.high_score))
