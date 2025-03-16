#include <STC15F2K60S2.H>
#include <stdio.h>
#include <intrins.h>
#include <string.h>
unsigned char temper_canshu=30;
unsigned char distance_canshu=35;
unsigned char temper=28;
unsigned char distance=50;
unsigned char R_arr[20];
unsigned char Rindex;
unsigned char code order1[]="ST\r\n";
unsigned char code order2[]="PARA\r\n";

int res1=1;
int res2=1;

void UartInit();
void sendString(unsigned char *str);
void sendByte(unsigned char dat);
char putchar(char ch);
void work();
void Delay500ms();

void main()
{
	UartInit();
	Delay500ms();
	sendString("hELLO\r\n");
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

/*void sendByte(unsigned char dat)
{
	SBUF=dat;
	while(TI==0);//发送成功TI=1,跳出循环
	TI=0;
}*/
void sendString(unsigned char *str)
{
	while(*str !='\0')
	{
		SBUF=*str;
		while(TI==0);
		TI=0;
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
		R_arr[Rindex]=SBUF;
		
		Rindex++;
		RI=0;
		/*if(R_arr[Rindex]!='\n')
		{
			Rindex++;
		}
		else
		{
			Rindex=0;
		}*/
		
	}
}
void work()
{
	if((Rindex!=0)&&(R_arr[Rindex-1]=='\n'))
	{
		unsigned char i;
		//res1=strcmp(order1,R_arr);//两个字符串相等则为0；
		//res2=strcmp(order2,R_arr);
//		if(res1==0)
		if((R_arr[0]=='S')&&(R_arr[1]=='T')/*&&(R_arr[2]=='\r')&&(R_arr[3]=='\n')*/)
		{
			printf("$%d,%d\r\n",(int)distance,(int)temper);
		}
		else if((R_arr[0]=='P')&&(R_arr[1]=='A')&&(R_arr[2]=='R')&&(R_arr[3]=='A'))
		{
			printf("#%d,%d\r\n",(int)distance_canshu,(int)temper_canshu);
		}
		else
		{
			printf("ERROR\r\n");
		}
		Rindex=0;
		for(i=0;i<20;i++)
		{
			R_arr[i]=0;
		}
		
	}
}

void Delay500ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 23;
	j = 205;
	k = 120;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
