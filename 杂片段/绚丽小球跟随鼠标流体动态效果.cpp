#include <graphics.h>
#include <math.h>
#include <time.h>

#define WIDTH		1500		// 屏幕宽
#define HEIGHT		800			// 屏幕高
#define NUM_MOVERS	100			// 小球数量
#define	FRICTION	0.96f		// 摩擦力
#define SPEED		20			//小球跟随弹开速度

struct Mover		// 定义小球结构
{
	COLORREF	color;			// 颜色
	float		x, y;			// 坐标
	float		vX, vY;			// 速度
};


// 定义全局变量
Mover	movers[NUM_MOVERS];			// 小球数组
int		mouseX, mouseY;				// 当前鼠标坐标
int		mouseVX, mouseVY;			// 鼠标速度
int		prevMouseX, prevMouseY;		// 上次鼠标坐标
bool	isMouseDown;				// 鼠标左键是否按下
DWORD*	pBuffer;					// 显存指针


void init()			// 初始化
{
	for (int i = 0; i < NUM_MOVERS; i++)	// 初始化小球数组
	{
		movers[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
		movers[i].x = WIDTH * 0.5;
		movers[i].y = HEIGHT * 0.5;
		movers[i].vX = float(cos(float(i))) * (rand() % 34);
		movers[i].vY = float(sin(float(i))) * (rand() % 34);
	}

	mouseX = prevMouseX = WIDTH / 2;		// 初始化鼠标变量
	mouseY = prevMouseY = HEIGHT / 2;
	srand((unsigned int)time(NULL));
	pBuffer = GetImageBuffer(NULL);			// 获取显存指针
}


void darken()			// 全屏变暗 50%
{
	for (int i = WIDTH * HEIGHT - 1; i >= 0; i--)
		if (pBuffer[i] != 0)
			pBuffer[i] = RGB(GetRValue(pBuffer[i]) >> 1, GetGValue(pBuffer[i]) >> 1, GetBValue(pBuffer[i]) >> 1);
}


void delay(DWORD ms)		// 绝对延时
{
	static DWORD oldtime = GetTickCount();
	while (GetTickCount() - oldtime < ms)
		Sleep(1);
	oldtime = GetTickCount();
}


void run()			// 绘制动画(一帧)
{
	darken();	// 全屏变暗

	mouseVX = mouseX - prevMouseX;
	mouseVY = mouseY - prevMouseY;
	prevMouseX = mouseX;
	prevMouseY = mouseY;

	float toDist = WIDTH * 0.86f;
	float stirDist = WIDTH * 0.125f;
	float blowDist = WIDTH * 0.5f;

	for (int i = 0; i < NUM_MOVERS; i++)
	{
		float x = movers[i].x;
		float y = movers[i].y;
		float vX = movers[i].vX;
		float vY = movers[i].vY;

		float dX = x - mouseX;
		float dY = y - mouseY;
		float d = (float)sqrt(dX * dX + dY * dY);
		dX = d ? dX / d : 0;
		dY = d ? dY / d : 0;

		if (isMouseDown && d < blowDist)
		{
			float blowAcc = (1 - (d / blowDist)) * 14;
			vX += dX * blowAcc + 0.5f - float(rand()) / RAND_MAX;
			vY += dY * blowAcc + 0.5f - float(rand()) / RAND_MAX;
		}

		if (d < toDist)
		{
			float toAcc = (1 - (d / toDist)) * WIDTH * 0.0014f;
			vX -= dX * toAcc;
			vY -= dY * toAcc;
		}

		if (d < stirDist)
		{
			float mAcc = (1 - (d / stirDist)) * WIDTH * 0.00026f;
			vX += mouseVX * mAcc;
			vY += mouseVY * mAcc;
		}

		vX *= FRICTION;
		vY *= FRICTION;

		float avgVX = (float)fabs(vX);
		float avgVY = (float)fabs(vY);
		float avgV = (avgVX + avgVY) * 0.5f;

		if (avgVX < 0.1) vX *= float(rand()) / RAND_MAX * 3;
		if (avgVY < 0.1) vY *= float(rand()) / RAND_MAX * 3;

		float sc = avgV * 0.45f;
		sc = max(min(sc, 3.5f), 0.4f);

		float nextX = x + vX;
		float nextY = y + vY;

		if (nextX > WIDTH) { nextX = WIDTH;	vX *= -1; }
		else if (nextX < 0) { nextX = 0;		vX *= -1; }
		if (nextY > HEIGHT) { nextY = HEIGHT;	vY *= -1; }
		else if (nextY < 0) { nextY = 0;		vY *= -1; }

		movers[i].vX = vX;
		movers[i].vY = vY;
		movers[i].x = nextX;
		movers[i].y = nextY;

		// 画小球
		setcolor(movers[i].color);
		setfillstyle(movers[i].color);
		fillcircle(int(nextX + 0.5), int(nextY + 0.5), int(sc + 0.5));
	}
}

int main()
{
	initgraph(WIDTH, HEIGHT);	// 创建绘图窗口
	BeginBatchDraw();			// 启用批绘图模式
	init();
	MOUSEMSG m;					// 鼠标消息变量
	while (true)
	{
		while (MouseHit())		// 处理鼠标消息
		{
			m = GetMouseMsg();
			switch (m.uMsg)
			{
			case WM_MOUSEMOVE:		mouseX = m.x;	mouseY = m.y;	break;
			case WM_LBUTTONDOWN:	isMouseDown = true;				break;
			case WM_LBUTTONUP:		isMouseDown = false;			break;
			}
		}
		run();
		FlushBatchDraw();		// 显示缓存的绘制内容
		delay(SPEED);
	}

	EndBatchDraw();
	closegraph();
	return EXIT_SUCCESS;
}