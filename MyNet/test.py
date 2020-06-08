
from statsmodels.tsa.stattools import acf
import numpy as np
from dtw import dtw
import random


def cal_dtw(series, p):
	"""随机取长度为 5*p 的片段计算小片段两两相似度以验证"""
	dist = []
	start = random.choice(range(len(series) - 5 * p))  # 保证不越界
	delta = max(series) - min(series)
	for i in range(4):
		for j in range(i + 1, 5):
			d = dtw(series[start + i * p:start + (i + 1) * p].reshape(-1, 1),
			        series[start + j * p:start + (j + 1) * p].reshape(-1, 1),
			        dist=lambda x, y: np.linalg.norm(x - y))
			dist.append(d[0] / p / delta)
	for i in range(10):  # 保证每个相似度都较高
		if dist[i] > 0.025:
			print(dist)
			return False
	return True


def cal_t(val, idx):
	"""递归检测参数中存在的全部尖峰形式的周期"""
	if len(val) < 3:
		return []
	else:
		i, v, re = [], [], []
		for n in range(len(val) - 2):
			if 0.5 < val[n] < val[n + 1] and val[n + 1] > val[n + 2]:  # 要求正相关性程度不低于显著相关
				v.append(val[n])
				if idx:
					i.append(idx[n + 1])
				else:
					i.append(n + 1)
		if i:
			re.append(i[0])  # 所需只是最小周期
			t = cal_t(v, i)
			if t:
				re.append(t[0])
		return re


row_data = np.loadtxt('data/traffic.txt', delimiter=',')
for i in range(20):
	print('i = ', i)
	data = row_data[:, random.choice(range(row_data.shape[1]))]
	ACF = acf(data, fft=True, nlags=round(len(data) * 0.1))  # 只考虑周期不大于总长 1/10 的情况
	period = cal_t(ACF, [])
	print(period)
	if period:
		if cal_dtw(data, period[-1]):
			arg = period[-1]
			print('success, period = ', arg)
		else:
			print('DTW too far! Using default arg...')
	else:
		print('No obvious period! Using default arg...')


