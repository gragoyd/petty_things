import itchat
import requests
import threading
import time
import hashlib


allowRobot = [         # 允许接入机器人的用户
	'辛诚愚', '王天亨', '王闯', '乔育萱', '殷浩庭', '吴明洋', '聂安然',
]
apiKey = 'dd9bc102e12942b093d44974781cadfc'
apiUrl = 'http://www.tuling123.com/openapi/api'
talkingUsers = {}       # 存储本次程序运行时向我发送过消息的用户
lock = threading.Lock()


def wait_reply(nickname, username):
	for count in range(60):
		time.sleep(1)
		lock.acquire()
		if not talkingUsers[username]['count']:     # 若计时被中断则不接入机器人，直接返回
			return
		lock.release()
	talkingUsers[username]['robot'] = True
	declare = '您好，东东已经有一分钟没回复你消息，现在接入机器人，仅供娱乐，注意保护个人隐私。\n'
	menu = '我能和您聊天，也能帮您搜图、查询新闻、列车、航班类信息，甚至能教您做菜哟~~~\n'
	explanation = '连接类命令：\n接入机器人：-接入\n断开机器人：-断开'
	itchat.send(nickname + declare + menu + explanation, toUserName=username)


def add_user(msg):
	return {
		'userNameMd5': hashlib.md5(msg['User']['UserName'].encode()).hexdigest(),
		'nickName': msg['User']['NickName'],
		'remarkName': msg['User']['RemarkName'],
		'count': True,      # 是否允许继续计数等待（注意：若改为 False, 没有地方再改为 True）
		'robot': False,     # 是否允许机器人接入
		'customRobot': True,    # 是否允许对方自由设定是否接入机器人
		'thread': threading.Thread(target=wait_reply, args=(msg['User']['NickName'], msg['User']['UserName']))
	}


def get_response(username, msg):
	data = {
		'key': apiKey,
		'info': msg,
		# 实际发送的用户名为加密后的，避免泄露信息
		'userid': talkingUsers[username]['userNameMd5']
	}
	try:
		r = requests.post(apiUrl, data=data).json()
	except:
		return '机器人貌似出了个小错......'
	code = r.get('code')
	if 100000 == code:
		return r.get('text')
	elif 200000 == code:
		return r.get('text') + '，链接为：\n' + r.get('url')
	elif 302000 == code:
		news = ''
		for block in r.get('list'):
			news += block['article'] + ' [' + block['source'] + ']\n' + block['detailurl'] + '\n\n'
		return r.get('text') + '\n' + news
	elif 308000 == code:
		cookbook = ''
		for block in r.get('list'):
			cookbook += block['name'] + '\n'
			if block['icon']:
				cookbook += '示例链接为：' + block['icon'] + '\n'
			cookbook += '食材为' + block['info'] + '\n' + '详情链接：' + '\n' + block['detailurl'] + '\n\n'
			return r.get('text') + '\n' + cookbook
	elif 40001 == code:
		return '[' + r.get('text') + ']'
	elif 40002 == code:
		return '您发送的消息为空'
	elif 40004 == code:
		talkingUsers[username]['robot'] = False
		talkingUsers[username]['customRobot'] = False
		return '[机器人今天需要休息啦，想玩明天再继续和我聊吧]'
	elif 40007 == code:
		return '[数据格式异常]'
	else:
		return '[未知返回数据类型]'


@itchat.msg_register(itchat.content.TEXT)
def catch_msg(msg):
	"""截获系统消息并适当地接入机器人回复消息"""
	if msg['User']['RemarkName'] in allowRobot:
		fromusername = msg['FromUserName']
		tousername = msg['ToUserName']
		# 每次登陆网页版微信，分配的收发用户码会改变
		if tousername != msg['User']['UserName']:    # 若接收方不是对方，则是我收消息
			if fromusername in talkingUsers.keys():
				if talkingUsers[fromusername]['customRobot']:
					# 对方自己设定是否接入机器人
					if '-接入' == msg['Text']:
						talkingUsers[fromusername]['robot'] = True
						itchat.send('[机器人已接入]', toUserName=fromusername)
						return
					elif '-断开' == msg['Text']:
						talkingUsers[fromusername]['robot'] = False
						itchat.send('[机器人已断开]', toUserName=fromusername)
						return
				if talkingUsers[fromusername]['robot']:
					if len(msg['Text']) > 30:       # 图灵 API 的对话数据要求
						return '[与机器人对话的消息需为 30 字之内]'
					else:
						reply = get_response(fromusername, msg['Text'])
						# 保证机器人出问题的时候也能有默认的回复
						return reply or '我已收到如下内容:\n' + msg['Text']
				else:
					pass        # 可能加入正在聊天然后我突然没聊了的识别回复功能
			else:
				talkingUsers[fromusername] = add_user(msg)
				talkingUsers[fromusername]['thread'].start()
				talkingUsers[fromusername]['thread'].join()
		else:      # 若我发消息
			if tousername in talkingUsers.keys():
				if '-接入' == msg['Text']:
					talkingUsers[tousername]['robot'] = True
					itchat.send('[机器人接入--高权限命令]', toUserName=tousername)
					return
				elif '-断开' == msg['Text']:
					talkingUsers[tousername]['robot'] = False
					itchat.send('[机器人断开--高权限命令]', toUserName=tousername)
					return
				elif '-' == msg['Text']:  # 用‘-’来切换是否允许机器人接入
					if talkingUsers[tousername]['customRobot']:
						talkingUsers[tousername]['customRobot'] = False
						itchat.send('[已禁止您自定义是否接入机器人]', toUserName=tousername)
					else:
						talkingUsers[tousername]['customRobot'] = True
						itchat.send('[已允许您自定义是否接入机器人]', toUserName=tousername)
					return

				# 若还在进行一分钟计时，则中断
				if talkingUsers[tousername]['count']:
					talkingUsers[tousername]['count'] = False
				# 若仍接入机器人，则断开
				if talkingUsers[tousername]['robot']:
					talkingUsers[tousername]['robot'] = False
					itchat.send('[机器人已断开]', toUserName=tousername)


@itchat.msg_register(itchat.content.NOTE)
def get_note(msg):
	"""截获系统消息并提醒抢红包"""
	print(msg)
	if '红包' in msg['Text']:
		import winsound
		winsound.Beep(3000, 3000)

		# import pygame
		# pygame.mixer.init()
		# pygame.mixer.music.load('好运来.mp3')
		# pygame.mixer.music.play()


itchat.auto_login(hotReload=True, enableCmdQR=2)
itchat.run()
