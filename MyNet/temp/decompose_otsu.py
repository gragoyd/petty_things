import numpy as np
import matplotlib.pyplot as plt
from statsmodels.tsa.seasonal import seasonal_decompose

# 画自相关图
# from statsmodels.graphics.tsaplots import plot_acf
# plot_acf(data, lags=2000, fft=True).show()


rowdata = np.loadtxt('data/traffic.txt', delimiter=',')
rowdata = rowdata.T
data = rowdata[0]

d = seasonal_decompose(data, model='additive', period=24, two_sided=False)  # 或 model='multiplicative'
trend = d.trend
# 注意 trend 的第 0 ~ 23 为 nan，分解出的其它序列可能一样
fig, ax = plt.subplots(nrows=4, ncols=1)
ax[0].plot(data[3000:3000 + 24 * 15], color='blue')
ax[0].set_ylabel("observed")
ax[1].plot(d.trend[3000:3000 + 24 * 15], color='red')
ax[1].set_ylabel('trend')
ax[2].plot(d.seasonal[3000:3000 + 24 * 15], color='green')
ax[2].set_ylabel('seasonal')
ax[3].plot(d.resid[3000:3000 + 24 * 15], color='yellow')
ax[3].set_ylabel('residual')
plt.show()


# Otsu 算法

# 先将 trend 各值映射到 0~100 范围内
t = np.delete(trend, [i for i in range(len(trend)) if np.isnan(trend[i])])
t1 = t.copy()
tmax = np.max(t)
tmin = np.min(t)
t = t - tmin
t = np.trunc(t * (100 / tmax)).astype(int)
# 制作直方图
count = np.zeros(101)
for i in range(len(t)):
	count[t[i]] += 1
# 开始 Otsu
ICV = np.zeros(101)
M = round(t.mean())

for i in range(1, 100):
	na = sum(count[:i])
	nb = sum(count[i:])
	if na == 0 or nb == 0:
		ICV[i] = 0
	else:
		pa = na / (na + nb)
		pb = 1 - pa
		ma = sum([j * count[j] for j in range(i)]) / na
		mb = sum([j * count[j] for j in range(i, len(count))]) / nb
		ICV[i] = pa * pow(ma - M, 2) + pb * pow(mb - M, 2)

ICV = ICV.tolist()
line = ICV.index(max(ICV))      # 阀值 line=46
L = t1.min() + 0.46 * (t1.max() - t1.min())     # L 为换算回 trend 中的阀值

for i in range(len(t1)):
	if t1[i] > L:
		t1[i] = 1
	else:
		t1[i] = 0

plt.title('trend')
plt.plot(t1[500:500 + 24 * 20])
plt.show()
