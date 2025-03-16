#include <STC15F2K60S2.H>
#include "iic.h"
#include <intrins.h>

//矩阵键盘行列定义
sbit R1=P3^3;
sbit R2=P3^2;
sbit C1=P3^5;
sbit C2=P3^4;

unsigned char smg_state=0;//数码管界面显示模式，0时为数据界面，1时为参数界面，2时为计数界面
unsigned char Vin3_pre;//滑动变阻器上一次的分压值
unsigned int Vin3;//滑动变阻器当前的分压值
unsigned int Vp=0;//电压参数
unsigned int move_count=0;//计数值
unsigned char Vin3_Vp_state=0;//Vin3和Vp的大小状态

unsigned char L1_enable=0;//L1使能
unsigned char L3_enable=0;//L3使能
//定义动态显示中单个数码管点亮时长
#define TSMG 500
//数码管段码表
unsigned char code SMG_NoDot[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
     0x80,0x90,0x88,0x83,0xc6,0xc0,0x86,0x8e,
	0xbf,0x7f};//包含0-f和-.
unsigned char code SMG_Dot[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};//包含0.到9.
//

void selectHC573(unsigned char channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&&0x1f)|0x80;
			break;
		case 5:
			P2=(P2&&0x1f)|0xa0;
			break;
		case 6:
			P2=(P2&&0x1f)|0xc0;
			break;
		case 7:
			P2=(P2&&0x1f)|0xe0;
			break;
	}
}
void sysInit()//单片机初始化
{
	selectHC573(5);
	P0=P0&0xaf;//关掉继电器和蜂鸣器
	
	selectHC573(4);
	P0=0xff;//关掉LED灯
	
}
//数码管专用延时函数
void delaySMG(unsigned int t)
{
	while(t--);
}
void display_SMG(unsigned char pos,unsigned char value)//数码管位选和显示值
{
	selectHC573(7);
	P0=0xff;//消影
	
	selectHC573(6);
	P0=(0x01<<pos);//数码管位选
	
	selectHC573(7);
	P0=value;//数码管显示的内容
}
void ctrl_SMG()//数码管位选和显示值
{
	selectHC573(7);
	P0=0xff;//消影
	
	selectHC573(6);
	P0=0xff;//数码管位选
	
	selectHC573(7);
	P0=0xff;//数码管显示的内容
}
void SMG_show(unsigned char mode)
{
	switch(mode)
	{
		case 0:
			display_SMG(0,0xc1);//0位数码管显示U
			delaySMG(100);
			display_SMG(5,SMG_Dot[Vin3/100]);
			delaySMG(100);
			display_SMG(6,SMG_NoDot[(Vin3%100)/10]);
			delaySMG(100);
			display_SMG(7,SMG_NoDot[Vin3%10]);
			delaySMG(100);
			ctrl_SMG();
			delaySMG(100);
			break;
		case 1:
			display_SMG(0,0x8c);//0位数码管显示P
			delaySMG(100);
			display_SMG(5,SMG_Dot[Vp/100]);
			delaySMG(100);
			display_SMG(6,SMG_NoDot[(Vp%100)/10]);
			delaySMG(100);
			display_SMG(7,SMG_NoDot[Vp%10]);
			delaySMG(100);
			ctrl_SMG();
			delaySMG(100);
			break;
		case 2:
			display_SMG(0,0xc8);//0位数码管显示n
			delaySMG(100);
			display_SMG(6,SMG_NoDot[move_count/10]);
			delaySMG(100);
			display_SMG(7,SMG_NoDot[move_count%10]);
			delaySMG(100);
			ctrl_SMG();
			delaySMG(100);
			break;
	}
}

void keyCtrl()
{
	R1=0;
	R2=1;
	C1=C2=1;
	if(C1==0)//S12
	{
		delaySMG(200);
		if(C1==0)
		{
			L3_enable=0;
			if(smg_state==2)
			{
				smg_state=0;
			}
			else if(smg_state==1)
			{
				smg_state++;
				AT24_Write(0x00,Vp/10);//退出参数界面时将Vp*10，写入EEPROM的第0号单元；
			}
			else
			{
				smg_state++;
			}
		}
		while(C1==0)
		{
			SMG_show(smg_state);
		}
	}
	else if(C2==0)//S16
	{
		delaySMG(200);
		if(C2==0)
		{
			if(smg_state==1)//要在参数界面才生效
			{
				if(Vp==500)
				{
					Vp=0;
				}
				else
				{
					Vp=Vp+50;
				}
			}
			else
			{
				L3_enable++;
			}
		}
		while(C2==0)
		{
			SMG_show(smg_state);
		}
		
	}
	
	R1=1;
	R2=0;
	C1=C2=1;
	if(C1==0)//S13
	{
		delaySMG(200);
		if(C1==0)
		{
			if(smg_state==2)//要在计数界面才生效
			{
				move_count=0;
			}
			else
			{
				L3_enable++;
			}
		}
		while(C1==0)
		{
			SMG_show(smg_state);
		}
	}
	else if(C2==0)//S17
	{
		delaySMG(200);
		if(C2==0)
		{
			if(smg_state==1)//要在参数界面才生效
			{
				if(Vp==0)
				{
					Vp=500;
				}
				else
				{
					Vp=Vp-50;
				}
			}
			else
			{
				L3_enable++;
			}
		}
		while(C2==0)
		{
			SMG_show(smg_state);
		}
	}
	
}

void InitTimer0()
{
	TMOD = 0x01;                    //设置定时器为模式0
    TL0 = (65535-50000)%256;                     //初始化计时值
    TH0 = (65535-50000)/256;
    TR0 = 1;                        //定时器0开始计时
    ET0 = 1;                        //使能定时器0中断
    EA = 1;
}


unsigned int t_Led1=0;//对Led灯的L1进行使能
unsigned char Led1=0;
void SeviceTimer0() interrupt 1
{
	TL0 = (65535-50000)%256;                     //初始化计时值
    TH0 = (65535-50000)/256;
	if(L1_enable==1)
	{
		t_Led1++;
		if(t_Led1>=100)//Vin3<Vp,大于5秒L1点亮
		{
			Led1=1;
		}
	}
	else
	{
		Led1=0;
		t_Led1=0;
	}
	
	
	
}

void Delay50ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 3;
	j = 72;
	k = 161;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void main()
{
	sysInit();
	//AT24_Write(0x00,0);
	//delaySMG(500);
	//Vp=(AT24_Read(0x00))*10;//读取Vp的值
	InitTimer0();
	
	while(1)
	{
		Vin3_pre=PCF_Read(0x03);
		Vin3=(unsigned int)(Vin3_pre*100/255)*5;//从PCF读取Vin3的值，直接读取的值为0-255，要转化成0-5，通过扩大100倍保留三位有效数字
		
		if(Vin3>Vp) {Vin3_Vp_state=1;}//做标记
		if((Vin3_Vp_state==1)&&(Vin3<=Vp)) 
		{
			move_count++;
			Vin3_Vp_state=0;
		}//满足条件，计数值+1
		SMG_show(smg_state);
		keyCtrl();
		
		if(Vin3<Vp) L1_enable=1;//如果Vin3<Vp，开始计时，五秒后点亮L1
		else
		{
			L1_enable=0;
		}
		if(Led1==1)
		{
			selectHC573(4);
			P0=P0&0xfe;
		}
		else
		{
			selectHC573(4);
			P0=P0|0x01;
		}

		if(move_count%2!=0)//如果计数为奇数则L2点亮
		{
			selectHC573(4);
			P0=P0&0xfd;
		}
		else
		{
			selectHC573(4);
			P0=P0|0x02;
		}
		
		if(L3_enable>=3)
		{
			selectHC573(4);
			P0=P0&0xfb;
		}
		else
		{
			selectHC573(4);
			P0=P0|0x04;
		}
		
		
		
	}
}
