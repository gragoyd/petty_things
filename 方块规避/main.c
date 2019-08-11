#include "acllib.h"
#include<math.h>
#include<time.h>
#include<stdlib.h>

#define RAD(x) ((x)/360.0*2*3.1415926535)

#define Nzhangai 4			//障碍方块数量 
#define KUAN 700			//游戏界面宽度
#define GAO 700			//游戏界面高度 

char flag=0;
char Speedzhangai=5;			//障碍方块移动速度
char model;					//0-无敌模式，1-正常模式 

struct Zhangai{
	int x;
	int y;
	int kuan;
	int gao;
	int degree;
}zhangai[Nzhangai+1];

struct Zhangai * p = zhangai;

void paint(){
	int i;
	beginPaint();
	clearDevice();
	if(flag){
		setPenWidth(0);
		setBrushColor(RED);
		rectangle(p[0].x-p[0].kuan/2,p[0].y-p[0].gao/2,p[0].x+p[0].kuan/2,p[0].y+p[0].gao/2);
		setBrushColor(BLACK);
		for(i=1;i<Nzhangai+1;i++)
			rectangle(p[i].x,p[i].y,p[i].x+p[i].kuan,p[i].y+p[i].gao);	
		if(!model){
			setBrushColor(EMPTY);
			rectangle(0,0,50,25);
			setTextSize(20);
			paintText(3,2,"MENU");
		}	
	}
	else{
		setPenWidth(10);
		line(KUAN/2,0,KUAN/2,GAO);
		setBrushColor(RED);
		setTextSize(50);
		paintText(KUAN/2-250,70,"无敌模式");
		paintText(KUAN/2+50,70,"正常模式");
		
		setPenWidth(0);
		rectangle(KUAN/2-250,150,KUAN/2-50,200);
		paintText(KUAN/2-250,150,"level 1");
		rectangle(KUAN/2-250,250,KUAN/2-50,300);
		paintText(KUAN/2-250,250,"level 2");
		rectangle(KUAN/2-250,350,KUAN/2-50,400);
		paintText(KUAN/2-250,350,"level 3");
		rectangle(KUAN/2-250,450,KUAN/2-50,500);
		paintText(KUAN/2-250,450,"level 4");
		rectangle(KUAN/2-250,550,KUAN/2-50,600);
		paintText(KUAN/2-250,550,"level 5");
		
		rectangle(KUAN/2+50,150,KUAN/2+250,200);
		paintText(KUAN/2+50,150,"level 1");
		rectangle(KUAN/2+50,250,KUAN/2+250,300);
		paintText(KUAN/2+50,250,"level 2");
		rectangle(KUAN/2+50,350,KUAN/2+250,400);
		paintText(KUAN/2+50,350,"level 3");
		rectangle(KUAN/2+50,450,KUAN/2+250,500);
		paintText(KUAN/2+50,450,"level 4");
		rectangle(KUAN/2+50,550,KUAN/2+250,600);
		paintText(KUAN/2+50,550,"level 5");
				
	}
	endPaint();
}

char crash(){
	int i,re=0;
	for(i=1;i<Nzhangai+1;i++)
		if((p[i].x+p[i].kuan>=p[0].x-p[0].kuan/2)&&(p[i].x<=p[0].x+p[0].kuan/2)&&(p[i].y+p[i].gao>=p[0].y-p[0].gao/2)&&(p[i].y<=p[0].y+p[0].gao/2))
			re=1;
	return re;
}
 
void init(){
	srand(time(0));
	int i;
	p[1].x=50;
	p[1].y=50;
	p[2].x=KUAN-50;
	p[2].y=50;
	p[3].x=50;
	p[3].y=GAO-50;
	p[4].x=KUAN-50;
	p[4].y=GAO-50;
	for(i=1;i<Nzhangai+1;i++)
		p[i].degree=rand()%361;
	startTimer(0,10);
}

void hit(){
	int i;
	for(i=1;i<Nzhangai+1;i++){
		srand(time(0));
		if(p[i].x<=0)
			p[i].degree=rand()%91-45;
		if(p[i].x>=KUAN-p[i].kuan)
			p[i].degree=135+rand()%91;
		if(p[i].y<=0)
			p[i].degree=45+rand()%91;
		if(p[i].y>=GAO-p[i].gao)
			p[i].degree=-45-rand()%91;
	}
}

void move(){
	int i;
	for(i=1;i<Nzhangai+1;i++){
		p[i].x+=Speedzhangai*cos(RAD(p[i].degree));
		p[i].y+=Speedzhangai*sin(RAD(p[i].degree));
	}
}

void timer(int id){
	if(!id){
		paint();
		if(flag){
			hit();
			move();
			if(model&&crash()){
				flag=0;
				cancelTimer(0);
				paint();
			}							
		}	
	}
}

void mouseEvent(int x,int y,int key,int event){
	if(flag){
		if(event==MOUSEMOVE){
			p[0].x=x;
			p[0].y=y;
		}
		if(event==BUTTON_UP&&x>=0&&x<=50&&y>=0&&y<=25){
			flag=0;
			cancelTimer(0);
			paint();			
		}			
	}	
	else	
		if(!flag&&event==BUTTON_UP){
			p[0].x=x;
			p[0].y=y;
			if(x>=KUAN/2-250&&y>=150&&x<=KUAN/2-50&&y<=200){
				model=0;
				flag=1;
				init();
				Speedzhangai=5;
			}
			if(x>=KUAN/2-250&&y>=250&&x<=KUAN/2-50&&y<=300){
				model=0;
				flag=1;
				init();
				Speedzhangai=9;
			}
			if(x>=KUAN/2-250&&y>=350&&x<=KUAN/2-50&&y<=400){
				model=0;
				flag=1;
				init();
				Speedzhangai=13;
			}
			if(x>=KUAN/2-250&&y>=450&&x<=KUAN/2-50&&y<=500){
				model=0;
				flag=1;
				init();
				Speedzhangai=17;
			}
			if(x>=KUAN/2-250&&y>=550&&x<=KUAN/2-50&&y<=600){
				model=0;
				flag=1;
				init();
				Speedzhangai=21;
			}
			
			if(x>=KUAN/2+50&&y>=150&&x<=KUAN/2+250&&y<=200){
				model=1;
				flag=1;
				init();
				Speedzhangai=5;
			}
			if(x>=KUAN/2+50&&y>=250&&x<=KUAN/2+250&&y<=300){
				model=1;
				flag=1;
				init();
				Speedzhangai=9;
			}
			if(x>=KUAN/2+50&&y>=350&&x<=KUAN/2+250&&y<=400){
				model=1;
				flag=1;
				init();
				Speedzhangai=13;
			}
			if(x>=KUAN/2+50&&y>=450&&x<=KUAN/2+250&&y<=500){
				model=1;
				flag=1;
				init();
				Speedzhangai=17;
			}
			if(x>=KUAN/2+50&&y>=550&&x<=KUAN/2+250&&y<=600){
				model=1;
				flag=1;
				init();
				Speedzhangai=21;
			}
		}
}

int Setup(){
	p[0].kuan=50;
	p[0].gao=50;
	p[1].kuan=50;
	p[1].gao=50;
	p[2].kuan=100;
	p[2].gao=50;
	p[3].kuan=100;
	p[3].gao=100;
	p[4].kuan=50;
	p[4].gao=100;
	
	initWindow("方块规避------东东制作******All rights reserved",0,0,KUAN,GAO);	
	setTextBkColor(EMPTY);
	registerMouseEvent(mouseEvent);
	registerTimerEvent(timer);
	paint();
	return 0;
}
