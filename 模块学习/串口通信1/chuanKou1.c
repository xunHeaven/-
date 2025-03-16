#include <STC15F2K60S2.H>
#include <intrins.h>

unsigned char urdat;//用来接收数据
void InitUart();
void sendByte(unsigned char dat);
void main()
{
	InitUart();
	sendByte(0x5a);
	sendByte(0xaa);
	while(1);

}

void InitUart()
{
	TMOD=0x20;//定时器1，八位自动重装载
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;
	
	SCON=0x50;//8位ur模式，允许接收
	AUXR=0x00;
	
	ES=1;
	EA=1;
	
	
	
}
void serviceUart() interrupt 4
{
	if(RI==1)//是否接收到完整数据
	{
		RI=0;//将RI置0
		urdat=SBUF;//将传到寄存器SBUF中的内容读出
		
		sendByte(urdat+1);//实现发送更新
	}
}
void sendByte(unsigned char dat)
{
	SBUF=dat;//将dat放入SBUF
	while(TI==0);//没发送完成时TI=0,发送完毕后TI=1
	TI=0;//将TI置为0；
}