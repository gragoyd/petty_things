import numpy as np
import random


data = np.loadtxt(r'C:\Users\PC\Desktop\MyNet\data\traffic.txt', delimiter=',')
n, m = data.shape
fft_data = np.zeros((n, m))
w, T = [], []
s = random.sample(range(m), 50)
for i in s:
	t = max(abs(data[:, i]))
	data[:, i] = data[:, i] / t
	fft_data[:, i] = np.fft.fft(data[:, i])
	t = max(fft_data[:, i]) / 30      # 取最大幅值的 1/30 为阀值
	for j in range(n//2, n-10):     # 本应该为 [n//2, n) 的，但会出现不合理的大周期
		if fft_data[j, i] > t:
			w.append(n-j)
			T.append(round(n/24/(n-j)))        # Fn = Fs*n/N   以一天为单位时间，则 Fs=24
	T = list(set(T))
	T.sort(reverse=True)
	print(T)
	w.clear()
	T.clear()
