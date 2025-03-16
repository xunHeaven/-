#include <STC15F2K60S2.H>
#include <onewire.h>
#include <iic.h>
#include <intrins.h>
sbit TX=P1^0;
sbit RX=P1^1;
sbit R1=P3^3;
sbit R2=P3^2;
sbit C1=P4^4;
sbit C2=P4^2;
sbit L1=P0^0;
sbit Relay=P0^4;

unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,
	0xc0,0x86,0x8e,0xbf,0x7f};//数码管段码，0-f和-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};	
unsigned int record_dis[14];
unsigned char big_mode=1;//1为测距界面，2为参数界面，3为工厂模式
unsigned char ceju_mode=1;//1为cm单位距离显示，2为m单位
unsigned char canshu_mode=1;//1为距离参数，2为温度参数
unsigned char gongchang_mode=1;//1为距离校准值，2为传输速度，3为DAC输出下限

double v;
int distance=0;//距离
unsigned char distance_canshu=40;//距离参数10到90cm
int distance_jiaozhun=0;//距离校准值-90到90cm
char pos_jiaozhun;
unsigned int temper;//温度
unsigned char temper_canshu=30;//0-80
unsigned int speed=340;//传播速度10m/s-9990m/s
float DAC_V=1.0;//DAC输出下限0.1-2
char DAC_V1;
unsigned char is_2s_changan=0;//S8和S9长按判断
unsigned char is_6s=0;//是否在6s状态内
unsigned char is_dac=0;//是否输出DAC电压，只在测距界面生效
unsigned char is_500ms=0;//500ms测试一次距离
unsigned char count_s1=0;
unsigned char count_s2=0;
unsigned char count_ms1=0;
unsigned char count_ms2=0;
unsigned char i=0;//数组下标
unsigned char is_getDis=0;//是否进行过采集
unsigned char is_L1=1;
unsigned char is_res=0;//是否同时按下了S8和S9
unsigned char is_2s=0;
void InitSys();
void InitTimer();
void HCSelect(unsigned char channel);
void dispalySMG(unsigned char pos,unsigned char value);
void showSMG();
void delay(unsigned int t);
void scanKey();
void offSMG();
int read_Dis();//超声波读取
void showLed();
void relay();
void outDAC();
void run();

void main()
{
	InitSys();
	InitTimer();
	while(1)
	{
		run();
		scanKey();
		showSMG();
		showLed();
		relay();
	}
}
void run()
{
	temper=read_temp();
		if(is_500ms==1)//0.5s读取一次
		{
			is_500ms=0;
			distance=read_Dis();
			if(is_6s==1)
			{
				record_dis[i]=distance;
				i++;
				if(i==13)
				{
					i=0;
				}
			}
			else
			{
				i=0;
			}
			
		}
	outDAC();
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
void ServiceT1() interrupt 3
{
	TL1=(65535-50000+1)%256;
	TH1=(65535-50000+1)/256;

	count_ms1++;
	count_ms2++;
	if(count_ms2==2)
	{
		count_ms2=0;
		if(is_L1==1)
		{
			is_L1=0;
		}
		else
		{
			is_L1=1;
		}
		
	}
	if(count_ms1==10)
	{
		count_ms1=0;
		is_500ms=1;

	}
	if(is_6s==1)
	{
		count_s1++;
		if(count_s1==120)
		{
			is_6s=0;
			count_s1=0;
			is_getDis=1;//采集完毕
		}
	}
	if(is_res==1)
	{
		count_s2++;
		if(count_s2==40)
		{
			is_2s=1;
		}
	}
	else
	{
		count_s2=0;
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
	HCSelect(5);
	P0=0x00;
	HCSelect(4);
	P0=0xff;
}
void displaySMG(unsigned char pos,unsigned char value)
{
	HCSelect(6);
	P0=0x01<<pos;
	HCSelect(7);
	P0=value;
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
int read_Dis()
{
	int distance;
	unsigned char num=10;
	TX=0;
	CL=0xf4;
	CH=0xff;
	CR=1;
	while(num--)
	{
		while(!CF);
		TX=~TX;
		CL=0xf4;
		CH=0xff;
		CF=0;
	}
	CR=0;
	CL=0;
	CH=0;
	CR=1;
	while(RX  && (!CF));
	CR=0;
	if(CF)
	{
		CF=0;
		distance=0;
	}
	else
	{
		distance=((CH<<8)+CL)*((speed/2)*0.0001)+distance_jiaozhun;
		if(distance<0)
		{
			distance=0;
		}
	}
	return distance;
	
}
void outDAC()
{
	unsigned char j;
	if((is_getDis==1)&&(is_6s==0)&&(is_dac==1)&&(big_mode==1))
	{
		is_dac=0;
		for(j=0;j<14;j++)
		{
			if(record_dis[j]<=10)
			{
				v=DAC_V*51.0;
			}
			else if(record_dis[j]>=90)
			{
				v=5*51.0;
			}
			else
			{
				v=((5-DAC_V)*(record_dis[j]/80)+((9*DAC_V)-5)/8)*51.0;
			}
		}
	}
}
void showSMG()
{
	if(big_mode==1)
	{
		displaySMG(0,SMG_number[temper/100]);
		delay(100);
		offSMG();
		displaySMG(1,SMG_Dotnumber[(temper%100)/10]);
		delay(100);
		offSMG();
		displaySMG(2,SMG_number[temper%10]);
		delay(100);
		offSMG();
		displaySMG(3,SMG_number[16]);
		delay(100);
		offSMG();
		if(ceju_mode==1)
		{
			if(distance>999)
			{
				displaySMG(4,SMG_number[distance/1000]);
				delay(100);
				offSMG();
			}
			if(distance >99)
			{
				displaySMG(5,SMG_number[(distance%1000)/100]);
				delay(100);
				offSMG();
			}
			if(distance>9)
			{
				displaySMG(6,SMG_number[(distance%100)/10]);
				delay(100);
				offSMG();
			}
			displaySMG(7,SMG_number[distance%10]);
			delay(100);
			offSMG();
		}
		else if(ceju_mode==2)
		{
			if(distance>999)
			{
				displaySMG(4,SMG_number[distance/1000]);
				delay(100);
				offSMG();
			}
			if(distance >99)
			{
				displaySMG(5,SMG_Dotnumber[(distance%1000)/100]);
				delay(100);
				offSMG();
			}
			if(distance<100)
			{
				displaySMG(5,SMG_Dotnumber[(distance%1000)/100]);
				delay(100);
				offSMG();
			}
			if(distance>9)
			{
				displaySMG(6,SMG_number[(distance%100)/10]);
				delay(100);
				offSMG();
			}
			displaySMG(7,SMG_number[distance%10]);
			delay(100);
			offSMG();
		}
	}
	else if(big_mode==2)
	{
		displaySMG(0,0x8c);//P
		delay(100);
		offSMG();
		if(canshu_mode==1)
		{
			displaySMG(1,SMG_number[1]);
			delay(100);
			offSMG();
			displaySMG(6,SMG_number[distance_canshu/10]);
			delay(100);
			offSMG();
			displaySMG(7,SMG_number[distance_canshu%10]);
			delay(100);
			offSMG();
		}
		else if(canshu_mode==2)
		{
			displaySMG(1,SMG_number[2]);
			delay(100);
			offSMG();
			displaySMG(6,SMG_number[temper_canshu/10]);
			delay(100);
			offSMG();
			displaySMG(7,SMG_number[temper_canshu%10]);
			delay(100);
			offSMG();
		}
	}
	else if(big_mode==3)
	{
		displaySMG(0,SMG_number[15]);
		delay(100);
		offSMG();
		if(gongchang_mode==1)
		{
			displaySMG(1,SMG_number[1]);
			delay(100);
			offSMG();
			if(distance_jiaozhun>=0)
			{
				if(distance_jiaozhun>9)
				{
					displaySMG(6,SMG_number[distance_jiaozhun/10]);
					delay(100);
					offSMG();
				}
				displaySMG(7,SMG_number[distance_jiaozhun%10]);
				delay(100);
				offSMG();
			}
			else if(distance_jiaozhun<0)
			{
				pos_jiaozhun=(-1)*distance_jiaozhun;
				if(pos_jiaozhun>9)
				{
					displaySMG(5,SMG_number[16]);
					delay(100);
					offSMG();
					displaySMG(6,SMG_number[pos_jiaozhun/10]);
					delay(100);
					offSMG();
					displaySMG(7,SMG_number[pos_jiaozhun%10]);
					delay(100);
					offSMG();
				}
				else
				{
					displaySMG(6,SMG_number[16]);
					delay(100);
					offSMG();
					displaySMG(7,SMG_number[pos_jiaozhun%10]);
					delay(100);
					offSMG();
				}
			}
		}
		else if(gongchang_mode==2)
		{
			displaySMG(1,SMG_number[2]);
			delay(100);
			offSMG();
			if(speed>999)
			{
				displaySMG(4,SMG_number[speed/1000]);
				delay(100);
				offSMG();
			}
			if(speed>99)
			{
				displaySMG(5,SMG_number[(speed%1000)/100]);
				delay(100);
				offSMG();
			}
			if(speed>9)
			{
				displaySMG(6,SMG_number[(speed%100)/10]);
				delay(100);
				offSMG();
			}
			displaySMG(7,SMG_number[speed%10]);
			delay(100);
			offSMG();
		}
		else if(gongchang_mode==3)
		{
			DAC_V1=DAC_V*10;
			displaySMG(1,SMG_number[3]);
			delay(100);
			offSMG();
			displaySMG(6,SMG_Dotnumber[DAC_V1/10]);
			delay(100);
			offSMG();
			displaySMG(7,SMG_number[DAC_V1%10]);
			delay(100);
			offSMG();
			
		}
	}
}

void scanKey()
{
	if(is_6s==1)//在6s测距的时间内，其他按键失效
	{
		return;
	}
	C2=0;
	C1=R1=R2=1;
	if(R1==0)//S8
	{
		delay(100);
		if(R1==0)
		{
			if(R2==0)//S9
			{
				delay(100);
				if(R2==0)
				{
					is_res=1;
				}
				while((R1==0)&&(R2==0))
				{	
					run();
					showSMG();
					showLed();
					relay();
					if(is_2s==1)
					{
						temper_canshu=30;
						speed=340;
						DAC_V=1.0;
					}
				}
				is_res=0;
				is_2s=0;
			}
		}
	}
	
	R1=0;
	R2=1;
	C1=C2=1;
	if(C1==0)//S4
	{
		delay(100);
		if(C1==0)
		{
			if(big_mode==1)
			{
				big_mode=2;
				ceju_mode=1;
			}
			else if(big_mode==2)
			{
				big_mode=3;
				canshu_mode=1;
			}
			else if(big_mode==3)
			{
				big_mode=1;
				gongchang_mode=1;
			}
		}
		while(C1==0)
		{
			showSMG();
		}
	}
	else if(C2==0)//S8
	{
		delay(100);
		if(C2==0)
		{
			if(big_mode==1)
			{
				is_6s=1;
			}
			else if(big_mode==2)
			{
				if(canshu_mode==1)//距离参数加10
				{
					if(distance_canshu==90)
					{
						distance_canshu=10;
					}
					else
					{
						distance_canshu+=10;
					}
				}
				else if(canshu_mode==2)//温度参数加1
				{
					if(temper_canshu==80)
					{
						temper_canshu=0;
					}
					else
					{
						temper_canshu++;
					}
				}
			}
			else if(big_mode==3)
			{
				if(gongchang_mode==1)
				{
					if(distance_jiaozhun==90)//距离校准值
					{
						distance_jiaozhun=-90;
					}
					else
					{
						distance_jiaozhun+=5;
					}
				}
				else if(gongchang_mode==2)//速度
				{
					if(speed==9990)
					{
						speed=10;
					}
					else
					{
						speed+=10;
					}
				}
				else if(gongchang_mode==3)//DAC输出电压
				{
					if(DAC_V==2)
					{
						DAC_V=0.1;
					}
					else
					{
						DAC_V+=0.1;
					}
				}
			}
		}
		while(C2==0)
		{
			showSMG();
		}
	}
	
	R1=1;
	R2=0;
	C1=C2=1;
	if(C1==0)//S5
	{
		delay(100);
		if(C1==0)
		{
			if(big_mode==1)
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
			else if(big_mode==3)
			{
				if(gongchang_mode==3)
				{
					gongchang_mode=1;
				}
				else
				{
					gongchang_mode++;
				}
			}
		}
		while(C1==0)
		{
			showSMG();
		}
	}
	else if(C2==0)//S9
	{
		delay(100);
		if(C2==0)
		{
			if(big_mode==1)
			{
				is_dac=1;
			}
			else if(big_mode==2)
			{
				if(canshu_mode==1)
				{
					if(distance_canshu==10)
					{
						distance_canshu=90;
					}
					else
					{
						distance_canshu-=10;
					}
				}
				else if(canshu_mode==2)
				{
					if(temper_canshu==0)
					{
						temper_canshu=80;
					}
					else
					{
						temper_canshu-=1;
					}
				}
			}
			else if(big_mode==3)
			{
				if(gongchang_mode==1)
				{
					if(distance_jiaozhun==-90)
					{
						distance_jiaozhun=90;
					}	
					else
					{
						distance_jiaozhun-=5;
					}
				}
				else if(gongchang_mode==2)
				{
					if(speed==10)
					{
						speed=9990;
					}
					else
					{
						speed-=10;
					}
				}
				else if(gongchang_mode==3)
				{
					if(DAC_V==0.1)
					{
						DAC_V=2.0;
					}
					else
					{
						DAC_V-=0.1;
					}
				}
			}
			while(C2==0)
			{
				showSMG();
			}
		}
	}
}
void showLed()
{
	HCSelect(4);
	P0=0xff;
	if(big_mode==1)
	{
		if(distance>=255)
		{
			HCSelect(4);
			P0=0x00;
			
		}
		else
		{
			HCSelect(4);
			P0=~distance;
		}
		delay(500);
	}
	else if(big_mode==3)
	{
		HCSelect(4);
		if(is_L1==0)
		{
			L1=0;
		}
		else
		{
			L1=1;
		}
		delay(100);
	}
}
void relay()
{
	if((distance_canshu-5<=distance)&&(distance_canshu+5>=distance)&&(temper<=temper_canshu))
	{
		HCSelect(5);
		P0=0x10;
		
	}
	else
	{
		HCSelect(5);
		P0=0x00;
	}
}