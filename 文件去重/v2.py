import os
import hashlib
from pathlib import Path


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
	allMD5 = []
	count = 0
	for f in Path().rglob('*'):
		if not Path(f).is_dir():
			tempMD5 = md5sum(f)
			if tempMD5 in allMD5:
				print('del', f.name)
				os.remove(f.name)
				count += 1
			else:
				allMD5.append(tempMD5)
	input('\n删除了' + str(count) + '个重复的文件副本\n')
