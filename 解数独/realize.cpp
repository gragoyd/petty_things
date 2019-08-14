#include "head.h"

extern Box board[9][9], substitute[9][9];
extern int remain[10];

bool Box::del_notes(const int n, Box (* sub)[9]) {
	if (notes[n]) {
		notes[n] = false;
		--note_count;
		if (1 == note_count)
			box_success(sub);
		return true;
	}
	else
		return false;
}

void Box::operator=(const Box & b) {
	row = b.row;
	col = b.col;
	value = b.value;
	note_count = b.note_count;
	for (int i = 0; i < 10; ++i)
		notes[i] = b.notes[i];
}

void Box::box_success(Box (* sub)[9]) {
	note_count = 0;
	for (int i = 1; i < 10; ++i)
		if (notes[i]) {
			value = i;
			break;
		}
	if (sub) {
		notes[0] = true;		// 表明此格为测试性填数
		update_note(row, col, sub);
	}
	else {
		cout << "(" << row + 1 << ", " << col + 1 << ") pad " << value << endl;
		--remain[value];
		--remain[0];
		update_note(row, col, board);
	}
}

int notes_equal(Box &a, Box &b) {
	int count = 0;
	for (int i = 1; i < 10; ++i)
		if (a.notes[i] && b.notes[i])
			++count;
	return count;
}

void print_board() {
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			cout << board[i][j].value << ' ';
		}
		cout << endl;
	}
	cout << endl;
}

void success(const int r, const int c, const int n, Box (* sub)[9]) {
	if (sub) {
		sub[r][c].note_count = 0;
		sub[r][c].value = n;
		sub[r][c].notes[0] = true;		// 表明此格为测试性填数
		update_note(r, c, sub);
	}
	else {
		board[r][c].note_count = 0;
		board[r][c].value = n;
		cout << "(" << r + 1 << ", " << c + 1 << ") pad " << n << endl;
		--remain[n];
		--remain[0];
		update_note(r, c, board);
	}

}

bool check_row(const int r, const int num) {
	for (int i = 0; i < 9; ++i)						// 检查 r 行有无 n 存在
		if (num == board[r][i].value)
			return false;
	return true;
}

bool check_col(const int c, const int num) {
	for (int i = 0; i < 9; ++i)						//检查 c 列有无 n 存在
		if (num == board[i][c].value)
			return false;
	return true;
}

bool check_nin(const int br, const int bc, const int num) {
	int p = br * 3, q = bc * 3;
	for (int i = p; i < p + 3; ++i)					// 检查 (r, c) 格所在宫中 n 是否存在
		for (int j = q; j < q + 3; ++j)
			if (num == board[i][j].value)
				return false;
	return true;
}

bool method1(const int num) {		// 返回值指示对应的数是否成功又填入了一个
	int r, c, n, k, result = 0;
	Box * p = NULL;
	// 遍历排查行
	for (r = 0; r < 9; ++r)
		if (check_row(r, num)) {		// 行中可能能填入此数
			p = NULL;
			for (c = 0; c < 9; ++c)
				if (!board[r][c].value && board[r][c].notes[num])
					if (p) {			// 有多个格可填入该数，此行排查失败
						p = NULL;
						break;
					}
					else
						p = &board[r][c];
			if (p) {				// 若到这 p 非空，则成功找到此行内唯一可填该数的格子
				success(p->row, p->col, num);
				method1(num);		// 新填入的这个可能还导致其它地方也能由此法填入该数
				result = 1;
			}
		}
	// 遍历排查列
	for (c = 0; c < 9; ++c)
		if (check_col(c, num)) {
			p = NULL;
			for (r = 0; r < 9; ++r)
				if (!board[r][c].value && board[r][c].notes[num])
					if (p) {
						p = NULL;
						break;
					}
					else
						p = &board[r][c];
			if (p) {
				success(p->row, p->col, num);
				method1(num);
				result = 1;
			}
		}
	// 遍历排查宫
	for (int br = 0; br < 3; ++br)
		for (int bc = 0; bc < 3; ++bc)
			if (check_nin(br, bc, num)) {		// 宫中可能能填入该数
				p = NULL;
				n = br * 3, k = bc * 3;
				for (r = n; r < n + 3; ++r)
					for (c = k; c < k + 3; ++c)
						if (!board[r][c].value && board[r][c].notes[num])
							if (p) {		// 有多个格可填入该数，此宫排查失败
								p = NULL;
								goto NEXT_NINE;
							}
							else
								p = &board[r][c];
			NEXT_NINE:
				if (p) {
					success(p->row, p->col, num);
					method1(num);
					result = 1;
				}
			}
	return result;
}

void update_note(const int r, const int c, Box (* choice)[9]) {

	int key = choice[r][c].value;
	for (int t = 0; t < 9; ++t)
		if (!choice[r][t].value)
			choice[r][t].del_notes(key, choice == board ? NULL : choice);
	for (int t = 0; t < 9; ++t)
		if (!choice[t][c].value)
			choice[t][c].del_notes(key, choice == board ? NULL : choice);
	int m, k, p = r / 3 * 3, q = c / 3 * 3;
	for (m = p; m < p + 3; ++m)
		for (k = q; k < q + 3; ++k)
			if (!choice[m][k].value)
				choice[m][k].del_notes(key, choice == board ? NULL : choice);
}

bool dominance_del(Box * sourse, const int key, const char how, Box * skip1, Box * skip2) {		// 返回值为是否实际删除了相关格子中的显性笔记数

	int count = 0, r = sourse->row, c = sourse->col, rs1 =skip1->row, cs1 = skip1->col, rs2 = 9, cs2 = 9;
	if (skip2) {
		rs2 = skip2->row;
		cs2 = skip2->col;
	}
	switch (how) {
	case 'r':
		for (int i = 0; i < 9; ++i)
			if (!board[r][i].value && (i != c) && (i != cs1) && (i != cs2) && board[r][i].del_notes(key))
				++count;
		break;
	case 'c':
		for (int i = 0; i < 9; ++i)
			if (!board[i][c].value && (i != r) && (i != rs1) && (i != rs2) && board[i][c].del_notes(key))
				++count;
		break;
	case 'n':
		int m, k, p = r / 3 * 3, q = c / 3 * 3;
		for (m = p; m < p + 3; ++m)
			for (k = q; k < q + 3; ++k)
				if (!board[m][k].value && ((m != r) || (k !=c)) && ((m != rs1) || (k != cs1)) && ((m != rs2) || (k != cs2)) && board[m][k].del_notes(key))
					++count;
	}
	return count;
}

bool dominance_array(Box ** a, const int num, const char how) {		// a 数组中混杂着潜在的显性双格与三格，how 参数用来指示显性双格的制约关系: r, c, n

	int i, j, k, t, n1, n2, n3, result = 0;
	// 先排查显性双格
	for (k = 0; k < num - 1; ++k)
		if (2 == a[k]->note_count)
			for (t = k + 1; t < num; ++t)
				if (2 == a[t]->note_count && 2 == notes_equal(*a[k], *a[t])) {	// 确认是显性双格
					n1 = n2 = 0;				// 可能组中有不止一对显性双格
					for (i = 1; i < 10; ++i)	// 先找到俩显性笔记数
						if (a[k]->notes[i])
							if (n1) {			// 此时已找齐俩笔记
								n2 = i;
								break;
							}
							else
								n1 = i;
					// 若实际删掉了相关的笔记，则再以俩格子为中心递归自调再检查（返回值不细化到具体是哪个格子）
					if (dominance_del(a[k], n1, how, a[t]) || dominance_del(a[k], n2, how, a[t])) {
						dominance(a[k]->row, a[k]->col);
						dominance(a[t]->row, a[t]->col);
						result = 1;
					}
				}
	// 排查显性三格（三格的笔记数都为二或三，且三格中所有笔记都为特定的三个数）
	for (i = 0; i < num - 2; ++i)
		for (j = i + 1; j < num - 1; ++j)
			for (k = j + 1; k < num; ++k) {			// 若三个格子笔记恰出现三种数，则为显性三格
				n1 = n2 = n3 = 0;
				for (t = 1; t < 10; ++t) {		// 检查显性笔记数
					if (a[i]->notes[t] || a[j]->notes[t] || a[k]->notes[t]) {
						if (n1)
							if (n2)
								if (n3) {		// 已出现超过三种数，说明不是显性三格（实际上仅需两个格子的笔记就会出现至少三种数）
									n1 = 0;		// 置零表示非显性三格
									break;
								}
								else
									n3 = t;
							else
								n2 = t;
						else
							n1 = t;
					}
				}
				// n1 非零表为显性三格。若实际删掉了相关的笔记，则再以俩格子为中心递归自调再检查（返回值不细化到具体是哪个格子）
				if (n1 && (dominance_del(a[i], n1, how, a[j], a[k]) || dominance_del(a[i], n2, how, a[j], a[k]) || dominance_del(a[i], n3, how, a[j], a[k]) ||
							dominance_del(a[j], n1, how, a[i], a[k]) || dominance_del(a[j], n2, how, a[i], a[k]) || dominance_del(a[j], n3, how, a[i], a[k]) ||
							dominance_del(a[k], n1, how, a[j], a[i]) || dominance_del(a[k], n2, how, a[j], a[i]) || dominance_del(a[k], n3, how, a[j], a[i]))) {
					dominance(a[i]->row, a[i]->col);
					dominance(a[j]->row, a[j]->col);
					dominance(a[k]->row, a[k]->col);
					result = 1;
				}
			}
	return result;
}

bool dominance(const int r, const int c) {

	int i, j, count, result = 0;		// n1、n2 分别记录数组中实际有多少个待处理的潜在显性格
	Box *candidate[9] = { NULL };		// 用两行将潜在的双格和三格分开存放

	count = 0;							// 检查第 r 行
	for (i = 0; i < 9; ++i)
		if (2 == board[r][i].note_count || 3 == board[r][i].note_count)		// 笔记数非零即可保证格子为空
			candidate[count++] = &board[r][i];
	if ((count > 1) && dominance_array(candidate, count, 'r'))			// 至少有俩潜在的才可能是显性格子
		result = 1;

	count = 0;							// 检查第 c 列
	for (i = 0; i < 9; ++i)
		if (2 == board[i][c].note_count || 3 == board[i][c].note_count)
			candidate[count++] = &board[i][c];
	if ((count > 1) && dominance_array(candidate, count, 'c'))
		result = 1;

	count = 0;							// 检查所在宫
	int p = r / 3 * 3, q = c / 3 * 3;
	for (i = p; i < p + 3; ++i)
		for (j = q; j < q + 3; ++j)
			if (2 == board[i][j].note_count || 3 == board[i][j].note_count)
				candidate[count++] = &board[i][j];
	if ((count > 1) && dominance_array(candidate, count, 'n'))
		result = 1;

	return result;
}

bool method2() {
	int i, j, n, m, k, t;	// count 计每宫删数时有多少暂存的格子
	bool p[3], result = false;		// 用于指示该行/列分属不同三宫的三段格子中各段格子笔记中有无含特定的数
	for (i = 0; i < 9; ++i)
		for (n = 1; n < 10; ++n) {
			// 排查行
			t = p[0] = p[1] = p[2] = false;
			for (j = 0; j < 9; ++j)
				if (!board[i][j].value && board[i][j].notes[n])
					p[j / 3] = true;
			if (p[0] && !p[1] && !p[2])
				t = 2;
			else if (!p[0] && p[1] && !p[2])
				t = 5;
			else if (!p[0] && !p[1] && p[2])
				t = 8;
			if (t)			// 宫线删数情形成立（笔记含特定数的三个格在同一宫内，删除该宫其它两行格子笔记中的该数）
				for (m = i / 3 * 3; m < i / 3 * 3 + 3; ++m)
					if (m != i)
						for (k = t; k > t - 3; --k)
							if (!board[m][k].value && board[m][k].del_notes(n))
								result = true;
			// 排查列
			t = p[0] = p[1] = p[2] = false;
			for (j = 0; j < 9; ++j)
				if (!board[j][i].value && board[j][i].notes[n])
					p[j / 3] = true;
			if (p[0] && !p[1] && !p[2])
				t = 2;
			else if (!p[0] && p[1] && !p[2])
				t = 5;
			else if (!p[0] && !p[1] && p[2])
				t = 8;
			if (t)			// 宫线删数情形成立（笔记含特定数的三个格在同一宫内，删除该宫其它两行格子笔记中的该数）
				for (m = i / 3 * 3; m < i / 3 * 3 + 3; ++m)
					if (m != i)
						for (k = t; k > t - 3; --k)
							if (!board[k][m].value && board[k][m].del_notes(n))
								result = true;
		}
	return result;
}

bool method3(Box & box) {
	int i, j, N;	// N 存储 box 中正在测试的笔记
	for (N = 1; N < 10; ++N)		// 依次选取笔记进行测试
		if (box.notes[N]) {
			for (i = 0; i < 9; ++i)			// 复制以保护原始数据
				for (j = 0; j < 9; ++j)
					substitute[i][j] = board[i][j];

			success(box.row, box.col, N, substitute);		// 自动嵌套对相关格子进行填数

			for (i = 0; i < 9; ++i)			// 完成相关格子的推断填数之后排查看有没有冲突
				for (j = 0; j < 9; ++j)
					if (substitute[i][j].notes[0] && strife(substitute[i][j]))
						goto THIS_NOTE_FAIL;

			cout << "\nBowman padding begin:\n";
			for (i = 0; i < 9; ++i)			// 能到这说明猜测成功，覆盖原始数据
				for (j = 0; j < 9; ++j) {
					if (substitute[i][j].notes[0]) {
						cout << "(" << i + 1 << ", " << j + 1 << ") pad " << substitute[i][j].value << endl;
						substitute[i][j].notes[0] = false;	// 先把标志位复原，以免之后再次调用 method3 时拷贝数据导致标志位错误
						--remain[0];
						--remain[substitute[i][j].value];
					}
					board[i][j] = substitute[i][j];
				}
			cout << "\nBowman padding over.\n\n";
			return true;

		THIS_NOTE_FAIL:
			;
		}
	return false;		// 一直没猜测成功即为失败
}

bool strife(Box & box) {		// 填数冲突返回 true
	for (int i = 0; i < 9; ++i)
		if (((substitute[box.row][i].value == box.value) && (i != box.col)) ||
			((substitute[i][box.col].value == box.value) && (i != box.row)))
			return true;
	for (int i = box.row / 3 * 3; i < box.row / 3 * 3 + 3; ++i)
		for (int j = box.col / 3 * 3; j < box.col / 3 * 3 + 3; ++j) {
			if ((substitute[i][j].value == box.value) && ((i != box.row) || (j != box.col)))
				return true;
		}
	return false;
}