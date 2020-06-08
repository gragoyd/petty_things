import re
import matplotlib.pyplot as plt

h = 24
rgr, rgc = 0.5633, 0.8300
lar, lac = 0.5300, 0.8429
lsr, lsc = 0.4973, 0.8588
thr, thc = 0.4976, 0.8645

data = open('log_traffic/Net_h_' + str(h) + '.txt', 'r', encoding='UTF-8').read()
pr = re.compile(r'test rse \d.\d\d\d\d')
pc = re.compile(r'test corr \d.\d\d\d\d')
rrse = pr.findall(data)
corr = pc.findall(data)
t = re.compile(r'\d.\d\d\d\d')
for i in range(len(rrse)):
	rrse[i] = float(t.findall(rrse[i])[0])
	corr[i] = float(t.findall(corr[i])[0])

plt.figure(1)
s1 = plt.subplot(121)
p1, = plt.plot(range(5, 101, 5), rrse[:20], 'bo-')
p2, = plt.plot(range(5, 101, 5), [rgr for i in range(20)], color="green", linestyle="--")
p3, = plt.plot(range(5, 101, 5), [lar for i in range(20)], color="orange", linestyle="--")
p4, = plt.plot(range(5, 101, 5), [lsr for i in range(20)], color="cadetblue", linestyle="--")
p5, = plt.plot(range(5, 101, 5), [thr for i in range(20)], color="purple", linestyle="--")
l1 = plt.legend([p1, p2, p3, p4, p5], ["Trend", "RNN-GRU", 'LST-Attn', 'LST-Skip', 'THIS'], loc='best')
plt.ylim(min(rrse) - 0.01, max(rrse) + 0.01)
plt.xlabel('epoch')
plt.ylabel('value')
s1.set_title('RRSE')

s2 = plt.subplot(122)
p1, = plt.plot(range(5, 101, 5), corr[:20], 'r^-')
p2, = plt.plot(range(5, 101, 5), [rgc for i in range(20)], color="green", linestyle="--")
p3, = plt.plot(range(5, 101, 5), [lac for i in range(20)], color="orange", linestyle="--")
p4, = plt.plot(range(5, 101, 5), [lsc for i in range(20)], color="cadetblue", linestyle="--")
p5, = plt.plot(range(5, 101, 5), [thc for i in range(20)], color="purple", linestyle="--")
l1 = plt.legend([p1, p2, p3, p4, p5], ["Trend", "RNN-GRU", 'LST-Attn', 'LST-Skip', 'THIS'], loc='best')
plt.ylim(min(corr) - 0.01, max(corr) + 0.01)
plt.xlabel('epoch')
plt.ylabel('value')
s2.set_title('CORR')

plt.show()
