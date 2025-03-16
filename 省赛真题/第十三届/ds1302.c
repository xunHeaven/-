/*	# 	DS1302����Ƭ��˵��
	1. 	���ļ������ṩ���������빩����ѡ����ɳ�����Ʋο���
	2. 	����ѡ�ֿ������б�д��ش�����Ըô���Ϊ������������ѡ��Ƭ�����͡������ٶȺ�����
		�жԵ�Ƭ��ʱ��Ƶ�ʵ�Ҫ�󣬽��д�����Ժ��޸ġ�
*/								
#include <STC15F2K60S2.H>
#include <intrins.h>
#include "ds1302.h"
//
unsigned char write_addr[7]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
unsigned char read_addr[7]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
unsigned char Timer[7]={0x10,0x059,0x22,0x08,0x04,0x01,0x24};
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

//
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}
void write_date()
{
	unsigned char n;
	Write_Ds1302_Byte(0x8e,0x00);//�ر�д����
	for(n=0;n<7;n++)
	{
		Write_Ds1302_Byte(write_addr[n],Timer[n]);//����Ӧλ��д���Ӧ����
	}
	Write_Ds1302_Byte(0x8e,0x80);//�ر�д����
}
void read_date()
{
	unsigned char n;
	for(n=0;n<7;n++)
	{
		Timer[n]=Read_Ds1302_Byte(read_addr[n]);//������Ӧ�Ĵ��������ݴ���Timer
	}
}
