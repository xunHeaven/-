/*	#   I2C代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/
#include "iic.h"
#include <intrins.h>
#include <STC15F2K60S2.H>

#define DELAY_TIME	5

//
static void I2C_Delay(unsigned char n)
{
    do
    {
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();		
    }
    while(n--);      	
}

//
void I2CStart(void)
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

//
void I2CStop(void)
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

//
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

//
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

//等待应答
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

//发送应答，ackbit=1不发送应答
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}

//向AT24芯片写入数据
void AT24_Write(unsigned addr,unsigned char dat)
{
	I2CStart();
	I2CSendByte(0xa0);//设备地址，看figure7
	I2CWaitAck();
	I2CSendByte(addr);//字节地址，即要把数据写到AT24芯片的哪一个单元
	I2CWaitAck();
	I2CSendByte(dat);//要写入的数据
	I2CWaitAck();
	I2CStop();
}

//从AT24芯片读出数据
unsigned char AT24_Read(unsigned char addr)
{
	unsigned char temp;//保存读出的数据
	I2CStart();
	I2CSendByte(0xa0);//设备地址，写
	I2CWaitAck();
	I2CSendByte(addr);//字节地址，即要从AT24芯片的哪一个单元读数据
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xa1);//设备地址，读
	I2CWaitAck();
	temp=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	
	return temp;
}

void PCF_Write(unsigned char dat)//out口输出模拟信号（需万用表测试）
{
	I2CStart();
	I2CSendByte(0x90);//设备地址，写
	I2CWaitAck();
	I2CSendByte(0x40);//参数是PCF中的控制字，DA转换表示输出电压，要用out口
	I2CWaitAck();
	I2CSendByte(dat);//要写入的数据
	I2CWaitAck();
	I2CStop();
}

unsigned char PCF_Read(unsigned char ctrl_byte)//读取PCF8591
{
	unsigned char temp;
	I2CStart();
	I2CSendByte(0x90);//设备地址，写
	I2CWaitAck();
	I2CSendByte(ctrl_byte);//参数是PCF中的控制字，0x01表示光敏电阻，0x03表示滑动电阻
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0x91);//设备地址，读
	I2CWaitAck();
	temp=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	
	return temp;
}

