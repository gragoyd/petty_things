#include<stdio.h>
#include<stdlib.h>
double m;
int main()
{long double hanoi(int,char,char,char);
 int i,flag=1;
 char d;
 while(flag) 
 {printf("输入汉诺塔的层数后按回车以获得解决方案:     ");
  scanf("%d",&i);
  while(i<1||i%1!=0)
  {printf("层数有误，请重新输入:");
   scanf("%d",&i);
   }  
  m=0;
  printf("转移%d层塔的步骤是(假定是将塔从A移动到C):\n", i);
  hanoi(i,'A','B','C');
  printf("      总共需要移动%lf次\n\n",m);
  printf("---------------完毕---------------             "); 
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
