import torch
import numpy as np
from torch.autograd import Variable
from statsmodels.tsa.stattools import acf
import random


def normal_std(x):
    return x.std() * np.sqrt((len(x) - 1.)/(len(x)))


class Data_utility:
    # train 是训练集占总数据的比例，valid 是验证集占总数据的比例，test 是测试集占总数据的比例
    def __init__(self, file_name, train, valid, cuda, horizon, window, normalize=2):
        self.cuda = cuda
        self.P = window
        self.h = horizon
        self.rawdat = np.loadtxt(file_name, delimiter=',')
        self.dat = np.zeros(self.rawdat.shape)
        self.n, self.m = self.dat.shape     # traffic 数据集：n 个时刻，每个时刻 m 个传感器数据
        self.normalize = normalize
        self.scale = np.ones(self.m)        # 用于存储每列数据绝对值的最大值
        self._normalized()
        # 计算 window 参数
        self.series = self.rawdat[:, random.choice(range(self.rawdat.shape[1]))]
        self.acf = acf(self.series, fft=True, nlags=round(len(self.series) * 0.1))  # 只考虑周期不大于总长 1/10 的情况
        self.w = self._cal_t(self.acf, [])
        if self.w:
            print('Replace args.window with ', self.w[-1])
            self.P = self.w[-1]
        else:
            print('No obvious period! Using default arg...')
        # 计算 window 参数结束
        self._split(int(train * self.n), int((train+valid) * self.n), self.n)
        self.scale = torch.from_numpy(self.scale).float()
        tmp = self.test[1] * self.scale.expand(self.test[1].size(0), self.m)
            
        if self.cuda:
            self.scale = self.scale.cuda()
        self.scale = Variable(self.scale)
        
        self.rse = normal_std(tmp)
        self.rae = torch.mean(torch.abs(tmp - torch.mean(tmp)))
    
    def _normalized(self):   # 数据归一化（等比例地将数据的绝对值压缩至小于 1
       
        if self.normalize == 0:      # 实际无需压缩
            self.dat = self.rawdat
        elif self.normalize == 1:    # 针对全体数据用最大值进行压缩
            max_data = np.max(self.rawdat)
            self.dat = self.rawdat / max_data
        elif self.normalize == 2:      # 针对每一列用其最大值进行压缩（对某一传感器所有时刻点压缩）
            for i in range(self.m):
                self.scale[i] = np.max(np.abs(self.rawdat[:, i]))
                self.dat[:, i] = self.rawdat[:, i] / self.scale[i]

    def _split(self, train, valid, test):
        
        train_set = range(self.P + self.h - 1, train)       # 注意训练集不是从 0 开始
        valid_set = range(train, valid)
        test_set = range(valid, test)
        self.train = self._batchify(train_set)
        self.valid = self._batchify(valid_set)
        self.test = self._batchify(test_set)

    def _batchify(self, idx_set):
        # idx_set 是全时刻序列中相应分集的索引序列
        n = len(idx_set)
        X = torch.zeros((n, self.P, self.m))    # 注意输入的形式
        Y = torch.zeros((n, self.m))
        
        for i in range(n):
            end = idx_set[i] - self.h + 1
            start = end - self.P
            X[i, :, :] = torch.from_numpy(self.dat[start:end, :])       # window 是 X 的第二个维度
            Y[i, :] = torch.from_numpy(self.dat[idx_set[i], :])    # horizon 是输入那段时间 X 往后预测的时刻与 X 最后一个时刻的间隔

        return [X, Y]

    def get_batches(self, inputs, targets, batch_size, shuffle=True):   # 默认将输入的各时间片段顺序打乱（片段内仍时序，片段长度为 window）
        length = len(inputs)        # inputs 的行数
        if shuffle:     # 索引乱序
            index = torch.randperm(length)
        else:
            index = torch.LongTensor(range(length))
        start_idx = 0
        while start_idx < length:
            end_idx = min(length, start_idx + batch_size)
            excerpt = index[start_idx:end_idx]
            X = inputs[excerpt]
            Y = targets[excerpt]
            if self.cuda:
                X = X.cuda()
                Y = Y.cuda()  
            yield Variable(X), Variable(Y)
            start_idx += batch_size

    def _cal_t(self, val, idx):
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
                t = self._cal_t(v, i)
                if t:
                    re.append(t[0])
            return re
