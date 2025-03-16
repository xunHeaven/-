#include <STC15F2K60S2.H>
#include <iic.h>
#include <ds1302.h>
#include <intrins.h>
sbit TX=P1^0;
sbit RX=P1^1;

unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,
	0xc0,0x86,0x8e,0xbf,0x7f};//数码管段码，0-f和-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
sbit C1=P4^4;
sbit C2=P4^2;
sbit R1=P3^3;
sbit R2=P3^2;
unsigned char big_mode=1;//1-2
unsigned char xianshi_mode=1;//1-时间，2-距离，3-数据记录
unsigned char ceju_mode=1;//2-定时测距，1-触发方式测距
unsigned char juli_mode=1;//1-最大值，3-平均值，2-最小值
unsigned char canshu_mode=1;//1-采集时间，2-距离参数
unsigned int distance;
unsigned int maxDis=0;
unsigned int minDis=0;
unsigned long aveDis=0;
unsigned int count_Dis=0;//距离总计数次数
unsigned int count_tDis=0;
unsigned char time_canshu=2;
unsigned char distance_canshu=20;
unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char count_s1=0;//测距间隔
unsigned char is_1s=1;
unsigned char liangdu=0;//由亮转暗标识；
unsigned char RB1;
unsigned int Dis_arr[3]=0;
unsigned int outV;

void HCSelect(unsigned char channel);
void InitSys();
void displaySMG(unsigned char pos,unsigned char value);
void showSMG();
void offSMG();
void delay(unsigned int t);
void run();
void scanKey();
void getTime();//读取ds1302时间
void read_Dis();
void get_Dis();//得到距离
void InitTimer();
void outDAC();
void showLED();
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
	get_Dis();
	outDAC();
	showSMG();
	showLED();
}
void InitTimer()
{
	TMOD=0x01;
	TL0=(65535-50000+1)%256;
	TH0=(65535-50000+1)/256;
	TR0=1;
	ET0=1;
	EA=1;
}
void ServiceTimer() interrupt 1
{
	TL0=(65535-50000+1)%256;
	TH0=(65535-50000+1)/256;
	if(is_1s==0)
	{
		count_s1++;
		if(count_s1==20)
		{
			is_1s=1;
			count_s1=0;
		}
	}
	
}

void HCSelect(unsigned char channel)
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
	HCSelect(4);
	P0=0xff;
	HCSelect(5);
	P0=0x00;
}
void displaySMG(unsigned char pos,unsigned char value)
{
	HCSelect(6);
	P0=0x01<<pos;
	HCSelect(7);
	P0=value;
	delay(100);
	offSMG();
}
void delay(unsigned int t)
{
	while(t--);
}
void offSMG()
{
	HCSelect(6);
	P0=0x00;
	HCSelect(7);
	P0=0xff;
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
			displaySMG(0,0xc7);//L
			if(ceju_mode==1)
			{
				displaySMG(2,SMG_number[12]);
			}
			else if(ceju_mode==2)
			{
				displaySMG(2,SMG_number[15]);
			}
			if(distance>99)
			{
				displaySMG(5,SMG_number[distance/100]);
			}
			if(distance>9)
			{
				displaySMG(6,SMG_number[(distance%100)/10]);
			}
			displaySMG(7,SMG_number[distance%10]);
		}
		else if(xianshi_mode==3)
		{
			displaySMG(0,0x89);//H
			if(juli_mode==1)
			{
				displaySMG(1,0xfe);
				if(maxDis>99)
				{
					displaySMG(5,SMG_number[maxDis/100]);
				}
				if(maxDis>9)
				{
					displaySMG(6,SMG_number[(maxDis%100)/10]);
				}
				displaySMG(7,SMG_number[maxDis%10]);
			}
			else if(juli_mode==3)
			{
				displaySMG(1,SMG_number[16]);
				if(aveDis>999)
				{
					displaySMG(4,SMG_number[aveDis/1000]);
				}
				if(aveDis>99)
				{
					displaySMG(5,SMG_number[(aveDis%1000)/100]);
				}

				displaySMG(6,SMG_Dotnumber[(aveDis%100)/10]);
				displaySMG(7,SMG_number[aveDis%10]);
			}
			else if(juli_mode==2)
			{
				displaySMG(1,0xf7);
				if(minDis>99)
				{
					displaySMG(5,SMG_number[minDis/100]);
				}
				if(minDis>9)
				{
					displaySMG(6,SMG_number[(minDis%100)/10]);
				}
				displaySMG(7,SMG_number[minDis%10]);
			}
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
			displaySMG(6,SMG_number[distance_canshu/10]);
			displaySMG(7,SMG_number[distance_canshu%10]);
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
			else if(big_mode==2)
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
	if(C2==0)//S8
	{
		delay(100);
		if(C2==0)
		{
			if(big_mode==1)
			{
				if(xianshi_mode==2)
				{
					if(ceju_mode==1)
					{
						ceju_mode=2;
					}
					else
					{
						ceju_mode=1;
					}
				}
				else if(xianshi_mode==3)
				{
					if(juli_mode==3)
					{
						juli_mode=1;
					}
					else
					{
						juli_mode++;
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
				if(xianshi_mode==1)
				{
					xianshi_mode=2;
				}
				else if(xianshi_mode==2)
				{
					xianshi_mode=3;
					//ceju_mode=1;
				}
				else if(xianshi_mode==3)
				{
					xianshi_mode=1;
					juli_mode=1;
				}
			}
			else if(big_mode==2)
			{
				if(canshu_mode==1)
				{
					canshu_mode=2;
				}
				else 
				{
					canshu_mode=1;
				}
			}
		}
		while(C1==0)
		{
			run();
		}
	}
	if(C2==0)//S9
	{
		delay(100);
		if(C2==0)
		{
			if(big_mode==2)
			{
				if(canshu_mode==1)
				{
					if(time_canshu==9)
					{
						time_canshu=2;
					}
					else if(time_canshu==2)
					{
						time_canshu=3;
					}
					else
					{
						time_canshu+=2;
					}
				}
				else if(canshu_mode==2)
				{
					if(distance_canshu==80)
					{
						distance_canshu=10;
					}
					else
					{
						distance_canshu+=10;
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
void read_Dis()
{
	unsigned char num=10;
	unsigned int ustime;
	TX=0;
	CL=0xf4;//12us
	CH=0xff;
	CR=1;
	while(num--)
	{
		while(!CF);
		CL=0xf4;
		CH=0xff;
		TX=~TX;
		CF=0;
	}
	CR=0;
	CL=0;
	CH=0;
	CR=1;
	while(RX&&(!CF));
	CR=0;
	if(CF==1)
	{
		CF=0;
		distance=0;
	}
	else
	{
		ustime=(CH<<8)+CL;
		distance=ustime*0.017;
	}
}
void getTime()
{
	read_time();
	second=(Timer[0]/16)*10+(Timer[0])%16;
	minute=(Timer[1]/16)*10+(Timer[1])%16;
	hour=(Timer[2]/16)*10+(Timer[2])%16;
}
void get_Dis()
{
	if(ceju_mode==1)
	{
		RB1=read_PCF(0x01);
		if(RB1>=50)
		{
			liangdu=1;
		}
		else
		{
			if(liangdu==1)
			{
				read_Dis();
				count_Dis++;
			}
			liangdu=0;
		}
	}
	else if(ceju_mode==2)
	{
		if(big_mode!=2)
		{
			if(((second%time_canshu)==0)&&(is_1s==1))
			{
				read_Dis();
				count_Dis++;
				if(count_tDis==3)
				{
					count_tDis=0;;
				}
				else
				{
					count_tDis++;
				}
				Dis_arr[count_tDis]=distance;	
				is_1s=0;
			}
			
		}
	}
	if(count_Dis==1)
	{
		maxDis=distance;
		minDis=distance;
		aveDis=distance*10;
	}
	else if(count_Dis>1)
	{
		if(maxDis<distance)
		{
			maxDis=distance;
		}
		if(minDis>distance)
		{
			minDis=distance;
		}
		aveDis=((aveDis*(count_Dis-1))+(distance*10))/count_Dis;
	}
}
void outDAC()
{
	if(distance>=80)
	{
		write_PCF(255);
	}
	else if(distance<=10)
	{
		write_PCF(51);
	}
	else
	{
		outV=((4*distance+30)/70)*51;
		write_PCF(outV);
	}
}
void showLED()
{
	HCSelect(4);
	if(big_mode==1)
	{
		if(xianshi_mode==1)
		{
			P0=0xfe;
		}
		else if(xianshi_mode==2)
		{
			P0=0xfd;
		}
		else if(xianshi_mode==3)
		{
			P0=0xfb;
		}
	}
	if(ceju_mode==1)
	{
		P0=(P0&0xf7);
	}
	else if(ceju_mode==2)
	{
		unsigned int lowD;
		unsigned int upD;
		lowD=distance-5;
		upD=distance+5;
		if(((Dis_arr[0]>=lowD)&&(Dis_arr[0]<=upD))&&((Dis_arr[1]>=lowD)&&(Dis_arr[1]<=upD))&&((Dis_arr[2]>=lowD)&&(Dis_arr[2]<=upD)))
		{
			P0=(P0&0xef);
		}
	}
	if(RB1>=50)
	{
		P0=(P0&0xdf);
	}
	delay(500);
}