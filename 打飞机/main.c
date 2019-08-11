#include "acllib.h"
#include<math.h>
#include<time.h> 
#include<stdlib.h>
//定义的静态变量有a,b,c
#define RAD(x) ((x)/360.0*2*3.1415926535)
#define kuan 1000				//游戏界面宽度 
#define gao 580					//游戏界面高度 
#define Npaodan 10				//炮弹总数
#define Nfeiji 5				//飞机总数
#define Hfeiji 100				//飞机图像的高 
#define R 10					//炮弹半径 
#define Speedpaodan 30			//炮弹速度 
#define Speedfeiji 10			//飞机速度 
#define Gapfeiji 20			//飞机出现间隔（*40ms）

char N1=Npaodan;				//剩余炮弹数量
char N2=Nfeiji;					//剩余飞机数量	

ACL_Image img[3];				//定义装飞机图片的结构体			 
 
struct Paodan{
	int x;
	int y;
	char status;								//0-未使用，1-正常飞行 
	char degree;								//飞行方向 
}paodan[Npaodan];

struct Feiji{
	int x;
	int y;
	char status;							//0-未使用，1-正常->，2-正常<-，3-被击中 
	char stay;							//被击中后滞空效果时长计数 
}feiji[Nfeiji];	 

struct Paoguan{
	int x;
	int y;
	char degree;						//炮管方向 
}paoguan;

struct Paodan * p=paodan;
struct Feiji * q=feiji;
 
void paint_paoguan(){
	paoguan.x=kuan/2+100*sin(RAD(paoguan.degree));
	paoguan.y=gao-30-100*cos(RAD(paoguan.degree));
	line(kuan/2,gao-30,paoguan.x,paoguan.y);
}

void paint_paodan(){
	char i;
	setBrushColor(BLACK);
	for(i=0;i<Npaodan;i++)
		if(p[i].status)
			ellipse(p[i].x-R,p[i].y-R,p[i].x+R,p[i].y+R);
}

void paint_feiji(){
	char i;
	for(i=0;i<Nfeiji;i++){
		switch(q[i].status){
			case 1:{
				putImageScale(&img[0],q[i].x,q[i].y,2*Hfeiji,Hfeiji);
				break;
			}
			case 2:{				
				putImageScale(&img[1],q[i].x,q[i].y,2*Hfeiji,Hfeiji);				
				break;
			}
			case 3:{
				putImageScale(&img[2],q[i].x,q[i].y,2*Hfeiji,Hfeiji);
				break;
			}
		}	
	}
}

void paint_za(){
	setTextSize(25);
	setTextBkColor(EMPTY);
	paintText(kuan/2-380,gao-25,"******东东制作******");
	paintText(kuan/2+110,gao-25,"All Rights Reserved");	
	setPenWidth(3);
	line(kuan/2-100,gao-30,kuan/2-100,gao);
	line(kuan/2+100,gao-30,kuan/2+100,gao);
	setBrushColor(RED);
	setPenWidth(0);
	rectangle(kuan/2-100,gao-30,kuan/2-100+200*N1/Npaodan,gao);
	paintText(kuan/2-80,gao-25,"剩余炮弹指示");
}

void paint(){
	beginPaint();	
	clearDevice();										//清除残留画面 	
	setPenWidth(3);
	line(0,gao-30,kuan,gao-30);
	setPenWidth(20);
	paint_paoguan();
	setPenWidth(0);
	paint_paodan();
	paint_feiji();
	paint_za();
	endPaint();
}

void crash(int i){										//判断炮弹是否击中第i架飞机 
	char j;
	for(j=0;j<Npaodan;j++){
		if((p[j].status)&&((q[i].status==1)||(q[i].status==2))&&(p[j].x>q[i].x)&&(p[j].x<q[i].x+2*Hfeiji)&&(p[j].y>q[i].y)&&(p[j].y<q[i].y+Hfeiji)){		//要求炮弹中心在飞机矩形图内，擦边蹭过飞机不算 
			q[i].status=3;
			p[j].x=0;								//回收击中了飞机的炮弹 
			p[j].y=0;
			p[j].status=0;
			N1++;
		}
	}
}

void create_feiji(char c){					//产生飞机 
	srand(time(0));
	q[c].y=rand()%151;						//随机给y赋0~150 
	if(rand()%2){									//随机取0或1，1就从右往左飞 
		q[c].x=kuan;												//飞机有一部分画不出 
		q[c].status=2;
	}		
	else{									//0就从左往右飞
		q[c].x=-2*Hfeiji;
		q[c].status=1;
	}	
}

void timer(int id){
	char i; 
	static char b=0,c=0;					//b用于计量产生飞机，c用于指示飞机 
	if(id==0){
		if(b++==Gapfeiji){						//按条件产生飞机 
			b=0;
			if(N2>0){
				while(q[c].status)				//遍历找出未被使用的飞机 
					if(c++==Nfeiji)
						c=0;
				create_feiji(c);
				N2--;
			}
		} 		
		for(i=0;i<Npaodan;i++)	 
			if((p[i].status)&&((p[i].y<=-R)||(p[i].x>=kuan+R)||(p[i].x<=-R))){		//回收未击中飞机飞出界的炮弹 
				p[i].x=0;
				p[i].y=0;
				p[i].status=0;
				N1++;
			}
		for(i=0;i<Nfeiji;i++){
			crash(i);
			if(((q[i].x<-2*Hfeiji)||(q[i].x>kuan))&&(q[i].status)){			//回收没被击中飞出界的飞机 
				q[i].status=0;
				N2++;
			}
			if((q[i].status==3)&&(q[i].stay++==25)){						//回收被击中后滞空足够长时间的飞机 ----1s 	
				q[i].status=0;
				q[i].stay=0;
				N2++; 
			}			
		}
	}
	if(id==1){
		for(i=0;i<Npaodan;i++){	 
			if(p[i].status){									//移动炮弹
				p[i].x+=Speedpaodan*sin(RAD(p[i].degree));
				p[i].y-=Speedpaodan*cos(RAD(p[i].degree));
			}
		}
		for(i=0;i<Nfeiji;i++){
			if(q[i].status==1)
				q[i].x+=Speedfeiji;
			if(q[i].status==2)
				q[i].x-=Speedfeiji;
		}
		paint();										//重新画图，刷新画面 
	}
}

void keyboard(int key, int event){
	char i; 
	static char a=0;								//a用于指示炮弹 
	if(key==VK_RIGHT&&event==0)					//炮管逆时针转5°（最多转至水平）
		if(paoguan.degree<90)
			paoguan.degree+=5;
	if(key==VK_LEFT&&event==0)					//炮管顺时针转5°（最多转至水平） 
		if(paoguan.degree>-90)	
			paoguan.degree-=5;	
	if(key==VK_SPACE&&event==0){				//空格发射炮弹 
		printf("空格\n");
		if(N1>0){
			while(p[a].status)					//从当前炮弹开始遍历找出未使用的 
				if(a++==Npaodan)
					a=0;					
			p[a].x=paoguan.x;
			p[a].y=paoguan.y;
			p[a].degree=paoguan.degree;				//记录每发炮弹飞行方向 
			p[a++].status=1; 
			if(a==Npaodan)
				a=0;
			N1--;
		}
	}		
}

int Setup(){
	initWindow("打飞机",0,0,kuan,gao);
	loadImage("飞机0.jpg",&img[0]);
	loadImage("飞机1.jpg",&img[1]);
	loadImage("爆炸.jpg",&img[2]);
	registerKeyboardEvent(keyboard);
	registerTimerEvent(timer);
	startTimer(0,40);
	startTimer(1,10);
	return 0; 
} 
