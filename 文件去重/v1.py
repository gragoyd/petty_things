import os
import hashlib


def md5sum(filename):
	f = open(filename, 'rb')
	md5 = hashlib.md5()
	while True:
		fb = f.read(8096)
		if not fb:
			break
		md5.update(fb)
	f.close()
	return md5.hexdigest()


if __name__ == '__main__':
	input('\n将脚本置于需去重的顶层文件夹内并按回车继续\n\n')
	print('开始处理\n')
	allMD5, sameFile = {}, {}
	fileDir = os.walk(os.getcwd())
	for i in fileDir:
		for f in i[2]:
			tempMD5 = md5sum(f)
			if tempMD5 in allMD5.keys():    # 有重复
				if tempMD5 in sameFile.keys():
					sameFile[tempMD5].append(f)
				else:
					sameFile[tempMD5] = [allMD5[tempMD5], f, ]
			else:
				allMD5[tempMD5] = f
	if sameFile:
		print('找到如下重复文件\n\n')
		for k, v in sameFile.items():
			print(k, ":")
			v.sort(key=len)
			for name in v:
				print('\t', name)
			print()
		input('按任意键开始去重')
		for v in sameFile.values():
			for i in range(1, len(v)):
				print('del', v[i])
				os.remove(v[i])
		input('已完成文件去重\n')
	else:
		input('未找到重复文件\n')
