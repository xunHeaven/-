#include <STC15F2K60S2.H>
#include <iic.h>
#include <onewire.h>
#include <ds1302.h>
#include <intrins.h>
sbit TX=P1^0;//TX=1发送超声波
sbit RX=P1^1;//RX=0收到超声波
sbit S4=P3^3;
unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,
	0xc0,0x86,0x8e,0xbf,0x7f};//数码管段码，0-f和-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};	

unsigned char mode=1;//1-时间，2-温度，3-电压，4-距离，5-频率，6-eeprom读取值
unsigned int temper;//温度
unsigned int v;//分压
unsigned int distance=0;//距离
unsigned int dat_f=0;//频率
unsigned int count_f=0;//频率计数
unsigned int count_t1=0;//频率计时
unsigned char e2promDat=1;//读取eeprom中的值
unsigned char is_ceju=0;



void HC(unsigned char channel);
void InitSys();
void displaySMG(unsigned char pos,unsigned char value);
void delay(unsigned int t);
void offSMG();
void showSMG();
void scanKey();
void getData();//读取各种数据的函数
unsigned int read_Dis();//测距函数
void InitTimer();
void main()
{
	InitSys();
	InitTimer();
	write_time();//时间初始化
	write_AT(0x01,126);//将126写入eeprom中1号地址
	while(1)
	{
		getData();
		scanKey();
	}
}
void InitTimer()
{
	TMOD=0x16;//定时器0为频率计数，定时器1为16为非重装载
	
	TL0=0xff;
	TH0=0xff;
	TL1=(65535-50000+1)%256;
	TH1=(65535-50000+1)/256;
	
	TR0=1;
	ET0=1;
	TR1=1;
	ET1=1;
	EA=1;
}
void serviceT0() interrupt 1
{
	count_f++;
}
void serviceT1() interrupt 3
{
	TL1=(65535-50000+1)%256;
	TH1=(65535-50000+1)/256;
	count_t1++;
	
	if(count_t1==20)
	{
		dat_f=count_f;
		count_f=0;
		count_t1=0;
		is_ceju=1;
	}
}
void getData()
{
	read_time();//读取时间
	temper=read_temp();//读取温度
	v=(read_PCF(0x03)*10)/51;//读取滑动变阻器分压,扩大10倍相当于保留1为小数
	if(is_ceju==1)//每秒测一次距离
	{
		is_ceju=0;
		distance=read_Dis();
	}  
	e2promDat=read_AT(0x01);//读取eeprom中1号单元的值
	showSMG();
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
	delay(500);
	offSMG();
}

void showSMG()//1-时间，2-温度，3-电压，4-距离，5-频率，6-eeprom读取值
{
	if(mode!=1)
	{
		displaySMG(0,SMG_number[15]);
		displaySMG(1,SMG_number[mode]);
	}

	if(mode==1)
	{
		displaySMG(0,SMG_number[Timer[2]/16]);
		displaySMG(1,SMG_number[Timer[2]%16]);
		
		displaySMG(2,SMG_number[16]);
		displaySMG(3,SMG_number[Timer[1]/16]);
		displaySMG(4,SMG_number[Timer[1]%16]);
		
		displaySMG(5,SMG_number[16]);
		displaySMG(6,SMG_number[Timer[0]/16]);
		displaySMG(7,SMG_number[Timer[0]%16]);
	}
	else if(mode==2)
	{
		displaySMG(5,SMG_number[temper/100]);
		displaySMG(6,SMG_Dotnumber[(temper%100)/10]);
		displaySMG(7,SMG_number[temper%10]);
	}
	else if(mode==3)
	{
		displaySMG(6,SMG_Dotnumber[v/10]);
		displaySMG(7,SMG_number[v%10]);
	}
	else if(mode==4)
	{
		displaySMG(5,SMG_number[distance/100]);
		displaySMG(6,SMG_number[(distance%100)/10]);
		displaySMG(7,SMG_number[distance%10]);
	}
	else if(mode==5)
	{
		if(dat_f>9999)
		{
			displaySMG(3,SMG_number[dat_f/10000]);
		}
		if(dat_f>999)
		{
			displaySMG(4,SMG_number[(dat_f%10000)/1000]);
		}
		if(dat_f>99)
		{
			displaySMG(5,SMG_number[(dat_f%1000)/100]);
		}
		if(dat_f>9)
		{
			displaySMG(6,SMG_number[(dat_f%100)/10]);
		}
		displaySMG(7,SMG_number[dat_f%10]);
	}
	else if(mode==6)
	{
		displaySMG(5,SMG_number[e2promDat/100]);
		displaySMG(6,SMG_number[(e2promDat%100)/10]);
		displaySMG(7,SMG_number[e2promDat%10]);
	}
}
void scanKey()
{
	if(S4==0)
	{
		delay(100);
		if(S4==0)
		{
			while(S4==0);
			{
				getData();
			}
			if(mode==6)
			{
				mode=1;
			}
			else
			{
				mode++;
			}
		}
	}
}
unsigned int read_Dis()
{
	unsigned char num=10;
	unsigned int utime;
	unsigned int dis;
	TX=0;
	CL=0xf3;//13us
	CH=0xff;
	CR=1;//开始计时
	while(num--)
	{
		while(CF==0);
		TX^=1;
		CL=0xf3;
		CH=0xff;
		CF=0;
	}
	CR=0;
	CL=0;
	CH=0;
	CR=1;
	while(RX&&(!CF));//RX=0表示收到数据
	CR=0;
	if(CF==1)
	{
		CF=0;
		dis=255;
	}
	else
	{
		utime=(CH<<8)+CL;
		dis=utime*0.017;
	}
	return dis;
}