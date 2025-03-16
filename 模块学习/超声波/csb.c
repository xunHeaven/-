#include <STC15F2K60S2.H>
#include <intrins.h>

sbit TX=P1^0;
sbit RX=P1^1;
unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
     0x80,0x90,0x88,0x83,0xc6,0xc0,0x86,0x8e,
	0xbf,0x7f};//数码管段码，0-f和-.
unsigned int distance=0;
void InitSys();
void HCselect(unsigned char channel);
void displaySMG(unsigned char pos,unsigned char value);
void showSMG();
void delay(unsigned int t);
void offSMG();
unsigned char read_Dis();


void main()
{
	InitSys();
	while(1)
	{
		distance=read_Dis();
		showSMG();
	}
}

void HCselect(unsigned char channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0x1f)|0x80;
			break;
		case 5:
			P2=(P2&0x1f)|0xa0;
			break;
		case 6:
			P2=(P2&0x1f)|0xc0;
			break;
		case 7:
			P2=(P2&0x1f)|0xe0;
			break;
	}
}
void InitSys()
{
	HCselect(5);
	P0=P0&0xaf;
	HCselect(4);
	P0=0xff;
}
void displaySMG(unsigned char pos,unsigned char value)
{
	HCselect(6);
	P0=(0x01<<pos);
	HCselect(7);
	P0=value;
}
void offSMG()
{
	HCselect(6);
	P0=0x00;
	HCselect(7);
	P0=0xff;
}
void delay(unsigned int t)
{
	while(t--);
}
void showSMG()
{
	displaySMG(5,SMG_number[distance/100]);
	delay(500);
	offSMG();
	displaySMG(6,SMG_number[(distance/10)%10]);
	delay(500);
	offSMG();
	displaySMG(7,SMG_number[distance%10]);
	delay(500);
	offSMG();
}
unsigned char read_Dis()
{
	unsigned char distance,num=10;
	TX=0;//TX=1发送超声波
	CL=0xf3;//设置定时初值
	CH=0xff;//设置定时初值
	CR=1;//PCA计时
	//TX引脚发射40KHz方波信号驱动超声波发送探头
	while(num--)
	{
		while(!CF);//12us后CF=1，跳出循环
		TX=~TX;
		CL=0xf3;//设置定时初值
		CH=0xff;
		CF=0;
	}
	CR=0;
	CL=0;
	CH=0;
	CR=1;
	while(RX && (!CF));//等待收到脉冲
	CR=0;
	if(CF)//发生溢出
	{
		CF=0;
		distance=255;
	}
	else //计算距离
	{
		distance=((CH<<8)+CL)*0.017;
	}
	return distance;
}