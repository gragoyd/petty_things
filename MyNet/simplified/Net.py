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
        self.hidS = args.hidSkip
        self.Ck = args.CNN_kernel
        self.hw = args.highway_window
        self.conv1 = nn.Conv2d(1, self.hidC, kernel_size=(self.Ck, self.m))
        self.GRU1 = nn.GRU(self.hidC, self.hidR)
        self.dropout = nn.Dropout(p=args.dropout)
        self.conv2 = nn.Conv1d(self.hidR, self.hidR, self.P - self.Ck + 1)
        self.attn_combine = nn.Linear(self.hidR * 2, self.hidR)
        self.GRU2 = nn.GRU(self.hidR, self.hidR)
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
        c = x.view(batch_size, 1, self.P, self.m)
        c = F.relu(self.conv1(c))
        c = self.dropout(c)
        c = torch.squeeze(c, 3)
        r = c.permute(2, 0, 1).contiguous()
        output, hidden = self.GRU1(r)
        hidden = torch.squeeze(hidden, 0)
        output = self.dropout(output)
        output = output.permute(1, 2, 0)
        output = self.conv2(output)
        output = torch.squeeze(output, 2)
        self.attn = nn.Linear(self.hidR * 2, batch_size).cuda()
        attn_weights = F.softmax(
            self.attn(torch.cat((output, hidden), 1)), dim=1)
        attn_applied = torch.mm(attn_weights, output)
        out = torch.cat((output, attn_applied), 1)
        out = F.relu(self.attn_combine(out))
        out = torch.unsqueeze(out, 0)
        hidden = torch.unsqueeze(hidden, 0)
        out, hidden = self.GRU2(out, hidden)
        hidden = torch.squeeze(hidden, 0)
        r = F.log_softmax(hidden, dim=1)
        res = self.linear1(r)

        if self.hw > 0:
            z = x[:, -self.hw:, :]
            z = z.permute(0, 2, 1).contiguous().view(-1, self.hw)
            z = self.highway(z)
            z = z.view(-1, self.m)
            res = res + z

        if self.output:
            res = self.output(res)

        return res
