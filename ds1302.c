/*
  程序说明: DS1302驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台 8051，12MHz
  日    期: 2011-8-9
*/

#include <reg52.h>
#include <intrins.h>

sbit SCK=P1^7;		
sbit SDA=P2^3;		
sbit RST = P1^3;   // DS1302复位												

void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK=0;
		SDA=temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

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
void setTime(unsigned char Time[])
{
	Write_Ds1302_Byte(0x8e, 0);								 //开写保护
	Write_Ds1302_Byte(0x80, Time[0] / 10 * 16 + Time[0] % 10);//设置秒值
	Write_Ds1302_Byte(0x82, Time[1] / 10 * 16 + Time[1] % 10);//设置分值
	Write_Ds1302_Byte(0x84, Time[2] / 10 * 16 + Time[2] % 10);//设置小时值
	Write_Ds1302_Byte(0x8e, 0x80);//关写保护
}
void getTime(unsigned char Time[])
{
	unsigned char bcd;
	bcd = Read_Ds1302_Byte(0x81);//读取秒值
	Time[0] = (bcd >> 4 & 0x07) * 10 + (bcd & 0x0f);
	bcd = Read_Ds1302_Byte(0x83);//读取分值
	Time[1] = (bcd >> 4 & 0x07) * 10 + (bcd & 0x0f);
	bcd = Read_Ds1302_Byte(0x85);//读取小时值
	Time[2] = (bcd >> 4 & 0x07) * 10 + (bcd & 0x0f);
}
void Time_Stop(unsigned char Time[]) //暂停
{
	Write_Ds1302_Byte(0x8e, 0x00);
	Write_Ds1302_Byte(0x80, ((Time[0] / 10 << 4) | Time[0] % 10) | 0x80);
	Write_Ds1302_Byte(0x8e, 0x80);
}