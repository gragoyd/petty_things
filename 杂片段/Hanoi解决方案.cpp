#include<stdio.h>
#include<stdlib.h>
double m;
int main()
{long double hanoi(int,char,char,char);
 int i,flag=1;
 char d;
 while(flag) 
 {printf("���뺺ŵ���Ĳ����󰴻س��Ի�ý������:     ");
  scanf("%d",&i);
  while(i<1||i%1!=0)
  {printf("������������������:");
   scanf("%d",&i);
   }  
  m=0;
  printf("ת��%d�����Ĳ�����(�ٶ��ǽ�����A�ƶ���C):\n", i);
  hanoi(i,'A','B','C');
  printf("      �ܹ���Ҫ�ƶ�%lf��\n\n",m);
  printf("---------------���---------------             "); 
  system("pause");
  printf("\n\n"); 
  }
 }


long double hanoi(int n,char a,char b,char c)
{if(n==1)
 {printf("%c-->%c\n",a,c);
  m++;
  }
 else
 {hanoi(n-1,a,c,b);
  printf("%c-->%c\n",a,c);
  m++;
  hanoi(n-1,b,a,c);
  }
 return m;
 }
