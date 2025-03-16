#include <STC15F2K60S2.H>
#include <stdio.h>
unsigned char command=0x00;//收到的上位机的命令

void HCSelect(unsigned char channel);
void InitSys();
void InitUart();
void sendByte(unsigned char dat);//串口发送字符函数
void sendString(unsigned char *str);//串口发送字符串
void Working();
void main()
{
	InitSys();
	InitUart();
	sendString("Welcome to XMF system!\r\n");//“\r”表示回车，会清除之前的内容，“\n”表示换行
	HCSelect(4);
	while(1)
	{
		Working();
	}
}

void HCSelect(unsigned char channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&&0x1f)| 0x80;
			break;
		case 5:
			P2=(P2&&0x1f)| 0xa0;
			break;
		case 6:
			P2=(P2&&0x1f)| 0xc0;
			break;
		case 7:
			P2=(P2&&0x1f)| 0xe0;
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
void InitUart()
{
	TMOD=0x20;//定时器1，8位重载载
	TH1=0xfd;//波特率为9600
	TL1=0xfd;
	TR1=1;//打开定时器1，波特率开始产生
	
	SCON=0x50;//模式1（8位的ur模式），允许接收
	AUXR=0x00;
	
	ES=1;
	EA=1;
}
void ServiceUart() interrupt 4
{
	if(RI==1)//单片机接收到完整数据，进入中断。发送完成的处理用查询来做
	{
		command=SBUF;
		RI=0;
	}
}
void sendByte(unsigned char dat)
{
	SBUF=dat;
	while(TI==0);//如果发送完毕，则TI==1，否则一直循环
	TI=0;//将TI置0
}
void sendString(unsigned char *str)
{
	while(*str != '\0')
	{
		sendByte(*str++);
	}
}
void Working()
{
	if(command != 0x00)
	{
		switch(command & 0xf0)//高四位表示命令类型
		{
			case 0xa0:
				P0=(P0 | 0x0f)&((~command)| 0xf0);//使得P0的高四位不变，低四位为command低四位取反的值
				command=0x00;
				break;
			case 0xb0:
				P0=(P0 | 0xf0)&(((~command)<<4)| 0x0f);//使得P0的低四位不变，高四位为command低四位取反的值
				command=0x00;
				break;
			case 0xc0:
				sendString("This System is running!\r\n");
				command=0x00;
				break;
		}
	}
}