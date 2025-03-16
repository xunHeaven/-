#include <STC15F2K60S2.H>
#include "iic.h"
#include <intrins.h>
sbit S4=P3^3;
sbit S5=P3^2;
sbit S6=P3^1;
sbit S7=P3^0;

unsigned char Led_mode=1;//Led灯闪烁模式1-4，工作模式
unsigned char SMG_mode=1;//数码管显示模式1-3
unsigned char Led_lumi=1;//亮度等级，1-4,由滑动变阻器控制
unsigned char pwm_duty=10;//pwm亮度控制
unsigned char countShine=0;//定时器1计时
unsigned char LedPwm=1;
unsigned char LEDCount=0;
unsigned char jiange=4;//LED工作模式流转间隔4-12，保存到AT24C02
unsigned char S7_key=1;//S7按下判断，如果按了S7，LED灯暂停或启动，1是启动
unsigned char getPCF=0;//读取PCF中AIN3的电压值保存
unsigned char count_100ms=0;//100毫秒计数
unsigned char S4_key=0;//数码管全灭状态下，是否按下S4
unsigned char is_800ms=0;//0.8s亮灭间隔判断标记
unsigned int code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
     0x80,0x90,0x88,0x83,0xc6,0xc0,0x86,0x8e,
	0xbf,0x7f};
unsigned char code SMG_Dot[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
unsigned char code LED_mode1[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//LED灯模式一亮灯数组
unsigned char code LED_mode2[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
unsigned char code LED_mode3[]={0x7e,0xbd,0xdb,0xe7};
unsigned char code LED_mode4[]={0xe7,0xdb,0xbd,0x7e};
void LED_show(unsigned char i,unsigned char mode);//LED显示函数
void SelectHC(unsigned char channel)
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
	SelectHC(4);
	P0=0xff;//关掉LED
	
	
	SelectHC(5);
	P0=P0&0xaf;//关掉蜂鸣器继电器
	
	SelectHC(6);
	P0=0xff;
	SelectHC(7);
	P0=0xff;//关掉数码管
}
void display_SMG(unsigned char pos,unsigned char value)
{
	SelectHC(7);
	P0=0xff;
	SelectHC(6);
	P0=0x01<<pos;
	SelectHC(7);
	P0=value;
}

void all_SMG()//使显示时数码管亮度相同
{
	SelectHC(7);
	P0=0xff;
	SelectHC(6);
	P0=0xff;
	SelectHC(7);
	P0=0xff;
}
void display_LED(unsigned pos)//led灯亮灭控制
{
	P0=0xff;
	SelectHC(4);
	P0=pos;
}
void delaySMG(unsigned int t)
{
	while(t--);
}

void SMG_show(unsigned char mode)//数码管显示内容
{
	switch(mode)
	{
		case 1:
			if(S4_key==0)
			{	
				all_SMG();
			}
			else
			{
				display_SMG(6,SMG_number[16]);
				delaySMG(100);
				display_SMG(7,SMG_number[Led_lumi]);
				delaySMG(100);
				all_SMG();
				delaySMG(100);
			}
			break;
		case 2:   //设置Led显示的模式
			if(is_800ms)//如果is_800ms=1，则显示模式的数码管当前处于点亮
			{
				display_SMG(0,SMG_number[16]);
				delaySMG(100);
				display_SMG(1,SMG_number[Led_mode]);
				delaySMG(100);
				display_SMG(2,SMG_number[16]);
				delaySMG(100);
				delaySMG(100);
			}
			else//如果is_800ms=0，则显示模式的数码管当前处于熄灭
			{
				display_SMG(0,0xff);
				delaySMG(100);
				display_SMG(1,0xff);
				delaySMG(100);
				display_SMG(2,0xff);
				delaySMG(100);
				delaySMG(100);
			}
			if(jiange>=10)//如果流转时间大于1000ms
			{
				display_SMG(4,SMG_number[jiange/10]);
				delaySMG(100);
				display_SMG(5,SMG_number[jiange%10]);
				delaySMG(100);
				display_SMG(6,SMG_number[0]);
				delaySMG(100);
				display_SMG(7,SMG_number[0]);
				delaySMG(100);
				all_SMG();
				delaySMG(100);
			}
			else
			{
				display_SMG(5,SMG_number[jiange%10]);
				delaySMG(100);
				display_SMG(6,SMG_number[0]);
				delaySMG(100);
				display_SMG(7,SMG_number[0]);
				delaySMG(100);
				all_SMG();
				delaySMG(100);
			}
			break;
			
		case 3://设置led流转时间的模式
			display_SMG(0,SMG_number[16]);
			delaySMG(100);
			display_SMG(1,SMG_number[Led_mode]);
			delaySMG(100);
			display_SMG(2,SMG_number[16]);
			delaySMG(100);
			if(is_800ms)
			{
				if(jiange>=10)//如果流转时间大于1000ms
				{
					display_SMG(4,SMG_number[jiange/10]);
					delaySMG(100);
					display_SMG(5,SMG_number[jiange%10]);
					delaySMG(100);
					display_SMG(6,SMG_number[0]);
					delaySMG(100);
					display_SMG(7,SMG_number[0]);
					delaySMG(100);
					all_SMG();
					delaySMG(100);
				}
				else
				{
					display_SMG(5,SMG_number[jiange%10]);
					delaySMG(100);
					display_SMG(6,SMG_number[0]);
					delaySMG(100);
					display_SMG(7,SMG_number[0]);
					delaySMG(100);
					all_SMG();
					delaySMG(100);
				}
			}
			else
			{
				display_SMG(4,0xff);
				delaySMG(100);
				display_SMG(5,0xff);
				delaySMG(100);
				display_SMG(6,0xff);
				delaySMG(100);
				display_SMG(7,0xff);
				delaySMG(100);
				all_SMG();
				delaySMG(100);
			}
			
			break;
	}
}
void keyCtrl()//按键操作
{
	if(S4==0)
	{
		delaySMG(100);
		if(S4==0)
		{
			if(SMG_mode==1)//如果是数码管的模式1，则按下后数码管显示led亮度模式
			{
				S4_key=1;
			}
			else if(SMG_mode==2)//如果是数码管模式2，可以对LED显示模式进行设置
			{
				if(Led_mode==1)
				{
					Led_mode=4;
				}
				else
				{
					Led_mode--;
				}
			}
			else//如果是数码管模式3，则对LED模式流转时间进行设置
			{
				if(jiange==4)
				{
					jiange=12;
				}
				else
				{
					jiange--;
				}
			}
		}
		while(S4==0)
		{
			SMG_show(SMG_mode);
			LED_show(0,Led_mode);
		}
		S4_key=0;
	}
	
	if(S5==0)//按下S5，在设置数码管设置界面进行加操作
	{
		delaySMG(100);
		if(S5==0)
		{
			if(SMG_mode==2)
			{
				if(Led_mode==4)
				{
					Led_mode=1;
				}
				else
				{
					Led_mode++;
				}
			}
			else if(SMG_mode==3)
			{
				if(jiange==12)
				{
					jiange=4;
				}
				else
				{
					jiange++;
				}
			}
		}
		while(S5==0)
		{
			SMG_show(SMG_mode);
			LED_show(0,Led_mode);
		}
	}
	
	if(S6==0)//按下S6，数码管模式改变
	{
		delaySMG(100);
		if(S6==0)
		{
			if(SMG_mode==3)
			{
				write_AT(0,jiange);//将Led模式流转时间间隔存入eeprom
				SMG_mode=1;
			}
			else
			{
				SMG_mode++;
			}
		}
		while(S6==0)
		{
			SMG_show(SMG_mode);
			LED_show(0,Led_mode);
		}
	}
	if(S7==0)//按下S7后Led暂停或启动
	{
		delaySMG(100);
		if(S7==0)
		{
			if(S7_key==0)
			{
				S7_key=1;
			}
			else
			{
				S7_key=0;
			}
		}
		while(S7==0)
		{
			//SMG_show(SMG_mode);
			LED_show(0,Led_mode);
		}
	}
}



void getLumi()//led灯亮度等级判断
{
	getPCF=Read_PCF(0x03);//读取滑动变阻器的分压0-255
	if(getPCF<64)
	{
		Led_lumi=1;
	}
	else if(getPCF<128)
	{
		Led_lumi=2;
	}
	else if(getPCF<192)
	{
		Led_lumi=3;
	}
	else
	{
		Led_lumi=4;
	}
	
}
unsigned char P0L,P0H;//Led灯低4位和高4位
void LED_show(unsigned char i,unsigned char mode)//LED显示函数
{
	SelectHC(4);
	P0=0xff;
	if(LedPwm==0)
	{
		if(mode==1)
	{
		SelectHC(4);
		P0=LED_mode1[LEDCount%8];
		
	}
	else if(mode==2)
	{
		SelectHC(4);
		P0=LED_mode2[LEDCount%8];
	}
	else if(mode==3)
	{
		SelectHC(4);
		P0=LED_mode3[LEDCount%4];
	}
	else if(mode==4)
	{
		SelectHC(4);
		P0=LED_mode4[LEDCount%4];
	}
	}
	else
	{
		SelectHC(4);
		P0=0xff;
	}
	
}
void InitTimer0()//10ms定时器
{
	TMOD = 0x01;                    //设置定时器为模式0(16位非重装载)
    TL0 = (65535-10000)%256;                     //初始化计时值
    TH0 = (65535-10000)/256;
    TR0 = 1;                        //定时器0开始计时
    ET0 = 1;                        //使能定时器0中断
    EA = 1;
}

void InitTimer1()
{
	TMOD = 0x10;                    //设置定时器为模式0(16位自动重装载)
    TL1 = (65535-100)%256;                     //初始化计时值
    TH1 = (65535-100)/256; 
    TR1 = 1;                        //定时器1开始计时
    ET1 = 1;                        //使能定时器0中断
    EA = 1;
}

void ServiceTimer1() interrupt 3
{
	TL1 = (65535-100)%256;                     //初始化计时值
    TH1 = (65535-100)/256; 
	countShine++;
	if(countShine==pwm_duty*Led_lumi)
	{
		LedPwm=1;//高电平
	}
	else if(countShine==100)
	{
		LedPwm=0;//低电平
		countShine=0;
	}
	
	
	
}

unsigned int TimeFor800ms=0;//数码管闪烁800ms计时
unsigned int TimeForLed=0;//Led模式流转计时
unsigned char jishi=0;
void ServiceTimer0() interrupt 1
{
	TL0 = (65535-10000)%256;                     //初始化计时值
    TH0 = (65535-10000)/256;
	TimeFor800ms++;
	if(S7_key==1)//当S7_key=0时LED流转计时暂停
	{
		TimeForLed++;
		jishi++;
	}
	if(jishi==8)//led闪烁
	{
		LEDCount++;
		jishi=0;
	}
	if(LEDCount==8)
	{
		LEDCount==0;
	}
	
	if(TimeFor800ms==80)//数码管闪烁800ms判断
	{
		if(is_800ms==0)
		{
			is_800ms=1;
		}
		else
		{
			is_800ms=0;
		}
		TimeFor800ms=0;
	}
	if((TimeForLed/10)/jiange==1)//定时器到达流转时间就进行Led模式转换
	{
		TimeForLed=0;
		if(Led_mode==4)
		{
			Led_mode=1;
		}
		else
		{
			Led_mode++;
		}
		LEDCount=0;
		
	}
	//SMG_show(SMG_mode);
}


void main()
{
	InitSys();
	jiange=read_AT(0);
	InitTimer0();
	InitTimer1();
	//jiange=12;
	while(1)
	{
		getLumi();//LED亮度等级
		keyCtrl();//按键操作
		SMG_show(SMG_mode);//数码管内容显示；
		LED_show(0,Led_mode);//LED灯内容显示
		
		
	}
}