# 命令行运行：python draw.py --save temp.pt --horizon 24
# 记得修改默认参数中的文件

import argparse     # 用于解析 sys.argv 中的参数
import math
import time

import torch
import torch.nn as nn
import Net
import numpy as np
import importlib
import matplotlib.pyplot as plt

from utils import *
import Optim


parser = argparse.ArgumentParser(description='PyTorch Time series forecasting')
# parser.add_argument('--data', type=str, required=True, help='location of the data file')
parser.add_argument('--data', type=str, default='data/traffic.txt',
                    help='location of the data file')
parser.add_argument('--window', type=int, default=24 * 7, help='window size')
parser.add_argument('--horizon', type=int, default=1)
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
parser.add_argument('--save', type=str,  default='temp.pt',
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
args.window = Data.P    # 若替换了 window，也需在 args 中更新，避免后续调用 args 导致错误

# model = eval(args.model).Model(args, Data)
# if args.cuda:
#     model.cuda()
# nParams = sum([p.nelement() for p in model.parameters()])
# print('* number of parameters: %d' % nParams)

if args.L1Loss:
    criterion = nn.L1Loss(reduction='sum')
else:
    criterion = nn.MSELoss(reduction='sum')
evaluateL2 = nn.MSELoss(reduction='sum')
evaluateL1 = nn.L1Loss(reduction='sum')

if args.cuda:
    criterion = criterion.cuda()
    evaluateL1 = evaluateL1.cuda()
    evaluateL2 = evaluateL2.cuda()


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


with open('Net_h_24.pt', 'rb') as f:
    model = torch.load(f)

test_acc, test_rae, test_corr = evaluate(Data, Data.test[0], Data.test[1],
                                         model, evaluateL2, evaluateL1, args.batch_size)
print("Reload the model with the best performance in valid loss to test:")
print("test rse {:5.4f} | test rae {:5.4f} | test corr {:5.4f}".format(
    test_acc, test_rae, test_corr))

# model.eval()
# predict = None
# test = None
#
# with torch.no_grad():       # torch v1.0 以上得加这个，避免存梯度爆内存
#     for X, Y in Data.get_batches(Data.test[0], Data.test[1], args.batch_size, False):
#         output = model(X)
#         if predict is None:
#             predict = output
#             test = Y
#         else:
#             predict = torch.cat((predict, output))
#             test = torch.cat((test, Y))
#
# predict = predict.data.cpu().numpy()
# real = test.data.cpu().numpy()
#
# np.save("predict.npy", predict)
# np.save("real.npy", real)
#
#
# p1, = plt.plot(predict[:, 100][2000:2300], 'blue')
# p2, = plt.plot(real[:, 100][2000:2300], 'red')
# l1 = plt.legend([p2, p1], ["real", "predict"], loc='upper right')
# plt.xlabel('time (h)')
# plt.ylabel('occupancy (%)')
# plt.show()
