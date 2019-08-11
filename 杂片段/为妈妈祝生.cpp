#include<stdio.h>
#include<stdlib.h> 
#include<string.h>
#include<time.h>
int main()
{char a[10]={"傻东阳"};
 char b[10];
 printf("请输入口令后按回车键：\n");
 gets(b);
 while(strcmp(a,b)!=0)
 {printf("口令错误，请重新输入：\n");
  scanf("%s",b);}
 struct tm *newtime;     //开始读取时间 
 char tmpbuf[50]; 
    time_t localTime1; 
    time( &localTime1 ); 
    newtime=localtime(&localTime1); 
	strftime( tmpbuf,50, "现在时间：%Y-%m-%d  %H:%M:%S\n", newtime);
    printf(tmpbuf);
	printf("\n这是我为你送上祝福的时刻\n");
	system("pause");
 for(int i=1;i<=400;i++)
   printf("%-3d妈妈生日快乐\n",i);
   printf("\n\n\n\n                   傻妈妈40岁啦！给你说400句生日快乐！！！\n");
   system("pause");
   return 0;
} 

