#include <STC15F2K60S2.H>
#include <iic.h>
#include <ds1302.h>
#include <onewire.h>
#include <intrins.h>
sbit C1=P4^4;
sbit C2=P4^2;
sbit R1=P3^3;
sbit R2=P3^2;
unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,
	0xc0,0x86,0x8e,0xbf,0x7f};//数码管段码，0-f和-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};	

unsigned char big_mode=1;//1-显示，2-参数
unsigned char xianshi_mode=1;//1-时间，2-温度，3-亮暗
unsigned char canshu_mode=1;//1-时间参数，2-温度参数,3-指示灯参数
unsigned char time_canshu=17;
unsigned char real_time_canshu=17;
unsigned char temper_canshu=25;
unsigned char real_temper_canshu=25;
unsigned char led_canshu=4;
unsigned char real_led_canshu=4;
unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned int temper;
unsigned int v;
unsigned char v_temp;
unsigned char is_liang=0;//0-亮，1-暗
unsigned char count_an=0;
unsigned char count_liang=0;
unsigned char is_3s=1;
unsigned char is_L3=1;

void HC(unsigned char channel);
void InitSys();
void delay(unsigned int t);
void displaySMG(unsigned char pos,unsigned char value);
void showSMG();
void offSMG();
void scanKey();
void getTime();//读取时间和温度,以及光敏电阻的值
void run();
void showLED();
void InitTimer();
void main()
{
	InitSys();
	InitTimer();
	write_time();
	while(1)
	{
		run();
		scanKey();
	}
}
void run()
{
	getTime();
	showSMG();
	showLED();
}
void InitTimer()
{
	TMOD=0x10;
	TL1=(65535-50000+1)%256;
	TH1=(65535-50000+1)/256;
	TR1=1;
	ET1=1;
	EA=1;
}
void serviceT1() interrupt 3
{
	TL1=(65535-50000+1)%256;
	TH1=(65535-50000+1)/256;
	if((is_liang==1)&&(is_3s==0))
	{
		count_an++;
		if(count_an==60)
		{
			is_3s=1;
			count_an=0;
		}
	}
	if((is_liang==0)&&(is_3s==1))
	{
		count_liang++;
		if(count_liang==60)
		{
			is_3s=0;
			count_liang=0;
		}
	}
}
void HC(unsigned char channel)
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
void getTime()
{
	read_time();
	second=(TIMER[0]/16)*10+TIMER[0]%16;
	minute=(TIMER[1]/16)*10+TIMER[1]%16;
	hour=(TIMER[2]/16)*10+TIMER[2]%16;
	
	temper=read_temp();
	v_temp=read_PCF(0x01);
	v=(v_temp*100)/51;
	if(v_temp<51)
	{
		is_liang=1;
	}
	else
	{
		is_liang=0;
	}
}
void InitSys()
{
	HC(5);
	P0=0x00;
	HC(4);
	P0=0xff;
}
void delay(unsigned int t)
{
	while(t--);
}
void offSMG()
{
	HC(6);
	P0=0x00;
	HC(7);
	P0=0xff;
}
void displaySMG(unsigned char pos,unsigned char value)
{
	HC(6);
	P0=0x01<<pos;
	HC(7);
	P0=value;
	delay(100);
	offSMG();
}
void showSMG()
{
	if(big_mode==1)
	{
		if(xianshi_mode==1)
		{
			displaySMG(0,SMG_number[hour/10]);
			displaySMG(1,SMG_number[hour%10]);
			displaySMG(2,SMG_number[16]);
			displaySMG(3,SMG_number[minute/10]);
			displaySMG(4,SMG_number[minute%10]);
			displaySMG(5,SMG_number[16]);
			displaySMG(6,SMG_number[second/10]);
			displaySMG(7,SMG_number[second%10]);
		}
		else if(xianshi_mode==2)
		{
			displaySMG(0,SMG_number[12]);
			displaySMG(5,SMG_number[temper/100]);
			displaySMG(6,SMG_Dotnumber[(temper%100)/10]);
			displaySMG(7,SMG_number[temper%10]);
		}
		else if(xianshi_mode==3)
		{
			displaySMG(0,SMG_number[15]);
			
			displaySMG(2,SMG_Dotnumber[v/100]);
			displaySMG(3,SMG_number[(v%100)/10]);
			displaySMG(4,SMG_number[v%10]);
			
			displaySMG(7,SMG_number[is_liang]);
		}
	}
	else if(big_mode==2)
	{
		displaySMG(0,0x8c);//P
		if(canshu_mode==1)
		{
			displaySMG(1,SMG_number[1]);
			displaySMG(6,SMG_number[time_canshu/10]);
			displaySMG(7,SMG_number[time_canshu%10]);
		}
		else if(canshu_mode==2)
		{
			displaySMG(1,SMG_number[2]);
			displaySMG(6,SMG_number[temper_canshu/10]);
			displaySMG(7,SMG_number[temper_canshu%10]);
		}
		else if(canshu_mode==3)
		{
			displaySMG(1,SMG_number[3]);
			displaySMG(7,SMG_number[led_canshu]);
		}
	}
}
void scanKey()
{
	R1=0;
	R2=C1=C2=1;
	if(C1==0)//S4
	{
		delay(100);
		if(C1==0)
		{
			if(big_mode==1)
			{
				big_mode=2;
				xianshi_mode=1;
			}
			else
			{
				big_mode=1;
				canshu_mode=1;
			}
		}
		while(C1==0)
		{
			run();
		}
		
	}
	if(C2==0)//s8
	{
		delay(100);
		if(C2==0)
		{
			if(big_mode==2)
			{
				if(canshu_mode==1)
				{
					if(time_canshu==0)
					{
						time_canshu=23;
					}
					else
					{
						time_canshu--;
					}
				}
				else if(canshu_mode==2)
				{
					if(temper_canshu==0)
					{
						temper_canshu=99;
					}
					else
					{
						temper_canshu--;
					}
				}
				else if(canshu_mode==3)
				{
					if(led_canshu==4)
					{
						led_canshu=8;
					}
					else
					{
						led_canshu--;
					}
				}
			}
		}
		while(C2==0)
		{
			run();
		}
	}
	
	R2=0;
	R1=C1=C2=1;
	if(C1==0)//S5
	{
		delay(100);
		if(C1==0)
		{
			if(big_mode==1)
			{
				if(xianshi_mode==3)
				{
					xianshi_mode=1;
				}
				else
				{
					xianshi_mode++;
				}
			}
			else if(big_mode==2)
			{
				if(canshu_mode==1)
				{
					canshu_mode=2;
					real_time_canshu=time_canshu;
				}
				else if(canshu_mode==2)
				{
					canshu_mode=3;
					real_temper_canshu=temper_canshu;
				}
				else if(canshu_mode==3)
				{
					canshu_mode=1;
					real_led_canshu=led_canshu;
				}
			}
		}
		while(C1==0)
		{
			run();
		}
	}
	if(C2==0)//s9
	{
		delay(100);
		if(C2==0)
		{
			if(big_mode==2)
			{
				if(canshu_mode==1)
				{
					if(time_canshu==23)
					{
						time_canshu=0;
					}
					else
					{
						time_canshu++;
					}
				}
				else if(canshu_mode==2)
				{
					if(temper_canshu==99)
					{
						temper_canshu=0;
					}					
					else
					{
						temper_canshu++;
					}
				}
				else if(canshu_mode==3)
				{
					if(led_canshu==8)
					{
						led_canshu=4;
					}
					else
					{
						led_canshu++;
					}
				}
			}
		}
		while(C2==0)
		{
			run();
		}
	}
}

void showLED()
{
	
	HC(4);
	P0=0xff;
	if((TIMER[2]>=real_time_canshu)||(TIMER[2]<=8))//L1
	{
		P0=0xfe;
	}
	if(temper<real_temper_canshu)//L2
	{
		P0=(P0&0xfd);
	}
	if((is_liang==1)&&(is_3s==1))//L3
	{
		P0=P0&0xfb;
		is_L3=0;
	}
	else if((is_liang==0)&&(is_3s==0))
	{
		P0=P0&0xff;
		is_L3=1;
	}
	if(is_L3==0)
	{
		P0=P0&0xfb;
	}
	
	if(is_liang==1)
	{
		P0=P0&(~(0x01<<(led_canshu-1)));
	}
	
	delay(500);
}