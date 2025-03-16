#include <STC15F2K60S2.H>
#include "onewire.h"

unsigned char code SMG_NoDot[18] = {0xc0,0xf9,0xa4,
	0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,
	0xc0,0x86,0x8e,0xbf,0x7f};

unsigned char code SMG_IsDot[10] = {0x40,0x79,0x24,
	0x30,0x19,0x12,0x02,0x78,0x00,0x10};
unsigned int T_data;
void SMG_delay(unsigned int t)
{
	while(t--);
}
void SelectHC138(unsigned char channel)
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
	SelectHC138(7);
	P0=0xff;
	SelectHC138(6);
	P0=(0x01<<pos);
	SelectHC138(7);
	P0=value;
}

void show_SMG()
{
	display_SMG(7,SMG_NoDot[T_data%10]);
	SMG_delay(100);
	
	display_SMG(6,SMG_IsDot[(T_data%100)/10]);
	SMG_delay(100);
	
	display_SMG(5,SMG_NoDot[T_data/100]);
	SMG_delay(100);
}

void delay(unsigned int t)
{
	while(t--)
	{
		show_SMG();
	}
}
void read_Temp()
{
	unsigned char LSB,MSB;//存储温度的低8位和高8位
	
	init_ds18b20();
	Write_DS18B20(0xcc);//跳过ROM
	Write_DS18B20(0x44);//开始转换
	
	delay(1000);
	
	init_ds18b20();//复位
	Write_DS18B20(0xcc);//跳过ROM
	Write_DS18B20(0xbe);//发读数据的指令
	
	LSB=Read_DS18B20();//读取寄存器中的第0字节
	MSB=Read_DS18B20();//读取寄存器中的第1字节
	init_ds18b20();//复位
	T_data=0x0000;
	T_data=MSB;//低8位赋值
	//T_data=T_data<<8;//左移8位，变成高8位
	T_data<<=8;
	T_data=T_data | LSB;//通过或操作，将LSB中的值赋给T_data低8位
	if((T_data & 0xf800)==0x0000)
	{
		T_data>>=4;//把小数部分移出去
		T_data=T_data*10;//扩大10倍,为了消去小数点
		T_data=T_data+(LSB & 0x0f)*0.625;//LSB & 0x0f表示要取LSB的低4位
	}
}

void main(void)
{
	while(1)
	{
		//T_data=267;
		read_Temp();
		show_SMG();
	}
}