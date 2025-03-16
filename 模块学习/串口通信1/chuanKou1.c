#include <STC15F2K60S2.H>
#include <intrins.h>

unsigned char urdat;//������������
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
	TMOD=0x20;//��ʱ��1����λ�Զ���װ��
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;
	
	SCON=0x50;//8λurģʽ���������
	AUXR=0x00;
	
	ES=1;
	EA=1;
	
	
	
}
void serviceUart() interrupt 4
{
	if(RI==1)//�Ƿ���յ���������
	{
		RI=0;//��RI��0
		urdat=SBUF;//�������Ĵ���SBUF�е����ݶ���
		
		sendByte(urdat+1);//ʵ�ַ��͸���
	}
}
void sendByte(unsigned char dat)
{
	SBUF=dat;//��dat����SBUF
	while(TI==0);//û�������ʱTI=0,������Ϻ�TI=1
	TI=0;//��TI��Ϊ0��
}