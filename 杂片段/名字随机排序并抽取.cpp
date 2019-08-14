#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

void f(ifstream &filein, ofstream &fileout, int &n);

int main()
{
	ifstream filein[4];
	filein[0].open("C:\\Users\\PC\\Desktop\\names\\source_man_single.txt");
	filein[1].open("C:\\Users\\PC\\Desktop\\names\\source_men_doubles.txt");
	filein[2].open("C:\\Users\\PC\\Desktop\\names\\source_mixed_doubles.txt");
	filein[3].open("C:\\Users\\PC\\Desktop\\names\\source_woman_single.txt");
	ofstream fileout[4];
	fileout[0].open("C:\\Users\\PC\\Desktop\\names\\result_man_single.txt");
	fileout[1].open("C:\\Users\\PC\\Desktop\\names\\result_men_doubles.txt");
	fileout[2].open("C:\\Users\\PC\\Desktop\\names\\result_mixed_doubles.txt");
	fileout[3].open("C:\\Users\\PC\\Desktop\\names\\result_woman_single.txt");
	int i, n[4];
	cout << "依次输入男单、男双、混双、女单项目需要抽取的人数：\t";
	for (i = 0; i < 4; ++i) {
		if (!filein[i].is_open()) {
			cout << "the #" << i + 1 << "file open failed!\n";
			break;
		}
		cin >> n[i];
		f(filein[i], fileout[i], n[i]);
		filein[i].close();
		fileout[i].close();
	}
}

void f(ifstream &filein, ofstream &fileout, int &n) {
	int count = 0, i;
	string temp;						//确定名字个数
	for (; !filein.eof(); ++count)
		getline(filein, temp);
	filein.seekg(0, ios::beg);
	string *pname = new string[count];	//读取名字
	for (i = 0; i < count; ++i)
		getline(filein, pname[i]);
	int *pnum = new int[count];			//准备生成随机序号
	int tempnum;
	for (i = 0; i < count; ++i)
		pnum[i] = i;
	for (i = count - 1; i >= 1; --i) {
		srand(time(0));
		swap(pnum[i], pnum[rand() % i]);
	}									//准备输出
	fileout << "\t\t名单已乱序\n";
	fileout << "\t\t这是抽出来的" << n << "位/组选手\n";
	for (i = 0; i < n; ++i)
		fileout << pname[pnum[i]] << endl;
	fileout << "\t\t这是剩余的" << count - n << "位/组选手\n";
	for (; i < count; ++i)
		fileout << pname[pnum[i]] << endl;
}