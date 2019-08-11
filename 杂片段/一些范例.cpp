#include<stdio.h>
int main()
{char c;
 int a=0;
 while((c=getchar())!='\n')          //直接输入多个字符，自动循环读取每一个字符 
  {a++;
  printf("%d. %c\n",a,c);} 
 return 0;} 
 

#include<stdio.h>                //读取系统时间并输出 
#include<time.h>
#include<stdlib.h>
int main()
{struct tm *newtime; 
    char tmpbuf[50]; 
    time_t localTime1; 
    time( &localTime1 ); 
    newtime=localtime(&localTime1); 
    strftime( tmpbuf,50, "现在时间：%Y-%m-%d  %H:%M:%S\n", newtime); 
    printf(tmpbuf); 
 return 0; 
} 

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(){
	int i;
	srand(time(0));
	for(i=0;i<20;i++){
		printf("%d\n",rand()%x);			//产生0~x-1间的随机数 
		printf("%d\n",rand()%x-y);			//产生-y~x-y-1间的随机数 
	}
		
	return 0;
}

/* 
system("SCHTASKS");   显示任务计划列表 

    计划关机语句
shutdown -i        显示图形用户界面
shutdown -a        取消计划
shutdown -r        30s后重启
shutdown -f        强行关闭所有任务 
shutdown -l        注销当前用户 
shutdown -s -t xx  在xxs后关机 */
