#include <STC15F2K60S2.H>
#include <intrins.h>
#include <iic.h>
sbit S4=P3^3;
sbit S5=P3^2;
sbit S6=P3^1;
sbit S7=P3^0;
sbit TX=P1^0;
sbit RX=P1^1;
//sbit relay=P0^4;
//sbit buzzer=P0^6;
sbit pwmkey=P0^5;
unsigned char big_mode=1;//1-4
unsigned char freq_mode=1;//1Ϊ��λhz,2Ϊ��λkhz
unsigned char code SMG_number[]=
	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,
	0xc0,0x86,0x8e,0xbf,0x7f};//����ܶ��룬0-f��-.
unsigned char code SMG_Dotnumber[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};	
unsigned long freq;
unsigned int count_t1=0;//1sƵ�ʼ�ʱ
unsigned long count_f=0;//Ƶ�ʼ���
unsigned char count_t2=0;//����S7��ʱ
unsigned int shidu;
unsigned int distance=0;
unsigned char distance_canshu=60;//10-120
unsigned char ceju_mode=1;//1Ϊcm,2Ϊm
unsigned char canshu_mode=1;//1-3
unsigned int freq_canshu=9000;//1000-12000
unsigned char shidu_canshu=40;//10-60
unsigned char is_S7=0;//����S7�ж�
unsigned char is_1s=0;//��ʱ�Ƿ���1s
unsigned char count_relay=0;//�̵������ϴ���
unsigned char RB2;//��RB2��ֵ
unsigned char relay=0;//�жϼ̵����Ƿ����ϣ�1Ϊ����
unsigned char pwm=0;//����͵�ƽΪ0���ߵ�ƽΪ1
unsigned char is_500ms=1;
unsigned int count_ms1=0;//500ms���һ��
unsigned int count_ms2=0;//1khz,80%ռ�ձȼ�ʱ
unsigned int count_ms3=0;//led����˸Ƶ��Ϊ100ms;
unsigned char is_led=0;
void HCSelect(unsigned char channel);
void InitSys();
void displaySMG(unsigned char pos,unsigned char value);
void offSMG();
void showSMG();
void delay(unsigned int t);
void scanKey();
void run();
void InitTimer();
void getShidu();//��ȡʪ�ȵĺ���
void outDAC();
void relayCount();//�̵������Ϻͼ���,����pwm�����������
void read_Dis();//���������
void get_Dis();//0.5s���һ��
void showLED();

void main()
{
	InitSys();
	InitTimer();
	while(1)
	{
		run();
		scanKey();
	}
}
void run()
{
	get_Dis();
	getShidu();
	outDAC();
	relayCount();
	showSMG();
	showLED();
}
void InitTimer()
{
	TMOD=0x16;//��ʱ��1Ϊ16λ����װ�أ���ʱ��0Ϊ8λ�Զ���װ������
	TL0=0xff;
	TH0=0xff;
	
	TL1=(65535-100+1)%256;
	TH1=(65535-100+1)/256;
	
	TR0=1;
	ET0=1;
	TR1=1;
	ET1=1;
	EA=1;
}
void serviceTimer0() interrupt 1
{
	count_f++;
}
void serviceTimer1() interrupt 3
{
	TL1=(65535-100+1)%256;//100us
	TH1=(65535-100+1)/256;
	count_t1++;
	if(count_t1==10000)
	{
		freq=count_f;
		count_t1=0;
		count_f=0;
	}
	if(is_S7==1)
	{
		count_t2++;
		if(count_t2==10000)
		{
			is_1s=1;
			count_t2=0;
		}
	}
	count_ms1++;
	if(count_ms1==5000)
	{
		is_500ms=1;
		count_ms1=0;
	}
	count_ms2++;//
	if(count_ms2==8)//����100um��ʱ����count_ms2Ϊ10����1ms
	{
		pwm=0;//8-10����͵�ƽ
	}
	else if(count_ms2==10)
	{
		pwm=1;//0-8����ߵ�ƽ
		count_ms2=0;
	}
	count_ms3++;
	if(count_ms3==1000)//100ms
	{
		if(is_led==0)
		{
			is_led=1;
		}
		else
		{
			is_led=0;
		}
		count_ms3=0;
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
void showSMG()
{
	if(big_mode==1)
	{
		displaySMG(0,SMG_number[15]);
		delay(100);
		offSMG();
		if(freq_mode==1)
		{
			if(freq>99999)
			{
				displaySMG(2,SMG_number[freq/100000]);
				delay(100);
				offSMG();
			}
			if(freq>9999)
			{
				displaySMG(3,SMG_number[(freq%100000)/10000]);
				delay(100);
				offSMG();
			}
			if(freq>999)
			{
				displaySMG(4,SMG_number[(freq%10000)/1000]);
				delay(100);
				offSMG();
			}
			if(freq>99)
			{
				displaySMG(5,SMG_number[(freq%1000)/100]);
				delay(100);
				offSMG();
			}
			if(freq>9)
			{
				displaySMG(6,SMG_number[(freq%100)/10]);
				delay(100);
				offSMG();
			}
			displaySMG(7,SMG_number[freq%10]);
			delay(100);
			offSMG();
			
		}
		else if(freq_mode==2)
		{
			displaySMG(7,SMG_number[(freq%1000)/100]);
			delay(100);
			offSMG();
			displaySMG(6,SMG_Dotnumber[(freq%10000)/1000]);
			delay(100);
			offSMG();
			if(freq>9999)
			{
				displaySMG(5,SMG_number[(freq%100000)/10000]);
				delay(100);
				offSMG();
			}
			if(freq>99999)
			{
				displaySMG(4,SMG_number[freq/100000]);
				delay(100);
				offSMG();
			}
			
		}
	}
	else if(big_mode==2)
	{
		displaySMG(0,0x89);
		delay(100);
		offSMG();
		displaySMG(6,SMG_number[shidu/10]);
		delay(100);
		offSMG();
		displaySMG(7,SMG_number[shidu%10]);
		delay(100);
		offSMG();
		
	}
	else if(big_mode==3)
	{
		displaySMG(0,SMG_number[10]);
		delay(100);
		offSMG();
		if(ceju_mode==1)
		{
			if(distance>99)
			{
				displaySMG(5,SMG_number[distance/100]);
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
			displaySMG(5,SMG_Dotnumber[distance/100]);
			delay(100);
			offSMG();
			displaySMG(6,SMG_number[(distance%100)/10]);
			delay(100);
			offSMG();
			displaySMG(7,SMG_number[distance%10]);
			delay(100);
			offSMG();

		}
	}
	else if(big_mode==4)
	{
		displaySMG(0,0x8c);//P
		delay(100);
		offSMG();
		if(canshu_mode==1)
		{
			displaySMG(1,SMG_number[1]);
			delay(100);
			offSMG();
			if(freq_canshu>9999)
			{
				displaySMG(5,SMG_number[freq_canshu/10000]);
				delay(100);
				offSMG();
			}
			displaySMG(6,SMG_Dotnumber[(freq_canshu%10000)/1000]);
			delay(100);
			offSMG();
			displaySMG(7,SMG_number[(freq_canshu%1000)/100]);
			delay(100);
			offSMG();
			
		}
		else if(canshu_mode==2)
		{
			displaySMG(1,SMG_number[2]);
			delay(100);
			offSMG();
			displaySMG(6,SMG_number[shidu_canshu/10]);
			delay(100);
			offSMG();
			displaySMG(7,SMG_number[shidu_canshu%10]);
			delay(100);
			offSMG();
		}
		else if(canshu_mode==3)
		{
			displaySMG(1,SMG_number[3]);
			delay(100);
			offSMG();
			displaySMG(6,SMG_Dotnumber[distance_canshu/100]);
			delay(100);
			offSMG();
			displaySMG(7,SMG_number[(distance_canshu%100)/10]);
			delay(100);
			offSMG();
		}
	}
}
void scanKey()
{
	if(S4==0)
	{
		delay(100);
		if(S4==0)
		{
			if(big_mode==1)
			{
				big_mode=2;
				freq_mode=1;
			}
			else if(big_mode==2)
			{
				big_mode=3;
			}
			else if(big_mode==3)
			{
				big_mode=4;
				ceju_mode=1;
			}
			else if(big_mode==4)
			{
				big_mode=1;
				canshu_mode=1;
			}
		}
		while(S4==0)
		{
			run();
		}
	}
	if(S5==0)
	{
		delay(100);
		if(S5==0)
		{
			if(big_mode==4)
			{
				if(canshu_mode==3)
				{
					canshu_mode=1;
				}
				else
				{
					canshu_mode++;
				}
				
			}
		}
		while(S5==0)
		{
			run();
		}
	}
	if(S6==0)
	{
		delay(100);
		if(S6==0)
		{
			if(big_mode==4)
			{
				if(canshu_mode==1)
				{
					if(freq_canshu==12000)
					{
						freq_canshu=1000;
					}
					else
					{
						freq_canshu+=500;
					}
				}
				else if(canshu_mode==2)
				{
					if(shidu_canshu==60)
					{
						shidu_canshu=10;
					}
					else
					{
						shidu_canshu+=10;
					}
				}
				else if(canshu_mode==3)
				{
					if(distance_canshu==120)
					{
						distance_canshu=10;
					}
					else
					{
						distance_canshu+=10;
					}
				}
			}
			if(big_mode==3)
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
		}
		while(S6==0)
		{
			run();
		}

	}
	if(S7==0)
	{
		delay(100);
		if(S7==0)
		{
			if(big_mode==4)
			{
				if(canshu_mode==1)
				{
					if(freq_canshu==1000)
					{
						freq_canshu=12000;
					}
					else
					{
						freq_canshu-=500;
					}
				}
				else if(canshu_mode==2)
				{
					if(shidu_canshu==10)
					{
						shidu_canshu=60;
					}
					else
					{
						shidu_canshu-=10;
					}
				}
				else if(canshu_mode==3)
				{
					if(distance_canshu==10)
					{
						distance_canshu=120;
					}
					else
					{
						distance_canshu-=10;
					}
				}
			}
			if(big_mode==1)
			{
				if(freq_mode==1)
				{
					freq_mode=2;
				}
				else
				{
					freq_mode=1;
				}
			}
			if(big_mode==2)
			{
				is_S7=1;
				
			}
		}
		while(S7==0)
		{
			if(is_1s==1)
			{
				count_relay=0;
				write_AT(0x00,count_relay);
			}
			run();
		}
		is_S7=0;
		is_1s=0;
		count_t2=0;
	}
}
void getShidu()
{
	RB2=read_PCF(0x03);
	if(RB2>=255)
	{
		shidu=99;
	}
	else
	{
		shidu=(100*RB2)/255;
	}
}
void outDAC()
{
	if(shidu<=shidu_canshu)
	{
		write_PCF(51);
	}
	else if(shidu>=80)
	{
		write_PCF(255);
	}
	else
	{
		unsigned char temp;
		temp=((shidu-shidu_canshu)/(80-shidu_canshu))*204+51;
		write_PCF(temp);
	}
	
}
void relayCount()
{
	HCSelect(5);
	if(pwm==1)
	{
		pwmkey=0;//����ߵ�ƽ
	}
	else
	{
		pwmkey=1;//����͵�ƽ
	}
	if(distance>distance_canshu)
	{
		if(relay==0)
		{
			count_relay++;
			relay=1;
			write_AT(0x00,count_relay);
		}
	}
	else
	{
		relay=0;
	}
	if(relay==1)//�̵�������
	{
		P0=(P0&0x20)|0x10;
	}
	else
	{
		P0=P0&0x20;
	}

}

void read_Dis()
{
	unsigned char num=8;
	TX=0;
	CL=0xf3;//12us
	CH=0xff;
	CR=1;//PCA��ʱ
	while(num--)
	{
		while(!CF);//12u��CF=1������ѭ��
		TX=~TX;
		CL=0xf3;
		CH=0xff;
		CF=0;
		
	}
	CR=0;
	CL=0;
	CH=0;
	CR=1;
	while(RX &&(!CF));//RX=0��ʾ�յ�����
	CR=0;
	if(CF)//CF=0�������
	{
		CF=0;
		distance=255;
	}
	else
	{
		distance=((CH<<8)+CL)*0.017;
	}
	
}
void get_Dis()
{
	if(is_500ms)
	{
		read_Dis();
	}
	is_500ms=0;
}
void showLED()
{
	HCSelect(4);
	if(big_mode==1)
	{
		P0=0xfe;
	}
	else if(big_mode==2)
	{
		P0=0xfd;
	}
	else if(big_mode==3)
	{
		P0=0xfb;
	}
	else if(big_mode==4)
	{
		if(canshu_mode==1)
		{
			if(is_led==1)
			{
				P0=0xfe;
			}
			else
			{
				P0=0xff;
			}
		}
		else if(canshu_mode==2)
		{
			if(is_led==1)
			{
				P0=0xfd;
			}
			else
			{
				P0=0xff;
			}
		}
		else if(canshu_mode==3)
		{
			if(is_led==1)
			{
				P0=0xfb;
			}
			else
			{
				P0=0xff;
			}
		}
	}
	if(freq>freq_canshu)
	{
		P0=P0&0xf7;
	}
	if(shidu>shidu_canshu)
	{
		P0=P0&0xef;
	}
	if(distance>distance_canshu)
	{
		P0=P0&0xdf;
	}
	delay(500);
}