#include <STC15F2K60S2.H>
#include <intrins.h>
#include "iic.h"
sbit S7=P3^0;
sbit S6=P3^1;
sbit S5=P3^2;
sbit S4=P3^3;

unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
     0x80,0x90,0x88,0x83,0xc6,0xc0,0x86,0x8e,
	0xbf,0x7f};//����ܶ��룬0-f��-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
unsigned char SMG_mode=0;//�������ʾģʽ��0Ϊ��ѹ��ʾ���棬1ΪƵ����ʾ����
unsigned char LED_mode=0;//LED��ʾģʽ��0Ϊ��ѹģʽ��1ΪƵ��ģʽ
unsigned char S5_key=0;//��ѹ���ģʽת����0ʱ��ѹֵΪ2��1ʱ��ѹֵ����RB2�仯������S5ʵ��ģʽת��
unsigned char S6_key=0;//LED������ģʽ��0ʱ������1ʱ�رա�����S6ʵ��ģʽת��
unsigned char S7_key=0;//���������ģʽ��0ʱ������1ʱ�رա�����S7ʵ��ģʽת��

//����������Ϊ�˼���Ƶ��
unsigned int dat_f=0;//��ʾ��������е�Ƶ��ֵ��ÿ����㣻
unsigned int count_f=0;//�ڶ�ʱ��0�м���һ���Ƶ��
unsigned char count_t=0;//��¼ʱ�䣬һ������
//��������������Ϊ�˱�ʾ��ѹ
unsigned int v1=200;//��ʾ�������ʾ��ѹ����ʼ��Ϊ�̶���ѹ2
unsigned int v2;//RB2�еĵ�ѹ

//��������
void InitSys();//��ʼ����Ƭ�����ص��������̵������޹��豸
void SelectHC(unsigned char channel);//ѡ��HC573������
void display_SMG(unsigned char pos,unsigned char value);//ѡ�����������ܺ͵�������
void off_SMG();//�ص����������
void delaySMG(unsigned int t);//�������ʱ����
void show_SMG();//�������ʾ���ƺ���
void show_LED();//LED��ʾ����
void scanKey();//������������
void InitTimer();//��ʱ��������ʼ��
//void ServiceTimer0() interrupt 1;//��ʱ��0������Ƶ�ʼ���
//void ServiceTimer1() interrupt 3;//��ʱ��1�����ڼ�ʱ
 

void main()
{
	InitSys();
	InitTimer();
	while(1)
	{
		v2=((unsigned char)readPCF(0x03))*100/51;
		if(S5_key==0)
		{
			v1=200;
		}
		else
		{
			v1=v2;
		}
		scanKey();
		show_SMG();
		show_LED();
	}
}
void InitSys()
{
	SelectHC(4);
	P0=0xff;
	SelectHC(5);
	P0=P0&0xaf;
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
void display_SMG(unsigned char pos,unsigned char value)
{
	SelectHC(7);
	P0=0xff;
	SelectHC(6);
	P0=0x01<<pos;
	SelectHC(7);
	P0=value;
}
void off_SMG()
{
	SelectHC(6);
	P0=0xff;
	SelectHC(7);
	P0=0xff;
}
void delaySMG(unsigned int t)
{
	while(t--);
}
void show_SMG()
{
	if(S7_key==0)//���������
	{
		if(SMG_mode==0)//�������ʾ��ѹ
		{
			display_SMG(0,0xc1);
			delaySMG(100);
			display_SMG(5,SMG_Dotnumber[v1/100]);
			delaySMG(100);
			display_SMG(6,SMG_number[(v1%100)/10]);
			delaySMG(100);
			display_SMG(7,SMG_number[v1%10]);
			delaySMG(100);
			off_SMG();
		}
		else if(SMG_mode==1)//�������ʾƵ��
		{
			display_SMG(0,0x8e);
			delaySMG(100);
			if(dat_f>9999)
			{
				display_SMG(3,SMG_number[dat_f/10000]);
				delaySMG(100);
			}
			if(dat_f>999)
			{
				display_SMG(4,SMG_number[(dat_f/1000)%10]);
				delaySMG(100);
			}
			if(dat_f>99)
			{
				display_SMG(5,SMG_number[(dat_f/100)%10]);
				delaySMG(100);
			}
			if(dat_f>9)
			{
				display_SMG(6,SMG_number[(dat_f/10)%10]);
				delaySMG(100);
			}
			display_SMG(7,SMG_number[dat_f%10]);
			delaySMG(100);
			off_SMG();
		}
	}
	else//S7_key==1,����ܹر�
	{
		off_SMG();
	}
}

void scanKey()
{
	if(S7==0)
	{
		delaySMG(100);
		if(S7==0)
		{
			if(S7_key==0)
			{
				S7_key=1;
			}
			else
			{
				S7_key=0;
			}
		}
		while(S7==0)
		{
			show_SMG();
			show_LED();
		}
	}
	if(S6==0)
	{
		delaySMG(100);
		if(S6==0)
		{
			if(S6_key==0)
			{
				S6_key=1;
			}
			else
			{
				S6_key=0;
			}
		}
		while(S6==0)
		{
			show_SMG();
			show_LED();
		}
	}
	if(S5==0)
	{
		delaySMG(100);
		if(S5==0)
		{
			if(S5_key==0)
			{
				S5_key=1;
				v1=v2;
			}
			else
			{
				S5_key=0;
				v1=200;
			}
		}
		while(S5==0)
		{
			show_SMG();
			show_LED();
		}
	}
	if(S4==0)
	{
		delaySMG(100);
		if(S4==0)
		{
			if(SMG_mode==0)
			{
				SMG_mode=1;
				LED_mode=1;
			}
			else
			{
				SMG_mode=0;
				LED_mode=0;
			}
		}
		while(S4==0)
		{
			show_SMG();
			show_LED();
		}
	}
}
void show_LED()
{
	if(S6_key==0)
	{
		if(LED_mode==0)//��ѹ��ʾ״̬
		{
			SelectHC(4);
			P0=0xfe;//L1������L2Ϩ��
			if(S5_key==1)//��Rb2�����ĵ�ѹ����L5������
			{
				P0=P0&0xef;
				if((v1>=150&&v1<250)||(v1>=350))//L3����
				{
					P0=P0&0xfb;
				}
				else//L3Ϩ��
				{
					P0=P0|0x04;
				}
			}
			else//L3��L5Ϩ��
			{
				P0=P0|0x14;
			}
			
		}
		else//Ƶ����ʾ״̬
		{
			SelectHC(4);
			P0=0xfd;//L2������L1Ϩ��
			if((dat_f>=1000&&dat_f<5000)||(dat_f>=10000))//L4����
			{
				P0=P0&0xf7;
			}
			else//Ϩ��L4��ֻ����L2
			{
				P0=0xfd;
			}
		}
	}
	else
	{
		SelectHC(4);
		P0=0xff;
	}
}

void InitTimer()
{
	TMOD = 0x16;                    //��4Ϊ���ƶ�ʱ��1����ʾ16λ���Զ���װ�أ���4λ���ƶ�ʱ��0����ʾ8λ�Զ���װ��
    TL1 =(65535-50000+1)%256;	//��ʼ����ʱֵ
    TH1 = (65535-50000+1)/256;
	TL0=0xff;
	TH0=0xff;
	
	TR0=1;
	ET0=1;
    TR1 = 1;                        //��ʱ��1��ʼ��ʱ
    ET1 = 1;                        //ʹ�ܶ�ʱ��1�ж�
    EA = 1;
}

void ServiceTimer0() interrupt 1
{
	count_f++;
}

void ServiceTimer1() interrupt 3//50ms
{
	TL1 =(65535-50000+1)%256;	//��ʼ����ʱֵ
	TH1 = (65535-50000+1)/256;
	count_t++;
	if(count_t==20)
	{
		dat_f=count_f;
		count_f=0;
		count_t=0;
	}
}