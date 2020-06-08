import torch
import numpy as np
from torch.autograd import Variable
from statsmodels.tsa.stattools import acf
import random


class Data_utility:
    def __init__(self, file_name, train, valid, cuda, horizon, window, normalize=2):
        self.cuda = cuda
        self.P = window
        self.h = horizon
        self.rawdat = np.loadtxt(file_name, delimiter=',')
        self.dat = np.zeros(self.rawdat.shape)
        self.n, self.m = self.dat.shape
        self.normalize = normalize
        self.scale = np.ones(self.m)
        self._normalized()
        self.series = self.rawdat[:, random.choice(
            range(self.rawdat.shape[1]))]
        self.acf = acf(self.series, fft=True,
                       nlags=round(len(self.series) * 0.1))
        self.w = self._cal_t(self.acf, [])
        if self.w:
            print('Replace args.window with ', self.w[-1])
            self.P = self.w[-1]
        else:
            print('No obvious period! Using default arg...')
        self._split(int(train * self.n), int((train+valid) * self.n), self.n)
        self.scale = torch.from_numpy(self.scale).float()
        tmp = self.test[1] * self.scale.expand(self.test[1].size(0), self.m)

        if self.cuda:
            self.scale = self.scale.cuda()
        self.scale = Variable(self.scale)

        self.rse = tmp.std() * np.sqrt((len(tmp) - 1.)/(len(tmp)))
        self.rae = torch.mean(torch.abs(tmp - torch.mean(tmp)))

    def _normalized(self):

        if self.normalize == 0:
            self.dat = self.rawdat
        elif self.normalize == 1:
            max_data = np.max(self.rawdat)
            self.dat = self.rawdat / max_data
        elif self.normalize == 2:
            for i in range(self.m):
                self.scale[i] = np.max(np.abs(self.rawdat[:, i]))
                self.dat[:, i] = self.rawdat[:, i] / self.scale[i]

    def _split(self, train, valid, test):

        train_set = range(self.P + self.h - 1, train)
        valid_set = range(train, valid)
        test_set = range(valid, test)
        self.train = self._batchify(train_set)
        self.valid = self._batchify(valid_set)
        self.test = self._batchify(test_set)

    def _batchify(self, idx_set):
        n = len(idx_set)
        X = torch.zeros((n, self.P, self.m))
        Y = torch.zeros((n, self.m))

        for i in range(n):
            end = idx_set[i] - self.h + 1
            start = end - self.P
            X[i, :, :] = torch.from_numpy(self.dat[start:end, :])
            Y[i, :] = torch.from_numpy(self.dat[idx_set[i], :])

        return [X, Y]

    def get_batches(self, inputs, targets, batch_size, shuffle=True):
        length = len(inputs)
        if shuffle:
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
        if len(val) < 3:
            return []
        else:
            i, v, re = [], [], []
            for n in range(len(val) - 2):
                if 0.5 < val[n] < val[n + 1] and val[n + 1] > val[n + 2]:
                    v.append(val[n + 1])
                    if idx:
                        i.append(idx[n + 1])
                    else:
                        i.append(n + 1)
            if i:
                re.append(i[0])
                if max(v) > min(v) * 1.25:
                    t = self._cal_t(v, i)
                    if t:
                        re.append(t[0])
            return re
