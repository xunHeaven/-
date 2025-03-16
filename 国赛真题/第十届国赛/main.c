#include <STC15F2K60S2.H>
#include <intrins.h>
#include <iic.h>
#include <onewire.h>
#include <stdio.h>
#include <string.h>
sbit TX=P1^0;
sbit RX=P1^1;
sbit C1=P3^5;
sbit C2=P3^4;
sbit R1=P3^3;
sbit R2=P3^2;

unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,
	0xc0,0x86,0x8e,0xbf,0x7f};//数码管段码，0-f和-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};	
//U-0xc1
unsigned char big_mode=1;//1-显示，2-参数
unsigned char xianshi_mode=1;//1-温度，2-距离，3-变更次数
unsigned char canshu_mode=1;//1-温度参数，2-距离参数
unsigned int temper;//温度
unsigned int distance;
unsigned int change_num;//变更次数
unsigned int distance_canshu=35;//0-99
unsigned int temper_canshu=30;//0-99
unsigned int old_distance_canshu=35;
unsigned int old_temper_canshu=30;
unsigned char S12_key=1;//S12按下为0，否则为1
unsigned char S13_key=1;
unsigned char is_1s1=0;//S12按下是否有1s
unsigned char is_1s2=0;//按下S13是否有1s
unsigned char count_s1=0;
unsigned char count_s2=0;
unsigned char is_DAC=1;
unsigned char old_high=0;
unsigned char count_s3=0;
unsigned char is_ms=0;//500ms读取一次
unsigned char command[10]={0};
unsigned char zifushu=0;//接收到的字符串长度
unsigned char is_flag=0;
unsigned char R_dat;

void HC(unsigned char channel);
void InitSys();
void delay(unsigned int t);
void displaySMG(unsigned char pos,unsigned char value);
void offSMG();
void showSMG();
void run();
void showLED();
void scanKey();
void InitTimer();
void read_Dis();
void read_data();
void InitUart();
void sendByte(unsigned char dat);
void sendString(unsigned char *str);
void workUart();
char putchar(char ch);
void main()
{
	InitSys();
	InitTimer();
	InitUart();
	while(1)
	{
		run();
		workUart();
		scanKey();
	}
}
void run()
{
	read_data();
	//workUart();
	showSMG();
	showLED();
	
}
void InitUart()
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x8F;			//设置定时初始值
	T2H = 0xFD;			//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
	ES = 1;				//使能串口1中断
	EA=1;
}
void sendByte(unsigned char dat)
{
	SBUF=dat;
	while(TI==0);
	TI=0;
}
void sendString(unsigned char *str)
{
	while(*str!='\0')
	{
		sendByte(*str++);
	}
}
void ServiceUart() interrupt 4
{
	/*if(RI==1)
	{
		R_dat=SBUF;
		if((count_char<6)&&(is_flag==0))
		{
			command[count_char]=R_dat;
			count_char++;
		}
		else
		{
			count_char=0;
			is_flag=1;
		}
		
		RI=0;
		
	}*/
	if (RI)				//检测串口1接收中断
	{
		command[zifushu] = SBUF;
		zifushu++;
		RI = 0;			//清除串口1接收中断请求位
	}
}
char putchar(char ch){//重定向
	sendByte(ch);
	return ch;
}
void workUart()
{
	/*if(is_flag==1)
	{
		is_flag=0;
		printf("ERROR\r\n");
	}
	else
	{
		if((command[0]=='S')&&(command[1]=='T')&&(command[2]=='\r')&&(command[3]=='\n'))
		{
			printf("$%d,%4.2f\r\n",(int)distance,(float)(temper/100));
		}
		else if((command[0]=='P')&&(command[1]=='A')&&(command[2]=='R')&&(command[3]=='A'))
		{
			printf("#%d,%d",(int)old_distance_canshu,(int)old_temper_canshu);
		}
		else
		{
			printf("ERROR\r\n");
		}
	}*/
	unsigned char i;
	if(command[zifushu - 1] == '\n'){
		if((command[0] == 'S') && (command[1] == 'T') && (command[2] == '\r') && (command[3] == '\n')){
			printf("$%d,%d.%d",(int)(distance),(int)(temper/ 100),(int)(temper% 100));
		}else if((command[0] == 'P') && (command[1] == 'A') && (command[2] == 'R') && (command[3] == 'A')){
			printf("#%d,%d\r\n",(int)(old_distance_canshu),(int)(old_temper_canshu));
		}else{
			printf("ERROR\r\n");
		}
		zifushu = 0;
		for(i = 0;i < 10;i++){
			command[i] = 0;
		}
	}

}
void InitTimer()
{
	TMOD=0x01;
	TH0=(65535-50000+1)/256;
	TL0=(65535-50000+1)%256;
	TR0=1;
	ET0=1;
	EA=1;
}
void ServiceTimer() interrupt 1
{
	TH0=(65535-50000+1)/256;
	TL0=(65535-50000+1)%256;
	if(S12_key==0)
	{
		count_s1++;
		if(count_s1==20)
		{
			is_1s1=1;
			count_s1=0;
		}
	}
	if(S13_key==0)
	{
		count_s2++;
		if(count_s2==20)
		{
			is_1s2=1;
			count_s2=0;
		}
	}
/*	if(is_1s1==1)
	{
		count_s1=0;
	}
	if(is_1s2==1)
	{
		count_s2=0;
	}*/
	count_s3++;
	if(count_s3==10)
	{
		is_ms=1;
		count_s3=0;
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
void showSMG()
{
	if(big_mode==1)
	{
		if(xianshi_mode==1)
		{
			displaySMG(0,SMG_number[12]);
			displaySMG(4,SMG_number[temper/1000]);
			displaySMG(5,SMG_Dotnumber[(temper%1000)/100]);
			displaySMG(6,SMG_number[(temper%100)/10]);
			displaySMG(7,SMG_number[temper%10]);
		}
		else if(xianshi_mode==2)
		{
			displaySMG(0,0xc7);
			displaySMG(6,SMG_number[distance/10]);
			displaySMG(7,SMG_number[distance%10]);
		}
		else if(xianshi_mode==3)
		{
			displaySMG(0,0xc8);
			if(change_num>9999)
			{
				displaySMG(3,SMG_number[change_num/10000]);
			}
			if(change_num>999)
			{
				displaySMG(4,SMG_number[(change_num%10000)/1000]);
			}
			if(change_num>99)
			{
				displaySMG(5,SMG_number[(change_num%1000)/100]);
			}
			if(change_num>9)
			{
				displaySMG(6,SMG_number[(change_num%100)/10]);
			}
			displaySMG(7,SMG_number[change_num%10]);
		}
	}
	else if(big_mode==2)
	{
		displaySMG(0,0x8c);
		if(canshu_mode==1)
		{
			displaySMG(3,SMG_number[1]);
			displaySMG(6,SMG_number[temper_canshu/10]);
			displaySMG(7,SMG_number[temper_canshu%10]);
		}
		else if(canshu_mode==2)
		{
			displaySMG(3,SMG_number[2]);
			displaySMG(6,SMG_number[distance_canshu/10]);
			displaySMG(7,SMG_number[distance_canshu%10]);
		}
	}
}	
void scanKey()
{
	R1=0;
	R2=C1=C2=1;
	if(C1==0)//S12
	{
		delay(100);
		if(C1==0)
		{
			S12_key=0;
			while(C1==0)
			{
				run();
			}
			S12_key=1;
			count_s1=0;
			if(is_1s1==1)
			{
				is_1s1=0;
				change_num=0;
				write_AT(0x00,0);
				write_AT(0x01,0);
			}
			else
			{
				is_1s1=0;
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
					}
					else
					{
						canshu_mode=1;
					}
				}
			}
		}
	}
	if(C2==0)//S16
	{
		delay(100);
		if(C2==0)
		{
			while(C2==0)
			{
				run();
			}
			if(big_mode==2)
			{
				if(canshu_mode==1)
				{
					if(temper_canshu<2)
					{
						temper_canshu=99;
					}
					else
					{
						temper_canshu-=2;
					}
				}
				else if(canshu_mode==2)
				{
					if(distance_canshu<5)
					{
						distance_canshu=99;
					}
					else
					{
						distance_canshu-=5;
					}
				}
			}
		}
	}
	
	R2=0;
	R1=C1=C2=1;
	if(C1==0)//S13
	{
		delay(100);
		if(C1==0)
		{
			S13_key=0;
			while(C1==0)
			{
				run();
			}
			S13_key=1;
			count_s2=0;
			if(is_1s2==1)
			{
				is_1s2=0;
				if(is_DAC==0)
				{
					is_DAC=1;
				}
				else
				{
					is_DAC=0;
				}
			}
			else
			{
				is_1s2=0;
				if(big_mode==1)
				{
					big_mode=2;
					xianshi_mode=1;
				}
				else if(big_mode==2)
				{
					big_mode=1;
					canshu_mode=1;
					if((old_distance_canshu!=distance_canshu)|(old_temper_canshu!=temper_canshu))
					{
						unsigned int h_num;
						unsigned int l_num;
						unsigned char h_num1;
						unsigned char l_num1;
						
						change_num++;
						if(change_num>255)
						{
							h_num=change_num/256;
							h_num1=(unsigned char)h_num;
							if(old_high!=h_num1)
							{
								old_high=h_num1;
								write_AT(0x01,old_high);
							}
						}
						l_num=change_num%256;
						l_num1=(unsigned char)l_num;
						write_AT(0x00,l_num1);
					}
					old_distance_canshu=distance_canshu;
					old_temper_canshu=temper_canshu;
				}
			}
		}

	}
	if(C2==0)//S17
	{
		delay(100);
		if(C2==0)
		{
			while(C2==0)
			{
				run();
			}
			if(big_mode==2)
			{
				if(canshu_mode==1)
				{
					if(temper_canshu>97)
					{
						temper_canshu=0;
					}
					else
					{
						temper_canshu+=2;
					}
				}
				else if(canshu_mode==2)
				{
					if(distance_canshu>94)
					{
						distance_canshu=0;
					}
					else
					{
						distance_canshu+=5;
					}
				}
			}
		}
	}
}
void read_Dis()
{
	unsigned char num=10;
	unsigned int utime;
	//unsigned dis;
	TX=0;
	CL=0xf3;
	CH=0xff;
	CR=1;
	while(num--)
	{
		while(!CF);
		TX^=1;
		CL=0xf3;
		CH=0xff;
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
		distance=99;
	}
	else
	{
		utime=(CH*256)+CL;
		distance=utime*0.017;
	}

}
void read_data()
{
	temper=read_temp();
	if(is_ms==1)
	{
		read_Dis();
		is_ms=0;
	}
	if(is_DAC==1)
	{
		if(distance>distance_canshu)
		{
			write_PCF(204);
		}
		else
		{
			write_PCF(102);
		}
	}
	else
	{
		write_PCF(21);
	}
}
void showLED()
{
	HC(4);
	P0=0xff;
	if(temper>(old_temper_canshu*100))
	{
		P0=0xfe;
	}
	if(distance<old_distance_canshu)
	{
		P0=(P0&0xfd);
	}
	if(is_DAC==1)
	{
		P0=(P0&0xfb);
	}
	delay(500);
}