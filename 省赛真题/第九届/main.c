#include <STC15F2K60S2.H>
#include "iic.h"
#include <intrins.h>
sbit S4=P3^3;
sbit S5=P3^2;
sbit S6=P3^1;
sbit S7=P3^0;

unsigned char Led_mode=1;//Led����˸ģʽ1-4������ģʽ
unsigned char SMG_mode=1;//�������ʾģʽ1-3
unsigned char Led_lumi=1;//���ȵȼ���1-4,�ɻ�������������
unsigned char pwm_duty=10;//pwm���ȿ���
unsigned char countShine=0;//��ʱ��1��ʱ
unsigned char LedPwm=1;
unsigned char LEDCount=0;
unsigned char jiange=4;//LED����ģʽ��ת���4-12�����浽AT24C02
unsigned char S7_key=1;//S7�����жϣ��������S7��LED����ͣ��������1������
unsigned char getPCF=0;//��ȡPCF��AIN3�ĵ�ѹֵ����
unsigned char count_100ms=0;//100�������
unsigned char S4_key=0;//�����ȫ��״̬�£��Ƿ���S4
unsigned char is_800ms=0;//0.8s�������жϱ��
unsigned int code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
     0x80,0x90,0x88,0x83,0xc6,0xc0,0x86,0x8e,
	0xbf,0x7f};
unsigned char code SMG_Dot[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
unsigned char code LED_mode1[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//LED��ģʽһ��������
unsigned char code LED_mode2[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
unsigned char code LED_mode3[]={0x7e,0xbd,0xdb,0xe7};
unsigned char code LED_mode4[]={0xe7,0xdb,0xbd,0x7e};
void LED_show(unsigned char i,unsigned char mode);//LED��ʾ����
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
void InitSys()
{
	SelectHC(4);
	P0=0xff;//�ص�LED
	
	
	SelectHC(5);
	P0=P0&0xaf;//�ص��������̵���
	
	SelectHC(6);
	P0=0xff;
	SelectHC(7);
	P0=0xff;//�ص������
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

void all_SMG()//ʹ��ʾʱ�����������ͬ
{
	SelectHC(7);
	P0=0xff;
	SelectHC(6);
	P0=0xff;
	SelectHC(7);
	P0=0xff;
}
void display_LED(unsigned pos)//led���������
{
	P0=0xff;
	SelectHC(4);
	P0=pos;
}
void delaySMG(unsigned int t)
{
	while(t--);
}

void SMG_show(unsigned char mode)//�������ʾ����
{
	switch(mode)
	{
		case 1:
			if(S4_key==0)
			{	
				all_SMG();
			}
			else
			{
				display_SMG(6,SMG_number[16]);
				delaySMG(100);
				display_SMG(7,SMG_number[Led_lumi]);
				delaySMG(100);
				all_SMG();
				delaySMG(100);
			}
			break;
		case 2:   //����Led��ʾ��ģʽ
			if(is_800ms)//���is_800ms=1������ʾģʽ������ܵ�ǰ���ڵ���
			{
				display_SMG(0,SMG_number[16]);
				delaySMG(100);
				display_SMG(1,SMG_number[Led_mode]);
				delaySMG(100);
				display_SMG(2,SMG_number[16]);
				delaySMG(100);
				delaySMG(100);
			}
			else//���is_800ms=0������ʾģʽ������ܵ�ǰ����Ϩ��
			{
				display_SMG(0,0xff);
				delaySMG(100);
				display_SMG(1,0xff);
				delaySMG(100);
				display_SMG(2,0xff);
				delaySMG(100);
				delaySMG(100);
			}
			if(jiange>=10)//�����תʱ�����1000ms
			{
				display_SMG(4,SMG_number[jiange/10]);
				delaySMG(100);
				display_SMG(5,SMG_number[jiange%10]);
				delaySMG(100);
				display_SMG(6,SMG_number[0]);
				delaySMG(100);
				display_SMG(7,SMG_number[0]);
				delaySMG(100);
				all_SMG();
				delaySMG(100);
			}
			else
			{
				display_SMG(5,SMG_number[jiange%10]);
				delaySMG(100);
				display_SMG(6,SMG_number[0]);
				delaySMG(100);
				display_SMG(7,SMG_number[0]);
				delaySMG(100);
				all_SMG();
				delaySMG(100);
			}
			break;
			
		case 3://����led��תʱ���ģʽ
			display_SMG(0,SMG_number[16]);
			delaySMG(100);
			display_SMG(1,SMG_number[Led_mode]);
			delaySMG(100);
			display_SMG(2,SMG_number[16]);
			delaySMG(100);
			if(is_800ms)
			{
				if(jiange>=10)//�����תʱ�����1000ms
				{
					display_SMG(4,SMG_number[jiange/10]);
					delaySMG(100);
					display_SMG(5,SMG_number[jiange%10]);
					delaySMG(100);
					display_SMG(6,SMG_number[0]);
					delaySMG(100);
					display_SMG(7,SMG_number[0]);
					delaySMG(100);
					all_SMG();
					delaySMG(100);
				}
				else
				{
					display_SMG(5,SMG_number[jiange%10]);
					delaySMG(100);
					display_SMG(6,SMG_number[0]);
					delaySMG(100);
					display_SMG(7,SMG_number[0]);
					delaySMG(100);
					all_SMG();
					delaySMG(100);
				}
			}
			else
			{
				display_SMG(4,0xff);
				delaySMG(100);
				display_SMG(5,0xff);
				delaySMG(100);
				display_SMG(6,0xff);
				delaySMG(100);
				display_SMG(7,0xff);
				delaySMG(100);
				all_SMG();
				delaySMG(100);
			}
			
			break;
	}
}
void keyCtrl()//��������
{
	if(S4==0)
	{
		delaySMG(100);
		if(S4==0)
		{
			if(SMG_mode==1)//���������ܵ�ģʽ1�����º��������ʾled����ģʽ
			{
				S4_key=1;
			}
			else if(SMG_mode==2)//����������ģʽ2�����Զ�LED��ʾģʽ��������
			{
				if(Led_mode==1)
				{
					Led_mode=4;
				}
				else
				{
					Led_mode--;
				}
			}
			else//����������ģʽ3�����LEDģʽ��תʱ���������
			{
				if(jiange==4)
				{
					jiange=12;
				}
				else
				{
					jiange--;
				}
			}
		}
		while(S4==0)
		{
			SMG_show(SMG_mode);
			LED_show(0,Led_mode);
		}
		S4_key=0;
	}
	
	if(S5==0)//����S5����������������ý�����мӲ���
	{
		delaySMG(100);
		if(S5==0)
		{
			if(SMG_mode==2)
			{
				if(Led_mode==4)
				{
					Led_mode=1;
				}
				else
				{
					Led_mode++;
				}
			}
			else if(SMG_mode==3)
			{
				if(jiange==12)
				{
					jiange=4;
				}
				else
				{
					jiange++;
				}
			}
		}
		while(S5==0)
		{
			SMG_show(SMG_mode);
			LED_show(0,Led_mode);
		}
	}
	
	if(S6==0)//����S6�������ģʽ�ı�
	{
		delaySMG(100);
		if(S6==0)
		{
			if(SMG_mode==3)
			{
				write_AT(0,jiange);//��Ledģʽ��תʱ��������eeprom
				SMG_mode=1;
			}
			else
			{
				SMG_mode++;
			}
		}
		while(S6==0)
		{
			SMG_show(SMG_mode);
			LED_show(0,Led_mode);
		}
	}
	if(S7==0)//����S7��Led��ͣ������
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
			//SMG_show(SMG_mode);
			LED_show(0,Led_mode);
		}
	}
}



void getLumi()//led�����ȵȼ��ж�
{
	getPCF=Read_PCF(0x03);//��ȡ�����������ķ�ѹ0-255
	if(getPCF<64)
	{
		Led_lumi=1;
	}
	else if(getPCF<128)
	{
		Led_lumi=2;
	}
	else if(getPCF<192)
	{
		Led_lumi=3;
	}
	else
	{
		Led_lumi=4;
	}
	
}
unsigned char P0L,P0H;//Led�Ƶ�4λ�͸�4λ
void LED_show(unsigned char i,unsigned char mode)//LED��ʾ����
{
	SelectHC(4);
	P0=0xff;
	if(LedPwm==0)
	{
		if(mode==1)
	{
		SelectHC(4);
		P0=LED_mode1[LEDCount%8];
		
	}
	else if(mode==2)
	{
		SelectHC(4);
		P0=LED_mode2[LEDCount%8];
	}
	else if(mode==3)
	{
		SelectHC(4);
		P0=LED_mode3[LEDCount%4];
	}
	else if(mode==4)
	{
		SelectHC(4);
		P0=LED_mode4[LEDCount%4];
	}
	}
	else
	{
		SelectHC(4);
		P0=0xff;
	}
	
}
void InitTimer0()//10ms��ʱ��
{
	TMOD = 0x01;                    //���ö�ʱ��Ϊģʽ0(16λ����װ��)
    TL0 = (65535-10000)%256;                     //��ʼ����ʱֵ
    TH0 = (65535-10000)/256;
    TR0 = 1;                        //��ʱ��0��ʼ��ʱ
    ET0 = 1;                        //ʹ�ܶ�ʱ��0�ж�
    EA = 1;
}

void InitTimer1()
{
	TMOD = 0x10;                    //���ö�ʱ��Ϊģʽ0(16λ�Զ���װ��)
    TL1 = (65535-100)%256;                     //��ʼ����ʱֵ
    TH1 = (65535-100)/256; 
    TR1 = 1;                        //��ʱ��1��ʼ��ʱ
    ET1 = 1;                        //ʹ�ܶ�ʱ��0�ж�
    EA = 1;
}

void ServiceTimer1() interrupt 3
{
	TL1 = (65535-100)%256;                     //��ʼ����ʱֵ
    TH1 = (65535-100)/256; 
	countShine++;
	if(countShine==pwm_duty*Led_lumi)
	{
		LedPwm=1;//�ߵ�ƽ
	}
	else if(countShine==100)
	{
		LedPwm=0;//�͵�ƽ
		countShine=0;
	}
	
	
	
}

unsigned int TimeFor800ms=0;//�������˸800ms��ʱ
unsigned int TimeForLed=0;//Ledģʽ��ת��ʱ
unsigned char jishi=0;
void ServiceTimer0() interrupt 1
{
	TL0 = (65535-10000)%256;                     //��ʼ����ʱֵ
    TH0 = (65535-10000)/256;
	TimeFor800ms++;
	if(S7_key==1)//��S7_key=0ʱLED��ת��ʱ��ͣ
	{
		TimeForLed++;
		jishi++;
	}
	if(jishi==8)//led��˸
	{
		LEDCount++;
		jishi=0;
	}
	if(LEDCount==8)
	{
		LEDCount==0;
	}
	
	if(TimeFor800ms==80)//�������˸800ms�ж�
	{
		if(is_800ms==0)
		{
			is_800ms=1;
		}
		else
		{
			is_800ms=0;
		}
		TimeFor800ms=0;
	}
	if((TimeForLed/10)/jiange==1)//��ʱ��������תʱ��ͽ���Ledģʽת��
	{
		TimeForLed=0;
		if(Led_mode==4)
		{
			Led_mode=1;
		}
		else
		{
			Led_mode++;
		}
		LEDCount=0;
		
	}
	//SMG_show(SMG_mode);
}


void main()
{
	InitSys();
	jiange=read_AT(0);
	InitTimer0();
	InitTimer1();
	//jiange=12;
	while(1)
	{
		getLumi();//LED���ȵȼ�
		keyCtrl();//��������
		SMG_show(SMG_mode);//�����������ʾ��
		LED_show(0,Led_mode);//LED��������ʾ
		
		
	}
}