#include <STC15F2K60S2.H>
#include <iic.h>
#include <ds1302.h>
#include <onewire.h>
#include <intrins.h>
sbit R1=P3^3;
sbit R2=P3^2;
sbit C1=P4^4;
sbit C2=P4^2;
sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;
sbit L4=P0^3;
sbit L5=P0^4;
sbit L6=P0^5;
unsigned int S9_t=0;//S8长按标记
unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,
	0xc0,0x86,0x8e,0xbf,0x7f};//数码管段码，0-f和-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};	
//U-0xc1
unsigned int count_t=0;//计时
unsigned int count_f=0;//频率计数
unsigned int dat_f=0;//频率记录
unsigned int S9_count=0;
unsigned char temp_arr[20]={0};//保存的温度数组
unsigned char maxTemp=0;//数组中的最大温度
unsigned int aveTemp=0;//平均温度
unsigned char giveTemp=30;//给定的温度参数

unsigned int shidu_arr[20]={0};//保存的湿度数组
unsigned int maxShidu=0;//平均湿度
unsigned int aveShidu=0;//最大湿度

unsigned char hour=0;//记录最近一次采集的时间（时）
unsigned char minute=0;//记录最近采集的时间（分）


unsigned char SMG_mode=1;//数码管模式
unsigned char huixian=1;//回显模式
unsigned char caiji=0;//采集次数
unsigned char v1;//光敏电阻采集到的分压
unsigned char v1_change=0;//从亮变暗标记
unsigned char v1_t=0;//亮变暗计时
unsigned char v1_liang=1;//暗变亮标记

unsigned char L4_key=0;
void Initsys();
void SelectHC(unsigned char channel);
void displaySMG(unsigned char pos,unsigned char value);
void delaySMG(unsigned int t);
void offSMG();
void getCaiji();
void showSMG();
void showLED();
void scanKey();
void InitTimer();
void getMaxAve();//计算温度和湿度数组的最大值和平均值

void main()
{
	Initsys();
	InitTimer();
	write_date();
	temp_arr[0]=read_Temp();
	temp_arr[0]=0;
	while(1)
	{
		v1=read_PCF(0x01);//读取光敏电阻v1电压
		read_date();//读取时间，并将数据赋给Timer[]
		getCaiji();
		getMaxAve();
		scanKey();
		showSMG();
		showLED();
		
	}
}

void InitTimer()
{
	TMOD = 0x16;  //设置定时器为模式0(16位自动重装载)

	TL0=0xff;
	TH0=0xff;
    TL1 = (65535-50000+1)%256;                     //初始化计时值
    TH1 = (65535-50000+1)/256;
	
	TR0=1;
	ET0=1;
    TR1 = 1;                        //定时器1开始计时
    ET1 = 1;                        //使能定时器0中断
    EA = 1;
}
void ServiceTimer0() interrupt 1
{
	count_f++;
}
void ServiceTimer1() interrupt 3
{
	TL1 = (65535-50000+1)%256;                     //初始化计时值
    TH1 = (65535-50000+1)/256;
	count_t++;
	if(count_t==20)//1S
	{
		dat_f=count_f;
		count_t=0;
		count_f=0;
	}
	if(S9_t==1)
	{
		S9_count++;
		if(S9_count==40)//2s
		{
			caiji=0;
			S9_t=0;
			S9_count=0;
		}
	}
	if(v1_change==1)
	{
		v1_t++;
		if(v1_t==60)//3s
		{
			v1_change=0;
			v1_t=0;
		}
	}
	if(count_t%2==0)
	{
		if(L4_key==0)
		{
			L4_key=1;
		}
		else
		{
			L4_key=0;
		}
	}
	
}

void getCaiji()
{
	if(v1_change==0)
	{
		if((v1<=50)&&(v1_liang==1))
		{
			delaySMG(100);
			if((v1<=50)&&v1_liang==1)
			{
				v1_change=1;
				v1_liang=0;
				caiji++;
				if((dat_f>=200)&&(dat_f<=2000))
				{
					shidu_arr[caiji-1]=(2*(dat_f/45))+10-(80/9);
				}
				temp_arr[caiji-1]=read_Temp();
				hour=Timer[2];
				minute=Timer[1];
			}

		}
	}
	if((v1_change==0)&&(v1>50))
	{
		v1_liang=1;
	}

}
void getMaxAve()
{
	unsigned char n;
	maxTemp=0;
	maxShidu=0;
	aveTemp=0;
	aveShidu=0;
	for(n=0;n<caiji;n++)
	{
		aveTemp+=temp_arr[n];
		aveShidu+=shidu_arr[n];
		if(maxTemp<temp_arr[n])
		{
			maxTemp=temp_arr[n];
		}
		if(maxShidu<shidu_arr[n])
		{
			maxShidu=shidu_arr[n];
		}

	}
	if(caiji!=0)
	{
		aveTemp=(aveTemp*10)/caiji;
		aveShidu=(aveShidu*10)/caiji;
	}
	else
	{
		aveTemp=(aveTemp*10);
		aveShidu=(aveShidu*10);
	}

}
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
void Initsys()
{
	SelectHC(5);
	P0=0xaf;
	SelectHC(4);
	P0=0xff;
}
void offSMG()
{
	SelectHC(6);
	P0=0x00;
	SelectHC(7);
	P0=0xff;
}
void displaySMG(unsigned char pos,unsigned char value)
{
	SelectHC(6);
	P0=0x01<<pos;
	SelectHC(7);
	P0=value;
}
void delaySMG(unsigned int t)
{
	while(t--);
}
void showSMG()
{
	if(v1_change==1)//光线由亮变暗，则显示温度湿度界面
	{
		displaySMG(0,0x86);//显示E
		delaySMG(100);
		displaySMG(3,SMG_number[temp_arr[caiji-1]/10]);//显示E
		delaySMG(100);
		displaySMG(4,SMG_number[temp_arr[caiji-1]%10]);//显示E
		delaySMG(100);
		
		displaySMG(5,SMG_number[16]);//显示E
		delaySMG(100);
		if(shidu_arr[caiji-1]==0)
		{
			displaySMG(6,SMG_number[10]);//显示A
			delaySMG(100);
			displaySMG(7,SMG_number[10]);//显示A
			delaySMG(100);
		}
		else
		{
			displaySMG(6,SMG_number[shidu_arr[caiji-1]/10]);
			delaySMG(100);
			displaySMG(7,SMG_number[shidu_arr[caiji-1]%10]);
			delaySMG(100);
		}
		offSMG();
	}
	else
	{
		if(SMG_mode==1)//模式1，显示时间，时-分-秒
		{
			displaySMG(0,SMG_number[Timer[2]/16]);
			delaySMG(100);
			displaySMG(1,SMG_number[Timer[2]%16]);
			delaySMG(100);
			
			displaySMG(2,SMG_number[16]);
			delaySMG(100);
	
			displaySMG(3,SMG_number[Timer[1]/16]);
			delaySMG(100);
			displaySMG(4,SMG_number[Timer[1]%16]);
			delaySMG(100);
			
			displaySMG(5,SMG_number[16]);
			delaySMG(100);
			
			displaySMG(6,SMG_number[Timer[0]/16]);
			delaySMG(100);
			displaySMG(7,SMG_number[Timer[0]%16]);
			delaySMG(100);
			offSMG();
		}
		else if(SMG_mode==2)//模式2，回显模式
		{
			if(huixian==1)//回显模式1，温度回显
			{
				displaySMG(0,SMG_number[12]);
				delaySMG(100);
				
				displaySMG(2,SMG_number[maxTemp/10]);
				delaySMG(100);
				displaySMG(3,SMG_number[maxTemp%10]);
				delaySMG(100);
				
				displaySMG(4,SMG_number[16]);
				delaySMG(100);
				
				displaySMG(5,SMG_number[aveTemp/100]);
				delaySMG(100);
				displaySMG(6,SMG_Dotnumber[(aveTemp%100)/10]);
				delaySMG(100);
				displaySMG(7,SMG_number[aveTemp%10]);
				delaySMG(100);
				offSMG();
			}
			else if(huixian==2)//回显模式2，湿度
			{
				displaySMG(0,0x89);//H
				delaySMG(100);
				
				displaySMG(2,SMG_number[maxShidu/10]);
				delaySMG(100);
				displaySMG(3,SMG_number[maxShidu%10]);
				delaySMG(100);
				
				displaySMG(4,SMG_number[16]);
				delaySMG(100);
				
				displaySMG(5,SMG_number[aveShidu/100]);
				delaySMG(100);
				displaySMG(6,SMG_Dotnumber[(aveShidu%100)/10]);
				delaySMG(100);
				displaySMG(7,SMG_number[aveShidu%10]);
				delaySMG(100);
				offSMG();
			}
			else if(huixian==3)//回显模式3，时间
			{
			
				displaySMG(0,SMG_number[15]);//F
				delaySMG(100);
				
				displaySMG(1,SMG_number[caiji/10]);
				delaySMG(100);
				displaySMG(2,SMG_number[caiji%10]);
				delaySMG(100);
	
				displaySMG(3,SMG_number[hour/16]);
				delaySMG(100);
				displaySMG(4,SMG_number[hour%16]);
				delaySMG(100);
			
				displaySMG(5,SMG_number[16]);
				delaySMG(100);
			
				displaySMG(6,SMG_number[minute/16]);
				delaySMG(100);
				displaySMG(7,SMG_number[minute%16]);
				delaySMG(100);
				offSMG();
			}
		}
		else if(SMG_mode==3)
		{
			displaySMG(0,0x8c);//P
			delaySMG(100);
			
			displaySMG(6,SMG_number[giveTemp/10]);
			delaySMG(100);
			displaySMG(7,SMG_number[giveTemp%10]);
			delaySMG(100);
			offSMG();
		}

			
	}
}
void scanKey()
{
	R1=0;
	R2=1;
	C1=C2=1;
	if((R1==0)&&(C1==0))//S4
	{
		delaySMG(200);
		if(C1==0)
		{
			if(SMG_mode==3)
			{
				SMG_mode=1;
				huixian=1;
			}
			else
			{
				SMG_mode++;
			}

		}
		while(C1==0)
		{
			showSMG();
			showLED();
		}
	}
	else if((R1==0)&&(C2==0))//S8
	{
		delaySMG(200);
		if(C2==0)
		{
			if(SMG_mode==3)
			{
				if(giveTemp==99)
				{
					giveTemp=0;
				}
				else
				{
					giveTemp++;
				}
			}
		}
		while(C2==0)
		{
			showSMG();
			showLED();
		}
	}
	
	R2=0;
	R1=1;
	C1=C2=1;
	if((R2==0)&&(C1==0))//S5
	{
		delaySMG(200);
		if(C1==0)
		{
			if(SMG_mode==2)
			{
				if(huixian==3)
				{
					huixian=1;
				}
				else
				{
					huixian++;
				}
			}
		}
		while(C1==0)
		{
			showSMG();
			showLED();
		}
	}
	else if((R2==0)&&(C2==0))//S9
	{
		delaySMG(100);
		if(C2==0)
		{
			if(SMG_mode==3)
			{
				if(giveTemp==0)
				{
					giveTemp=99;
				}
				else
				{
					giveTemp--;
				}
			}
			if((SMG_mode==2)&&(huixian==3))
			{
				S9_t=1;
			
			}
		}
		while(C2==0)
		{
			showSMG();
			showLED();
		}
		S9_t=0;
	}
}
void showLED()
{
	if((SMG_mode==1)&&(v1_change==0))
	{
		SelectHC(4);
		L1=0;
	}
	else if((SMG_mode==2)&&(v1_change==0))
	{
		SelectHC(4);
		L2=0;
	}
	if(v1_change==1)
	{
		SelectHC(4);
		L3=0;
	}
	if(caiji!=0)
	{
		if((temp_arr[caiji-1]>giveTemp)&&(L4_key==1))
		{
			SelectHC(4);
			L4=0;
		}
		else
		{
			SelectHC(4);
			L4=1;
		}
		if(shidu_arr[caiji-1]==0)
		{
			SelectHC(4);
			L5=0;
		}
		
	}
	if(caiji>=2)
	{
		if((temp_arr[caiji-1]>temp_arr[caiji-2])&&(shidu_arr[caiji-1]>shidu_arr[caiji-2]))
		{
			SelectHC(4);
			L6=0;
		}
	}
	delaySMG(100);
	SelectHC(4);
	P0=0xff;
}