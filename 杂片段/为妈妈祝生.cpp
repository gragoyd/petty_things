#include<stdio.h>
#include<stdlib.h> 
#include<string.h>
#include<time.h>
int main()
{char a[10]={"ɵ����"};
 char b[10];
 printf("���������󰴻س�����\n");
 gets(b);
 while(strcmp(a,b)!=0)
 {printf("����������������룺\n");
  scanf("%s",b);}
 struct tm *newtime;     //��ʼ��ȡʱ�� 
 char tmpbuf[50]; 
    time_t localTime1; 
    time( &localTime1 ); 
    newtime=localtime(&localTime1); 
	strftime( tmpbuf,50, "����ʱ�䣺%Y-%m-%d  %H:%M:%S\n", newtime);
    printf(tmpbuf);
	printf("\n������Ϊ������ף����ʱ��\n");
	system("pause");
 for(int i=1;i<=400;i++)
   printf("%-3d�������տ���\n",i);
   printf("\n\n\n\n                   ɵ����40����������˵400�����տ��֣�����\n");
   system("pause");
   return 0;
} 

