#include "acllib.h"

char flag=0;

void mouseListener(int x, int y, int button, int event){
	static int ox,oy;
	if(button==3&&event==0)
		flag=1;
	if(event==2){
		flag=0;
	}
	if(flag==1){
		beginPaint();
		line(ox,oy,x,y);
		endPaint();
	}		
	ox=x;
	oy=y;
}

int Setup(){
	initWindow("\t\t\t\t\t\t»­Ğ´°å---°´×¡ÓÒ¼üÍÏ¶¯",0,0,1500,750);
	registerMouseEvent(mouseListener);
	return 0;
}
