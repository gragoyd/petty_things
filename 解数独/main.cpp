#include "head.h"

Box board[9][9], substitute[9][9];			// 原始数据格子与猜数时临时数据格子
int remain[10] = { 81,9,9,9,9,9,9,9,9,9 };	// 第 0 个表总共剩余，其余索引表示对应的数还有多少个未填
int example[81] = {			// 测试用例
	0,6,0,0,8,0,0,0,0,
	1,8,0,4,0,0,0,0,7,
	0,0,0,5,0,0,0,2,9,
	0,0,6,0,0,0,0,5,0,
	0,5,3,0,6,9,4,0,2,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,8,0,0,6,
	0,0,9,2,7,0,0,0,0,
	0,0,0,0,0,0,0,4,0
};

int main() {

	for (int i = 0; i < 9; ++i)				// 输入初始数据
		for (int j = 0; j < 9; ++j) {
			// cin >> board[i][j].value;			控制台输入数据时启用此行并注释下一行
			board[i][j].value = example[9 * i + j];
			board[i][j].row = i;
			board[i][j].col = j;
			if (board[i][j].value) {
				--remain[board[i][j].value];
				--remain[0];
			}
		}
	cout << "Data recive completed:\n";
	print_board();

	for (int i = 0; i < 9; ++i)				// 给每个空格标记可能填入的数字（做笔记）
		for (int j = 0; j < 9; ++j)
			if (!board[i][j].value) {		// 若是空格
				for (int t = 1; t < 10; ++t)
					if (check_row(i, t) && check_col(j, t) && check_nin(i / 3, j / 3, t))
						board[i][j].add_notes(t);
				if (1 == board[i][j].note_count)			// 清理一波笔记（若笔记数为一，则填数成功）
					board[i][j].box_success();
			}

	bool bowman = true;		// 指示是否用法三
	while (remain[0]) {		// 循环填数
		for (int i = 1; i < 10; ++i)
			if (method1(i))
				bowman = false;

		for (int i = 0; i < 9; ++i)
			for (int j = 0; j < 9; ++j)
				if (2 == board[i][j].note_count || 3 == board[i][j].note_count)
					if (dominance(i, j))
						bowman = false;
		if (method2())
			bowman = false;

		if (bowman)			// 其它方法都失效，只能用猜的了
			for (int k = 2; k < 10; ++k)		// k 表空格笔记数，优先试笔记数少的，遍历笔记猜中的概率大
				for (int i = 0; i < 9; ++i)
					for (int j = 0; j < 9; ++j)
						if (k == board[i][j].note_count)
							method3(board[i][j]);
		bowman = true;
	}
	cout << "\n\nanswer get:\n";
	print_board();
	return 0;
}