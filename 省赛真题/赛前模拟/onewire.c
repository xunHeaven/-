/*	# 	单总线代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/
#include <onewire.h>
#include <intrins.h>
#include <STC15F2K60S2.H>
//
sbit DQ=P1^4;
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

unsigned int read_temp()
{
	unsigned char LSB,MSB;
	unsigned int temp;
	init_ds18b20();//��λ 
	Write_DS18B20(0xcc);//����ROM 
	Write_DS18B20(0x44);//��ʼת�� 
	
	init_ds18b20();//��λ 
	Write_DS18B20(0xcc);//����ROM 
	Write_DS18B20(0xbe);//��ʼ��ȡ 
	LSB=Read_DS18B20();//��һ����LSB���յͰ�λ ��С�����4λ 
	MSB=Read_DS18B20();//�ڶ�����MSB���ո߰�λ ����5λ�Ƿ���λ 
	//init_ds18b20();
	temp=MSB;//
	temp=temp<<8;
	temp=temp|LSB;
	if((temp&0xf800)==0x0000)//�жϷǸ� 
	{
		temp=temp>>4;//����λ��С�� 
		temp=(temp*10)+(LSB&0x0f)*0.625;//����10�����൱�ڱ���1λС�� 
		return temp;
	}
	
}
