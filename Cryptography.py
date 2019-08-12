"""
加解密默认使用内置密钥，若使用外部密钥，则需为本程序生成的密钥
内置 31 组长度密钥，默认每日更换密钥
当天消息加解密无需指定密钥序号，非当天消息则需先指定内置密钥序号
--------------------东东制作-----------------------
"""

from cryptography.fernet import Fernet
import datetime

tips = '''代码\t操作
E\t加密模式（输入“Q/q”退出加密模式）
D\t解密模式（输入“Q/q”退出解密模式）
N\t指定密钥序号(1 - 31)
K\t使用外部密钥（本次运行有效）
G\t生成密钥'''

# 31 组密钥元组
keyList = (
	b'2I9l9fHzpl1ozr4oBX8nRXZ-zYIYfoVps17xdUATVWM=',
	b'Rwaiq4lYShW62MvHDgSFLIIsdMSl-csYGCO1tEo3hFc=',
	b'3iV7R9JEdVNVOjEJPEg2MceDqzmhMKBxKReEAlQwEoo=',
	b'XV4gJOf1G1dztmMKW8hwXrTMWGUmrg8-HZ52Ob-Y1ss=',
	b'a01OiA7criMKb7A3nfJ8C0QBt6cXkoKkMnqQW311IlM=',
	b'7oRyryXtu1mkI8qAd15448GgxtFDiM8uBfjmkL_kF-c=',
	b'-uhuD27jDxPQoaocrV5lmJEBqR_VlIBo1S3M2sS24-U=',
	b'YiMJKvRllUXFVIPIYcZ4oWiBZxcDbxY3ym27lLUZLCg=',
	b'3Gv73yk2V06AH2THKePaqvO-MSHsSP5LW059sWPtoCs=',
	b'f5KHH0H_imvObaRXRsVCOdRkY514b7nkyI1SLkMMOCo=',
	b'7rbh8S9wI6BUtyo62xcZfiRGd_hxqEid-pD6UCCUlrw=',
	b'4uA0IW34tX3RNKE2kchva2SsBw2DUal_EBkjhTFlXW4=',
	b'Y5Eq39_l1vThWb6lNCTtlq_tv13YMelkPXTSMX_vWEU=',
	b'LuukokNnGjqlZer7RfE2QNY0NVEh6My2sMRo9IJVnJQ=',
	b'962gZz1IqNw_PPefN9l5YstAq5XW7WzwwZK-6cSnL7c=',
	b'NYvtGJelwDeD6zKv8o_dUkB1-N3_CzUknTBJQhPYfPM=',
	b'T1u5KrKEBxwCVhAVB0WeMxb2k5u5y-au-VD39tQ-8lw=',
	b'abLFVqTZsJwJps7cL_MyVjRAeXiy6EZ4LZs_khPj_Sg=',
	b'-RYn8kYuGpKNKDbB73Sz93dXC-mJFArKXKG4l0Mg_Dk=',
	b'bGYfWH75_8IvTUhiG1wbGrxTwlfJAm13ojNinF_bVyI=',
	b'lyh-s3QVq3sqO7UOWPX6W4Ew84ZkvL_dwSndPe7sPlc=',
	b'Omnu35Bikazh8l9yGYYygQQXH_u-9Q7MSK6LG2qbph4=',
	b'BbgcpNFLMb8rfe_SvHhN6-ASGrVIvpxmOtflkY9Qggc=',
	b'HNcKX1a0A3Ss4ROu4CpAKZsqXi39TE-8Rqrp9FuviSI=',
	b'1K0jactwOrM34zENhy2gRZAH_QK_MTh1Mx-s0U4GStQ=',
	b'ZlWckcDwBVtItfreBWjxnEEceKbQ9JddCrhekj6VWHw=',
	b'm_Dz0GcvBuRLSD_oLly0wK33AoQ4q2Tv4TPhKnj-zjQ=',
	b'zPVN72rHQ2f2bam7FQ1OpJEut9mWMar-ZgNVZznMsfA=',
	b'rpmF3tKMLuXzRQtvappk2vM-zGJnBTUJ6pyzc3DnQb0=',
	b'ilJ3PIa0JWvcomx4Nw_CPguDdx_fTlGqB3oqlhtga8E=',
	b'mIDcTVI4TBcl46pKB69rlYamIoWMotWmEbbQeLHgquQ=',
)


def my_encode(key):
	f = Fernet(key)
	plain = input('输入明文：\n')
	while True:
		if 'q' == plain[:].lower():
			break
		print(f.encrypt(plain[:].encode()).decode())
		plain = input()


def my_decode(key):
	f = Fernet(key)
	secret = input('输入密文：\n')
	while True:
		if 'q' == secret[:].lower():
			break
		try:
			print(f.decrypt(secret[:].encode()).decode())
		except:
			print('密文非法！退出解密模式')
			break
		secret = input()


def get_new_key():
	temp_key = input('输入密钥：\n')
	try:
		f = Fernet(temp_key[:].encode())
	except:
		print('外部密钥非法！', end=' ')
		temp_key = get_new_key()
	return temp_key


def change_inside_key():
	try:
		index = int(input('输入序号（1 ~ 31）：\n'))
	except ValueError:
		print('乖乖输入数字！！！')
	else:
		while index < 1 or index > 31:
			index = int(input('无效序号，重新输入序号（1 ~ 31）：\n'))
		return keyList[index - 1]


# 获取系统日期，确定今天是几号
keyIndex = int(datetime.datetime.now().strftime('%d'))
key = keyList[keyIndex - 1]

print(__doc__)
while True:
	print(tips)
	option = input('输入操作代码（不区分大小写）：\n').lower()

	if 'n' == option:
		key = change_inside_key()
	elif 'k' == option:
		key = get_new_key().encode()
	elif 'e' == option:
		my_encode(key)
	elif 'd' == option:
		my_decode(key)
	elif 'g' == option:
		print(Fernet.generate_key().decode())
	else:
		print('无效代码，重新输入：\n')

