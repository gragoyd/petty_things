#include<stdio.h>
int main()
{char c;
 int a=0;
 while((c=getchar())!='\n')          //ֱ���������ַ����Զ�ѭ����ȡÿһ���ַ� 
  {a++;
  printf("%d. %c\n",a,c);} 
 return 0;} 
 

#include<stdio.h>                //��ȡϵͳʱ�䲢��� 
#include<time.h>
#include<stdlib.h>
int main()
{struct tm *newtime; 
    char tmpbuf[50]; 
    time_t localTime1; 
    time( &localTime1 ); 
    newtime=localtime(&localTime1); 
    strftime( tmpbuf,50, "����ʱ�䣺%Y-%m-%d  %H:%M:%S\n", newtime); 
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
		printf("%d\n",rand()%x);			//����0~x-1�������� 
		printf("%d\n",rand()%x-y);			//����-y~x-y-1�������� 
	}
		
	return 0;
}

/* 
system("SCHTASKS");   ��ʾ����ƻ��б� 

    �ƻ��ػ����
shutdown -i        ��ʾͼ���û�����
shutdown -a        ȡ���ƻ�
shutdown -r        30s������
shutdown -f        ǿ�йر��������� 
shutdown -l        ע����ǰ�û� 
shutdown -s -t xx  ��xxs��ػ� */
