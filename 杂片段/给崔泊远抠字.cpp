
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main()
{
	ifstream filein;
	ofstream fileout[8];
	int i;
	string temp;
	filein.open("C:\\Users\\PC\\Desktop\\source.txt");
	if (!filein.is_open()) {
		cout << "can't open the file, check again!\n";
		exit(EXIT_FAILURE);
	}
	fileout[0].open("C:\\Users\\PC\\Desktop\\gouzei\\result_0.txt");
	fileout[1].open("C:\\Users\\PC\\Desktop\\gouzei\\result_1.txt");
	fileout[2].open("C:\\Users\\PC\\Desktop\\gouzei\\result_2.txt");
	fileout[3].open("C:\\Users\\PC\\Desktop\\gouzei\\result_3.txt");
	fileout[4].open("C:\\Users\\PC\\Desktop\\gouzei\\result_4.txt");
	fileout[5].open("C:\\Users\\PC\\Desktop\\gouzei\\result_5.txt");
	fileout[6].open("C:\\Users\\PC\\Desktop\\gouzei\\result_6.txt");
	fileout[7].open("C:\\Users\\PC\\Desktop\\gouzei\\result_7.txt");

	int found_profile, found_channel, found_radiance, found_temperature;
	do {												//滤掉开头
		getline(filein, temp);
		found_profile = temp.find("PROFILE INDEX:");
		found_channel = temp.find("CHANNEL INDEX:");
	} while (found_profile == string::npos || found_channel == string::npos);

	int filenum = 0;
	while (!filein.eof()) {			//文件未完
		do {						//循环直到找到 Radiance
			getline(filein, temp);
			found_radiance = temp.find("   Radiance");
		} while (found_radiance == string::npos || temp[found_radiance + 15] != ' ');
		for (i = found_radiance + 12; temp[i] == ' ' || temp[i] == ':'; ++i)		//滤掉空格和冒号
			;
		fileout[filenum++] << temp.substr(i) << endl;			//截取数据输出到文件
		//下一行紧接着就是所需数据，无需查找
		getline(filein, temp);
		found_temperature = temp.find("Temperature");
		for (i = found_temperature + 12; temp[i] == ' ' || temp[i] == ':'; ++i)		//滤掉空格和冒号
			;
		fileout[filenum++] << temp.substr(i) << endl;
		if (filenum == 8)
			filenum = 0;
	}

	filein.close();
	for (i = 0; i < 8; ++i) {
		fileout[i] << "done!\n";
		fileout[i].close();
	}
	cout << "done!\n";
	return 0;
}