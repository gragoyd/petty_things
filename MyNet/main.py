# 命令行运行：python main.py --model Net --skip -1 --save temp.pt
# 正式运行的时候记得改：args 的 save, epochs=100, skip=24（默认 skip=-1 来禁用 skip-RNN）

import argparse     # 用于解析 sys.argv 中的参数
import math
import time

import torch
import torch.nn as nn
import Net
import LSTNet
import numpy as np
import importlib
import matplotlib.pyplot as plt

from utils import *
import Optim


def evaluate(data, X, Y, model, evaluateL2, evaluateL1, batch_size):
    model.eval()
    total_loss = 0
    total_loss_l1 = 0
    n_samples = 0
    predict = None
    test = None

    with torch.no_grad():       # torch v1.0 以上得加这个，避免存梯度爆内存
        for X, Y in data.get_batches(X, Y, batch_size, False):
            output = model(X)
            if predict is None:
                predict = output
                test = Y
            else:
                predict = torch.cat((predict, output))
                test = torch.cat((test, Y))

            scale = data.scale.expand(output.size(0), data.m)
            total_loss += evaluateL2(output * scale, Y * scale).data
            total_loss_l1 += evaluateL1(output * scale, Y * scale).data
            n_samples += (output.size(0) * data.m)
        rse = math.sqrt(total_loss / n_samples)/data.rse
        rae = (total_loss_l1/n_samples)/data.rae

    predict = predict.data.cpu().numpy()
    Ytest = test.data.cpu().numpy()
    sigma_p = predict.std(axis=0)
    sigma_g = Ytest.std(axis=0)
    mean_p = predict.mean(axis=0)
    mean_g = Ytest.mean(axis=0)
    index = (sigma_g != 0)
    correlation = ((predict - mean_p) * (Ytest - mean_g)
                   ).mean(axis=0)/(sigma_p * sigma_g)
    correlation = (correlation[index]).mean()
    return rse, rae, correlation


def train(data, X, Y, model, criterion, optim, batch_size):
    model.train()
    total_loss = 0
    n_samples = 0
    for X, Y in data.get_batches(X, Y, batch_size, True):
        model.zero_grad()
        output = model(X)
        scale = data.scale.expand(output.size(0), data.m)
        loss = criterion(output * scale, Y * scale)
        loss.backward()
        grad_norm = optim.step()
        total_loss += loss.data
        n_samples += (output.size(0) * data.m)
    return total_loss / n_samples


parser = argparse.ArgumentParser(description='PyTorch Time series forecasting')
# parser.add_argument('--data', type=str, required=True, help='location of the data file')
parser.add_argument('--data', type=str, default='data/traffic.txt',
                    help='location of the data file')
parser.add_argument('--window', type=int, default=24 * 7, help='window size')
parser.add_argument('--horizon', type=int, default=1)
# 感觉这就是论文里 skip-RNN 的超参数 p
parser.add_argument('--skip', type=int, default=-1)
parser.add_argument('--hidSkip', type=int, default=5)

parser.add_argument('--epochs', type=int, default=100,
                    help='upper epoch limit')
parser.add_argument('--highway_window', type=int, default=24,
                    help='The window size of the highway component')

parser.add_argument('--model', type=str, default='Net', help='')
parser.add_argument('--hidCNN', type=int, default=100,
                    help='number of CNN hidden units')
parser.add_argument('--hidRNN', type=int, default=100,
                    help='number of RNN hidden units')
parser.add_argument('--CNN_kernel', type=int, default=6,
                    help='the kernel size of the CNN layers')
parser.add_argument('--clip', type=float, default=10.,
                    help='gradient clipping')
parser.add_argument('--batch_size', type=int, default=128,
                    metavar='N', help='batch size')
parser.add_argument('--dropout', type=float, default=0.1,
                    help='dropout applied to layers (0 = no dropout)')
parser.add_argument('--seed', type=int, default=54321, help='random seed')
parser.add_argument('--gpu', type=int, default=None)
parser.add_argument('--log_interval', type=int, default=2000,
                    metavar='N', help='report interval')
parser.add_argument('--save', type=str,  default='Net_h_3.pt',
                    help='path to save the final model')
parser.add_argument('--cuda', type=bool, default=True)
parser.add_argument('--optim', type=str, default='adam')
parser.add_argument('--lr', type=float, default=0.001)
parser.add_argument('--L1Loss', type=bool, default=True)
parser.add_argument('--normalize', type=int, default=2)
parser.add_argument('--output_fun', type=str, default='sigmoid')
args = parser.parse_args()

args.cuda = args.gpu is not None
if args.cuda:
    torch.cuda.set_device(args.gpu)
# Set the random seed manually for reproducibility.
torch.manual_seed(args.seed)
if torch.cuda.is_available():
    if not args.cuda:
        print("有个 CUDA 设备，可能忘了用，强制帮你用hhh")
        args.cuda = True
        torch.cuda.set_device(0)
    else:
        torch.cuda.manual_seed(args.seed)

Data = Data_utility(args.data, 0.6, 0.2, args.cuda,
                    args.horizon, args.window, args.normalize)
print('RSE:', Data.rse)

model = eval(args.model).Model(args, Data)      # 调用 LSTNet / Net 模块
if args.cuda:
    model.cuda()
nParams = sum([p.nelement() for p in model.parameters()])
print('* number of parameters: %d' % nParams)

if args.L1Loss:
    # 原为：criterion = nn.L1Loss(size_average=False)
    criterion = nn.L1Loss(reduction='sum')
else:
    # 原为：criterion = nn.MSELoss(size_average=False)
    criterion = nn.MSELoss(reduction='sum')
# 原为：evaluateL2 = nn.MSELoss(size_average=False)
evaluateL2 = nn.MSELoss(reduction='sum')
# 原为：evaluateL1 = nn.L1Loss(size_average=False)
evaluateL1 = nn.L1Loss(reduction='sum')
if args.cuda:
    criterion = criterion.cuda()
    evaluateL1 = evaluateL1.cuda()
    evaluateL2 = evaluateL2.cuda()


best_val = 10000000
optim = Optim.Optim(model.parameters(), args.optim,
                    args.lr, args.clip, lr_decay=0.9)

try:    # 任何时候都能按 Ctrl + C 立刻终止训练
    print('begin training')
    for epoch in range(1, args.epochs+1):
        epoch_start_time = time.time()
        train_loss = train(
            Data, Data.train[0], Data.train[1], model, criterion, optim, args.batch_size)
        val_loss, val_rae, val_corr = evaluate(Data, Data.valid[0], Data.valid[1],
                                               model, evaluateL2, evaluateL1, args.batch_size)
        print('| end of epoch {:3d} | time: {:5.2f}s | train_loss {:5.4f} | valid rse {:5.4f} | valid rae {:5.4f} '
              '| valid corr  {:5.4f}'.format(epoch, (time.time() - epoch_start_time),
                                             train_loss, val_loss, val_rae, val_corr))

        if val_loss < best_val:     # 若总损失是迄今为止最好的，就保存模型
            with open(args.save, 'wb') as f:
                torch.save(model, f)
            best_val = val_loss
        else:   # 也许需要衰减学习速率
            optim.updateLearningRate(val_loss, epoch)       # ###############若用 adam 的话，仿佛不用操心这个。。。
        if epoch % 5 == 0:      # 每五个 epoch 测试一次
            test_acc, test_rae, test_corr = evaluate(Data, Data.test[0], Data.test[1],
                                                     model, evaluateL2, evaluateL1, args.batch_size)
            print("test rse {:5.4f} | test rae {:5.4f} | test corr {:5.4f}".format(
                test_acc, test_rae, test_corr))

except KeyboardInterrupt:
    print('-' * 89)
    print('Exiting from training early')

with open(args.save, 'rb') as f:
    model = torch.load(f)
test_acc, test_rae, test_corr = evaluate(Data, Data.test[0], Data.test[1],
                                         model, evaluateL2, evaluateL1, args.batch_size)
print("Reload the model with the best performance in valid loss to test:")
print("test rse {:5.4f} | test rae {:5.4f} | test corr {:5.4f}".format(
    test_acc, test_rae, test_corr))
