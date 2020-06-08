import torch
import torch.nn as nn
import torch.nn.functional as F


class Model(nn.Module):
    def __init__(self, args, data):
        super(Model, self).__init__()
        self.use_cuda = args.cuda
        self.P = args.window
        self.m = data.m
        self.hidR = args.hidRNN
        self.hidC = args.hidCNN
        self.hidS = args.hidSkip       # ########################################
        self.Ck = args.CNN_kernel
        self.skip = args.skip       # #######################################
        self.pt = (self.P - self.Ck) // self.skip
        self.hw = args.highway_window
        # self.hidC 为输出深度（输出第二维为self.hidC）
        self.conv1 = nn.Conv2d(1, self.hidC, kernel_size=(self.Ck, self.m))
        # 竟然可以不写层数
        self.GRU1 = nn.GRU(self.hidC, self.hidR)
        self.dropout = nn.Dropout(p=args.dropout)       # 默认是会 dropout 的

        # 以下是自己添加的
        self.conv2 = nn.Conv1d(self.hidR, self.hidR, self.P - self.Ck + 1)
        # self.attn() 在 forward() 中定义，因线性层第二个参数需依据 x.size()来定
        self.attn_combine = nn.Linear(self.hidR * 2, self.hidR)
        self.GRU2 = nn.GRU(self.hidR, self.hidR)
        # 以上是自己添加的

        if self.skip > 0:
            self.GRUskip = nn.GRU(self.hidC, self.hidS)
            self.linear1 = nn.Linear(self.hidR + self.skip * self.hidS, self.m)
        else:
            self.linear1 = nn.Linear(self.hidR, self.m)

        if self.hw > 0:
            self.highway = nn.Linear(self.hw, 1)

        if args.output_fun == 'sigmoid':
            self.output = torch.sigmoid
        elif args.output_fun == 'tanh':
            self.output = torch.tanh
        else:
            self.output = None

    def forward(self, x):
        batch_size = x.size(0)

        # CNN:拓展维度后进行二维卷积，relu和dropout后再降维
        # batch_size 处一般是 args.batch_size, 除非集的时刻数不是 args.batch_size 的整数倍
        c = x.view(batch_size, 1, self.P, self.m)
        # 此处输出维度为 [batch_size, self.hidC, self.P - self.Ck + 1, 1]
        c = F.relu(self.conv1(c))
        c = self.dropout(c)
        # 去掉最后为 1 的维度变为 [batch_size, self.hidC, self.P - self.Ck + 1]
        c = torch.squeeze(c, 3)

        # 原始RNN——————————————————————————运行时注释
        # r = c.permute(2, 0, 1).contiguous()     # r.size([self.P - self.Ck + 1, batch_size, self.hidC])
        # _, r = self.GRU1(r)                     # gru 最后隐藏层状态 r.size([1, batch_size, self.hidR])
        # r = self.dropout(torch.squeeze(r, 0))   # r.size([batch_size, self.hidR])

        # RNN-Attention：设 c 是已 embedded 了的
        # r.size([self.P - self.Ck + 1, batch_size, self.hidC])
        r = c.permute(2, 0, 1).contiguous()
        # hidden.size([1, batch_size, self.hidR])   相当于编码
        output, hidden = self.GRU1(r)
        # hidden.size([batch_size, self.hidR])
        hidden = torch.squeeze(hidden, 0)
        # output.size: self.P - self.Ck + 1, batch_size, self.hidR
        output = self.dropout(output)
        # 对 output 卷积降维，类似于 conv1
        # output.size: batch_size, self.hidR, self.P - self.Ck + 1
        output = output.permute(1, 2, 0)
        output = self.conv2(output)
        # 此时 output.size([batch_size, self.hidR])
        output = torch.squeeze(output, 2)
        # 但不知道是对output卷积效果好，还是直接强制output.view(-1, self.hidR)效果好。因为不知道卷积搞错了没
        self.attn = nn.Linear(self.hidR * 2, batch_size).cuda()
        # print(output.size(), hidden.size())
        # cat()左右拼接,attn_weights.size([batch_size, batch_size])
        attn_weights = F.softmax(
            self.attn(torch.cat((output, hidden), 1)), dim=1)
        # attn_applied.size([batch_size, batch_size])
        attn_applied = torch.mm(attn_weights, output)
        # out.size:batch_size, self.hidR * 2
        out = torch.cat((output, attn_applied), 1)
        # out.size:batch_size, self.hidR
        out = F.relu(self.attn_combine(out))
        # out.size:1, batch_size, self.hidR
        out = torch.unsqueeze(out, 0)
        # hidden.size:1, batch_size, self.hidR
        hidden = torch.unsqueeze(hidden, 0)
        out, hidden = self.GRU2(out, hidden)
        # out = torch.squeeze(out, 0)                     # out.size:batch_size, self.hidR
        # hidden.size: batch_size, self.hidR
        hidden = torch.squeeze(hidden, 0)
        # ################## 不知道该用 out 还是 hidden。。。。。。也不知道最后该不该 softmax
        # 此时：r.size([batch_size, self.hidR])
        r = F.log_softmax(hidden, dim=1)

        # skip-rnn      默认不用
        if self.skip > 0:
            s = c[:, :, -self.pt * self.skip:].contiguous()
            # 原为：s = s.view(batch_size, self.hidC, self.pt, self.skip)
            s = s.view(batch_size, self.hidC, self.pt, -1)
            s = s.permute(2, 0, 3, 1).contiguous()
            # 原为：s = s.view(self.pt, batch_size * self.skip, self.hidC)
            s = s.view(self.pt, -1, self.hidC)
            _, s = self.GRUskip(s)
            # 原为：s = s.view(batch_size, self.skip * self.hidS)
            s = s.view(batch_size, -1)
            s = self.dropout(s)
            r = torch.cat((r, s), 1)

        res = self.linear1(r)       # ###############################注意此r非彼r

        # highway   对应数据由线性旁路经自回归组件与全连接层结果相加
        if self.hw > 0:
            z = x[:, -self.hw:, :]
            z = z.permute(0, 2, 1).contiguous().view(-1, self.hw)
            z = self.highway(z)
            z = z.view(-1, self.m)
            res = res + z

        if self.output:
            res = self.output(res)

        return res
