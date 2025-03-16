#include <STC15F2K60S2.H>
#include <stdio.h>
unsigned char command=0x00;//�յ�����λ��������

void HCSelect(unsigned char channel);
void InitSys();
void InitUart();
void sendByte(unsigned char dat);//���ڷ����ַ�����
void sendString(unsigned char *str);//���ڷ����ַ���
void Working();
void main()
{
	InitSys();
	InitUart();
	sendString("Welcome to XMF system!\r\n");//��\r����ʾ�س��������֮ǰ�����ݣ���\n����ʾ����
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
	TMOD=0x20;//��ʱ��1��8λ������
	TH1=0xfd;//������Ϊ9600
	TL1=0xfd;
	TR1=1;//�򿪶�ʱ��1�������ʿ�ʼ����
	
	SCON=0x50;//ģʽ1��8λ��urģʽ�����������
	AUXR=0x00;
	
	ES=1;
	EA=1;
}
void ServiceUart() interrupt 4
{
	if(RI==1)//��Ƭ�����յ��������ݣ������жϡ�������ɵĴ����ò�ѯ����
	{
		command=SBUF;
		RI=0;
	}
}
void sendByte(unsigned char dat)
{
	SBUF=dat;
	while(TI==0);//���������ϣ���TI==1������һֱѭ��
	TI=0;//��TI��0
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
		switch(command & 0xf0)//����λ��ʾ��������
		{
			case 0xa0:
				P0=(P0 | 0x0f)&((~command)| 0xf0);//ʹ��P0�ĸ���λ���䣬����λΪcommand����λȡ����ֵ
				command=0x00;
				break;
			case 0xb0:
				P0=(P0 | 0xf0)&(((~command)<<4)| 0x0f);//ʹ��P0�ĵ���λ���䣬����λΪcommand����λȡ����ֵ
				command=0x00;
				break;
			case 0xc0:
				sendString("This System is running!\r\n");
				command=0x00;
				break;
		}
	}
}