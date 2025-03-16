#include <STC15F2K60S2.H>
#include <intrins.h>
#include <stdio.h>
//sbit TX=P1^0;
//sbit RX=P1^1;
unsigned char juli=150;
unsigned char temper=27;
unsigned char juli_canshu=100;
unsigned char temper_canshu=37;

unsigned char Rarr[20];
unsigned char Rindex=0;
void UartInit();
void sendString(unsigned char *str);
void sendByte(unsigned char dat);
char putchar(char ch);
void work();
void Delay100ms();
void main()
{
	UartInit();
	Delay100ms();
	sendString("HELLO\r\n");
	while(1)
	{
		work();
	}
}

void UartInit()
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR &= 0xFB;		//定时器2时钟为Fosc/12,即12T
	T2L = 0xCC;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	ES=1;
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
	while(*str !='\0')
	{
		sendByte(*str);
		str++;
	}
}

char putchar(char ch)
{
	SBUF=ch;
	while(TI==0);
	TI=0;
	return ch;
}
void serviceUart() interrupt 4
{
	if(RI==1)
	{
		Rarr[Rindex]=SBUF;
		Rindex++;
		RI=0;
		/*if(Rindex>10)
		{
			Rindex=0;
		}*/
	}
}
void work()
{
	if((Rindex!=0)&&(Rarr[Rindex-1]=='\n'))
	{
		unsigned char i;
		if((Rarr[0]=='S')&&(Rarr[1]=='T')&&(Rarr[2]=='\r')&&(Rarr[3]=='\n'))
		{
			printf("$%d,%d\r\n",(int)juli,(int)temper);
		}
		else if((Rarr[0]=='P')&&(Rarr[1]=='A')&&(Rarr[2]=='R')&&(Rarr[3]=='A'))
		{
			printf("#%d,%d\r\n",(int)juli_canshu,(int)temper_canshu);
		}
		else
		{
			printf("ERROR\r\n");
		}
		Rindex=0;
		for(i=0;i<20;i++)
		{
			Rarr[i]=0;
		}
	}
}

void Delay100ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 5;
	j = 144;
	k = 71;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
