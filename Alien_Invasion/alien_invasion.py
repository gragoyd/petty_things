#   后期拓展思路：
#   使外星人也能向飞船射击
#   可添加盾牌，使飞船可躲至盾牌后或可使飞船免受一定次数的攻击
#   可使用 pygame.mixer 模块添加音效（爆炸声和射击声等）


import pygame
from pygame.sprite import Group
from settings import Settings
from game_stats import GameStats
from scoreboard import Scoreboard
from button import Button
from ship import Ship
import game_functions as gf


def run_game():
	# 初始化游戏并创建一个屏幕对象
	pygame.init()
	ai_settings = Settings()
	screen = pygame.display.set_mode((ai_settings.screen_width, ai_settings.screen_height))
	pygame.display.set_caption("Alien Invasion")
	screen.fill(ai_settings.bg_color)

	# 创建一个用于存储游戏统计信息的实例
	stats = GameStats(ai_settings)
	sb = Scoreboard(ai_settings, screen, stats)

	# 创建 Play 按钮
	play_button = Button(screen, "Play")

	# 创建飞船、子弹与外星人编组
	ship = Ship(ai_settings, screen)
	bullets = Group()
	aliens = Group()
	# 创建外星人群
	gf.create_fleet(ai_settings, screen, ship, aliens)

	# 开始游戏主循环
	while True:
		gf.check_events(ai_settings, screen, stats, sb, play_button, ship, aliens, bullets)

		if stats.game_active:
			ship.update()
			gf.update_bullets(ai_settings, screen, stats, sb, ship, aliens, bullets)
			gf.update_aliens(ai_settings, stats, screen, sb, ship, aliens, bullets)

		gf.update_screen(ai_settings, screen, stats, sb, ship, aliens, bullets, play_button)


run_game()
