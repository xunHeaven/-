/*	# 	单总线代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/
#include <STC15F2K60S2.H>
#include <intrins.h>
#include "onewire.h"
//
void Delay_OneWire(unsigned int t)  
{
	unsigned char i;
	while(t--){
		for(i=0;i<12;i++);
	}
}

//
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}

//
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
  	Delay_OneWire(10); 
    initflag = DQ;     
  	Delay_OneWire(5);
  
  	return initflag;
}

unsigned int read_Temp()
{
	unsigned int temp;
	unsigned char LSB;//将寄存器中第0号位置的字符保存，表示低八位
	unsigned char MSB;//将寄存器中第1号位置的字符保存，高5位为符号
	init_ds18b20();//复位
	Write_DS18B20(0xcc);//跳过ROM指令；
	Write_DS18B20(0x44);//开始温度转换，需要700-900ms
	
	init_ds18b20();//复位
	Write_DS18B20(0xcc);//跳过ROM指令；
	Write_DS18B20(0xbe);//读函数指令
	LSB=Read_DS18B20();//读取寄存器中第0字节的内容
	MSB=Read_DS18B20();//读取寄存器中第1字节的内容，数据是补码形式，高5位为符号位
	init_ds18b20();//复位
	temp=0;
	temp=MSB;
	temp=temp<<8;
	temp=temp|LSB;
	if((temp&0xf800)==0)
	{
		temp=temp>>4;
		temp=temp*10;
		temp=temp+(unsigned int)((LSB & 0x0f)*0.625);//本来应该乘以0.0625，但是temp扩大十倍便于计算；
		return temp;
	}

	
}