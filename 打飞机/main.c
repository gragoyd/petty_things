#include "acllib.h"
#include<math.h>
#include<time.h> 
#include<stdlib.h>
//����ľ�̬������a,b,c
#define RAD(x) ((x)/360.0*2*3.1415926535)
#define kuan 1000				//��Ϸ������ 
#define gao 580					//��Ϸ����߶� 
#define Npaodan 10				//�ڵ�����
#define Nfeiji 5				//�ɻ�����
#define Hfeiji 100				//�ɻ�ͼ��ĸ� 
#define R 10					//�ڵ��뾶 
#define Speedpaodan 30			//�ڵ��ٶ� 
#define Speedfeiji 10			//�ɻ��ٶ� 
#define Gapfeiji 20			//�ɻ����ּ����*40ms��

char N1=Npaodan;				//ʣ���ڵ�����
char N2=Nfeiji;					//ʣ��ɻ�����	

ACL_Image img[3];				//����װ�ɻ�ͼƬ�Ľṹ��			 
 
struct Paodan{
	int x;
	int y;
	char status;								//0-δʹ�ã�1-�������� 
	char degree;								//���з��� 
}paodan[Npaodan];

struct Feiji{
	int x;
	int y;
	char status;							//0-δʹ�ã�1-����->��2-����<-��3-������ 
	char stay;							//�����к��Ϳ�Ч��ʱ������ 
}feiji[Nfeiji];	 

struct Paoguan{
	int x;
	int y;
	char degree;						//�ڹܷ��� 
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
	paintText(kuan/2-380,gao-25,"******��������******");
	paintText(kuan/2+110,gao-25,"All Rights Reserved");	
	setPenWidth(3);
	line(kuan/2-100,gao-30,kuan/2-100,gao);
	line(kuan/2+100,gao-30,kuan/2+100,gao);
	setBrushColor(RED);
	setPenWidth(0);
	rectangle(kuan/2-100,gao-30,kuan/2-100+200*N1/Npaodan,gao);
	paintText(kuan/2-80,gao-25,"ʣ���ڵ�ָʾ");
}

void paint(){
	beginPaint();	
	clearDevice();										//����������� 	
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

void crash(int i){										//�ж��ڵ��Ƿ���е�i�ܷɻ� 
	char j;
	for(j=0;j<Npaodan;j++){
		if((p[j].status)&&((q[i].status==1)||(q[i].status==2))&&(p[j].x>q[i].x)&&(p[j].x<q[i].x+2*Hfeiji)&&(p[j].y>q[i].y)&&(p[j].y<q[i].y+Hfeiji)){		//Ҫ���ڵ������ڷɻ�����ͼ�ڣ����߲���ɻ����� 
			q[i].status=3;
			p[j].x=0;								//���ջ����˷ɻ����ڵ� 
			p[j].y=0;
			p[j].status=0;
			N1++;
		}
	}
}

void create_feiji(char c){					//�����ɻ� 
	srand(time(0));
	q[c].y=rand()%151;						//�����y��0~150 
	if(rand()%2){									//���ȡ0��1��1�ʹ�������� 
		q[c].x=kuan;												//�ɻ���һ���ֻ����� 
		q[c].status=2;
	}		
	else{									//0�ʹ������ҷ�
		q[c].x=-2*Hfeiji;
		q[c].status=1;
	}	
}

void timer(int id){
	char i; 
	static char b=0,c=0;					//b���ڼ��������ɻ���c����ָʾ�ɻ� 
	if(id==0){
		if(b++==Gapfeiji){						//�����������ɻ� 
			b=0;
			if(N2>0){
				while(q[c].status)				//�����ҳ�δ��ʹ�õķɻ� 
					if(c++==Nfeiji)
						c=0;
				create_feiji(c);
				N2--;
			}
		} 		
		for(i=0;i<Npaodan;i++)	 
			if((p[i].status)&&((p[i].y<=-R)||(p[i].x>=kuan+R)||(p[i].x<=-R))){		//����δ���зɻ��ɳ�����ڵ� 
				p[i].x=0;
				p[i].y=0;
				p[i].status=0;
				N1++;
			}
		for(i=0;i<Nfeiji;i++){
			crash(i);
			if(((q[i].x<-2*Hfeiji)||(q[i].x>kuan))&&(q[i].status)){			//����û�����зɳ���ķɻ� 
				q[i].status=0;
				N2++;
			}
			if((q[i].status==3)&&(q[i].stay++==25)){						//���ձ����к��Ϳ��㹻��ʱ��ķɻ� ----1s 	
				q[i].status=0;
				q[i].stay=0;
				N2++; 
			}			
		}
	}
	if(id==1){
		for(i=0;i<Npaodan;i++){	 
			if(p[i].status){									//�ƶ��ڵ�
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
		paint();										//���»�ͼ��ˢ�»��� 
	}
}

void keyboard(int key, int event){
	char i; 
	static char a=0;								//a����ָʾ�ڵ� 
	if(key==VK_RIGHT&&event==0)					//�ڹ���ʱ��ת5�㣨���ת��ˮƽ��
		if(paoguan.degree<90)
			paoguan.degree+=5;
	if(key==VK_LEFT&&event==0)					//�ڹ�˳ʱ��ת5�㣨���ת��ˮƽ�� 
		if(paoguan.degree>-90)	
			paoguan.degree-=5;	
	if(key==VK_SPACE&&event==0){				//�ո����ڵ� 
		printf("�ո�\n");
		if(N1>0){
			while(p[a].status)					//�ӵ�ǰ�ڵ���ʼ�����ҳ�δʹ�õ� 
				if(a++==Npaodan)
					a=0;					
			p[a].x=paoguan.x;
			p[a].y=paoguan.y;
			p[a].degree=paoguan.degree;				//��¼ÿ���ڵ����з��� 
			p[a++].status=1; 
			if(a==Npaodan)
				a=0;
			N1--;
		}
	}		
}

int Setup(){
	initWindow("��ɻ�",0,0,kuan,gao);
	loadImage("�ɻ�0.jpg",&img[0]);
	loadImage("�ɻ�1.jpg",&img[1]);
	loadImage("��ը.jpg",&img[2]);
	registerKeyboardEvent(keyboard);
	registerTimerEvent(timer);
	startTimer(0,40);
	startTimer(1,10);
	return 0; 
} 
