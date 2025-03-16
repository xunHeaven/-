/*	# 	å•æ€»çº¿ä»£ç ç‰‡æ®µè¯´æ˜
	1. 	æœ¬æ–‡ä»¶å¤¹ä¸­æä¾›çš„é©±åŠ¨ä»£ç ä¾›å‚èµ›é€‰æ‰‹å®Œæˆç¨‹åºè®¾è®¡å‚è€ƒã€‚
	2. 	å‚èµ›é€‰æ‰‹å¯ä»¥è‡ªè¡Œç¼–å†™ç›¸å…³ä»£ç æˆ–ä»¥è¯¥ä»£ç ä¸ºåŸºç¡€ï¼Œæ ¹æ®æ‰€é€‰å•ç‰‡æœºç±»å‹ã€è¿è¡Œé€Ÿåº¦å’Œè¯•é¢˜
		ä¸­å¯¹å•ç‰‡æœºæ—¶é’Ÿé¢‘ç‡çš„è¦æ±‚ï¼Œè¿›è¡Œä»£ç è°ƒè¯•å’Œä¿®æ”¹ã€‚
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
	init_ds18b20();//¸´Î» 
	Write_DS18B20(0xcc);//Ìø¹ıROM 
	Write_DS18B20(0x44);//¿ªÊ¼×ª»» 
	
	init_ds18b20();//¸´Î» 
	Write_DS18B20(0xcc);//Ìø¹ıROM 
	Write_DS18B20(0xbe);//¿ªÊ¼¶ÁÈ¡ 
	LSB=Read_DS18B20();//µÚÒ»´ÎÓÃLSB½ÓÊÕµÍ°ËÎ» £¬Ğ¡Êıµãºó4Î» 
	MSB=Read_DS18B20();//µÚ¶ş´ÎÓÃMSB½ÓÊÕ¸ß°ËÎ» £¬¸ß5Î»ÊÇ·ûºÅÎ» 
	//init_ds18b20();
	temp=MSB;//
	temp=temp<<8;
	temp=temp|LSB;
	if((temp&0xf800)==0x0000)//ÅĞ¶Ï·Ç¸º 
	{
		temp=temp>>4;//µÍËÄÎ»ÊÇĞ¡Êı 
		temp=(temp*10)+(LSB&0x0f)*0.625;//À©´ó10±¶£¬Ïàµ±ÓÚ±£Áô1Î»Ğ¡Êı 
		return temp;
	}
	
}
