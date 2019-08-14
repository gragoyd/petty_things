/*
所使用的数独查找方法：
	主要：
		对每个空格可能出现的数都记上笔记，并随着之后的查找运算除去不可能的笔记，帮助找出最终的数。
	method：
		1、对已填入不低于两次的数，检查该数所在行/列横纵交汇的宫的空格，看是否已经排除出该宫中唯一可能填入该数的格子。
			注：考虑数据结构，实现采用遍历九个宫检查各宫内是否仅有唯一小格笔记包含该数。且将此法拓展至检查行和列。
		2、若某行/列中某数可能填入的空格都在同一宫中，则该数仅可填入该宫与该行/列重叠的三个格中，借此可从该宫其它六格笔记中删除该数。
		3、Bowman 试数法：任找一空格（尽量取仅有俩笔记的）从笔记中选取一数填入并依此推断相关格子填数，排查所有因此填入数的格子：
								若查出违反规则，则初始选取格子不可能为之前填入的数；
								若填数都合理，则猜测成功。
	辅助：
		1、显性 n 格：若存在相互制约关系的 n 个格子恰好都只可能出现 n 个数，则其它与这 n 个格子相关联的格子都可去除那 n 个数出现的可能性笔记。
			例：
				某行两个格都只可能为 2 / 7，则该行其它待填的格子都可去除 2、7 的可能性笔记。
				某相制约的三个格子，分别可能为 1/8（或者 1/2/8 也行）、1/2/8、1/2/8，则这三个数只能分别填入这三个格中，此为显性三格，删除相制约其它格子中此三个数的笔记。
			注：考虑 n 实际取值出现的概率，仅实现 n = 2、3，即只考虑显性双/三格。
*/

#pragma once
#include <iostream>
using namespace std;

class Box {
public:
	int row;		// 格子行坐标
	int col;		// 格子列坐标
	int value = 0;
	int note_count = 0;		// 此格子笔记总数
	bool notes[10] = { false };		// 指示相应索引的数可不可能（第 0 个格子用于在 Bowman 法时指示是否为测试性填数）

	void add_notes(const int n) { notes[n] = true; ++note_count; }		// 用于初始添加笔记
	bool del_notes(const int n, Box (* sub)[9] = NULL);	// 返回值为是否真的删了相应的数（有可能笔记中本就没那个数）
	void operator=(const Box & b);
	void box_success(Box (* sub)[9] = NULL);		// 类方法版本的填数成功函数（仅限在笔记数为一时调用）
	friend int notes_equal(Box &a, Box &b);
};

// 比较两个格子的笔记相同的数的个数
int notes_equal(Box &a, Box &b);
// 输出所有格子
void print_board();
// 成功找到 (r, c) 填的数 n
void success(const int r, const int c, const int n, Box (* sub)[9] = NULL);
// 对特定行检查特定的数能否填入
bool check_row(const int r, const int num);
// 对特定列检查特定的数能否填入
bool check_col(const int c, const int num);
// 对特定宫检查特定的数能否填入
bool check_nin(const int br, const int bc, const int num);
// 法一
bool method1(const int num);
// 当一个格子填完后同步与其关联的格子的笔记，或者是未填但为显性双格时删除与之相关格子中的显性笔记数
void update_note(const int r, const int c, Box (* choice)[9]);
// 删除与显性格相关格子中的显性笔记数
bool dominance_del(Box * sourse, const int key, const char how, Box * skip1, Box * skip2 = NULL);
// 排查某数组中的显性格，是 dominance() 的组件（返回值指示实际上干事了没）
bool dominance_array(Box ** a, const int num, const char how);
// 以 (r, c) 为中心排查显性格（返回值指示实际上干事了没）
bool dominance(const int r, const int c);
// 法二（返回值指示实际上干事了没）
bool method2();
// 法三（返回值指示猜数成功了没）
bool method3(Box & box);
// 检查与特定格子相关联的已填数的格子填入的数是否与规则有冲突（仅限 method3 中对 substitute 使用）
bool strife(Box & box);
