#include <STC15F2K60S2.H>

unsigned char code SMG_number[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
     0x80,0x90,0x88,0x83,0xc6,0xc0,0x86,0x8e,
	0xbf,0x7f};
unsigned int t_hour=0;
unsigned int t_min=59;
unsigned int t_sec=0;
sbit S7=P3^0;
sbit S6=P3^1;
sbit S5=P3^2;
sbit S4=P3^3;
unsigned int i=0;
unsigned time_ctrl=0;	
void delay(unsigned int t)
{
	while(t--);
}
void SelectHC573(unsigned char channel)
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
	SelectHC573(7);
	P0=0xff;
	SelectHC573(6);
	P0=0x01<<pos;
	SelectHC573(7);
	P0=value;
}

//-------------定时器中断
void InitTimer0()
{
	TMOD = 0x00;                    //设置定时器为模式0(16位自动重装载)
    TL0 = (65535-50000)/256;        //初始化计时值
    TH0 = (65535-50000)%256;
	
    TR0 = 1;                        //定时器0开始计时
    ET0 = 1;                        //使能定时器0中断
    EA = 1;
}
unsigned int count=0;
void ServiceTimer0() interrupt 1
{
	TH0 = (65535-50000)/256;        //初始化计时值
    TL0 = (65535-50000)%256;
	count++;
	if(count==20)
	{
		count=0;
		t_sec++;
		if(t_sec==60)
		{
			t_sec=0;
			t_min++;
			
		}
		if(t_min==60)
		{
			t_hour++;
			t_min=0;
		}
		if(t_hour==24)
		{
			t_hour=0;
		}
	}
	
}

//--------------

void show_Timer(unsigned char smg0,unsigned char smg1,unsigned char smg3,unsigned char smg4,unsigned char smg6,unsigned char smg7)
{
	display_SMG(0,smg0);
	delay(500);
	display_SMG(1,smg1);
	delay(500);
	
	display_SMG(2,SMG_number[16]);
	delay(500);
	
	display_SMG(3,smg3);
	delay(500);
	display_SMG(4,smg4);
	delay(500);
	
	display_SMG(5,SMG_number[16]);
	delay(500);
	
	display_SMG(6,smg6);
	delay(500);
	display_SMG(7,smg7);
	delay(500);
}


void ScanKey()
{
	if(S4==0)
	{
		delay(100);
		if(S4==0)
		{
			time_ctrl++;
			if(time_ctrl==4)
			{
				time_ctrl=0;
			}
			while(S4==0)
			{
				show_Timer(SMG_number[t_hour/10],SMG_number[t_hour%10],SMG_number[t_min/10],SMG_number[t_min%10],SMG_number[t_sec/10],SMG_number[t_sec%10]);
				
			}
		}
	}
	if(S5==0)
	{
		delay(100);
		if(S5==0)
		{
			if(time_ctrl==1)
			{
				t_sec=0;
			}
			if(time_ctrl==2)
			{
				t_min=0;
			}
			if(time_ctrl==3)
			{
				t_hour=0;
			}
			while(S5==0)
			{
				show_Timer(SMG_number[t_hour/10],SMG_number[t_hour%10],SMG_number[t_min/10],SMG_number[t_min%10],SMG_number[t_sec/10],SMG_number[t_sec%10]);
				
			}
		}
		
	}
	if(S6==0)
	{
		delay(100);
		if(S6==0)
		{
			if(time_ctrl==1)
			{
				if(t_sec==0)
				{
					t_sec=59;
				}
				else
				{
					t_sec--;
				}
			}
			if(time_ctrl==2)
			{
				if(t_min==0)
				{
					t_min=59;
				}
				else
				{
					t_min--;
				}
			}
			if(time_ctrl==3)
			{
				if(t_hour==0)
				{
					t_hour=23;
				}
				else
				{
					t_hour--;
				}
			}
			while(S6==0)
			{
				show_Timer(SMG_number[t_hour/10],SMG_number[t_hour%10],SMG_number[t_min/10],SMG_number[t_min%10],SMG_number[t_sec/10],SMG_number[t_sec%10]);
			}
			
		}
	}
	if(S7==0)
	{
		delay(100);
		if(S7==0)
		{
			if(time_ctrl==1)
			{
				if(t_sec==59)
				{
					t_sec=0;
				}
				else
				{
					t_sec++;
				}
				
			}
			if(time_ctrl==2)
			{
				if(t_min==59)
				{
					t_min=0;
				}
				else
				{
					t_min++;
				}
			}
			if(time_ctrl==3)
			{
				if(t_hour==23)
				{
					t_hour=0;
				}
				else
				{
					t_hour++;
				}

			}
			while(S7==0)
			{
				show_Timer(SMG_number[t_hour/10],SMG_number[t_hour%10],SMG_number[t_min/10],SMG_number[t_min%10],SMG_number[t_sec/10],SMG_number[t_sec%10]);
			}
			
		}
	}
	
	
	
	
}

void main(void)
{
	InitTimer0();
	while(1)
	{
		if(time_ctrl==0)
		{
			show_Timer(SMG_number[t_hour/10],SMG_number[t_hour%10],SMG_number[t_min/10],SMG_number[t_min%10],SMG_number[t_sec/10],SMG_number[t_sec%10]);
		}
		else if(time_ctrl==1)
		{
			i=20;
			while(i--)
			{
				show_Timer(SMG_number[t_hour/10],SMG_number[t_hour%10],SMG_number[t_min/10],SMG_number[t_min%10],SMG_number[t_sec/10],SMG_number[t_sec%10]);
			}
			i=20;
			while(i--)
			{
				show_Timer(SMG_number[t_hour/10],SMG_number[t_hour%10],SMG_number[t_min/10],SMG_number[t_min%10],0xff,0xff);
			}
			
		}
		else if(time_ctrl==2)
		{
			i=20;
			while(i--)
			{
				show_Timer(SMG_number[t_hour/10],SMG_number[t_hour%10],SMG_number[t_min/10],SMG_number[t_min%10],SMG_number[t_sec/10],SMG_number[t_sec%10]);
			}
			i=20;
			while(i--)
			{
				show_Timer(SMG_number[t_hour/10],SMG_number[t_hour%10],0xff,0xff,SMG_number[t_sec/10],SMG_number[t_sec%10]);
			}
			
		}
		else if(time_ctrl==3)
		{
			i=20;
			while(i--)
			{
				show_Timer(SMG_number[t_hour/10],SMG_number[t_hour%10],SMG_number[t_min/10],SMG_number[t_min%10],SMG_number[t_sec/10],SMG_number[t_sec%10]);
			}
			i=20;
			while(i--)
			{
				show_Timer(0xff,0xff,SMG_number[t_min/10],SMG_number[t_min%10],SMG_number[t_sec/10],SMG_number[t_sec%10]);
			}
		}
		ScanKey();
		
	}
}
