#include<STC15F2K60S2.h>
#include<ds1302.h>
#include<onewire.h>
#define uint unsigned int
#define uchar unsigned char
uchar code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1
uchar code t_com[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};    //位码
uchar number[8]={16,16,17,16,16,17,16,16};
uchar Trg,Cont;
uchar Time[3];
uchar initTime[] = {50,59,23};
uchar Clock[3]={0,0,0};
uchar flag,rd_count;
uint p=0;
uchar key_flag=0;
uchar led_flag=0;
uchar temp_flag=0;
uchar mode=0;
uint Time_set=0,Clock_set=0;
long tt;
//变量定义

void Delay10ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}


//关闭蜂鸣器并初始化
void init()
{
	P2=0xa0;P0=0x00;P2=0x00;
	P2=0x80;P0=0xff;P2=0x00;
}
//按键三段代码
void button()
{
	uchar Readdata = P3^0xff;
	Trg=Readdata & (Readdata ^ Cont);
	Cont=Readdata;
}
//定时器初始化
void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	EA=1;
	ET0=1;
}

void Time_0() interrupt 1
{
	static uint count=0,C=0,led=0;
	count++;C++;
	if(count>=10)		//每10毫秒检测一次是否有按键按下
	{
		count=0;
		key_flag=1;
	}
	//数码管动态显示
	if(C>=2)
	{
		C=0;
		P0=t_com[p];P2=0xc0;P2=0x00;
		P0=~t_display[number[p]];P2=0xe0;P2=0x00;
		p++;
	}
	if(p>=8)
		p=0;
	rd_count++;		//时钟的秒计时
	
	//此处为led闪烁处理
	if(led_flag==1)
	{
		led++;
		if(led%400>=199&&led%400<=399)
		{
			P2 = 0x80;P0 = 0xfe;P2 = 0;
		}
		else
		{
			P2 = 0x80;P0 = 0xff;P2 = 0;
		}
	}
	if(led>=2000)	//可能存在问题，此处led灯闪烁重复两遍，所以将原本的5s（5000ms）改为了2s（2000ms）
	{							//但实际闪烁总时常确实为5s。
		led=0;
		led_flag=0;
		P2 = 0x80;P0 = 0xff;P2 = 0;
	}
}

void key_function()
{
	//时间显示调试
	if(Trg&0x01)
	{
		if(flag==1&&Time_set==0)
		{
			flag=0;
			Time_Stop(Time);
		}
		Time_set++;
	}
	//时间显示模式下的时分秒的加减
	if(flag==0&&Time_set==1&&Trg&0x04)
		{
			Time[2] = ++Time[2] % 24;
			if(Time[2]>=24)
						Time[2]=0;
			setTime(Time);
		}
	if(flag==0&&Time_set==1&&Trg&0x08)
		{
			Time[2] = --Time[2] % 24;
					if(Time[2]<=0)
						Time[2]=23;
					setTime(Time);
		}
	if(flag==0&&Time_set==2&&Trg&0x04)
		{
			Time[1] = ++Time[1] % 60;
					if(Time[1]>=60)
						Time[1]=0;
					setTime(Time);
		}
	if(flag==0&&Time_set==2&&Trg&0x08)
		{
			Time[1] = --Time[1] % 60;
					if(Time[1]<=0)
						Time[1]=59;
					setTime(Time);
		}
	if(flag==0&&Time_set==3&&Trg&0x04)
		{
			Time[0] = ++Time[0] % 60;
					if(Time[0]>=60)
						Time[0]=0;
					setTime(Time);
		}
	if(flag==0&&Time_set==3&&Trg&0x08)
		{
			Time[0] = --Time[0] % 60;
					if(Time[0]<=0)
						Time[0]=59;
					setTime(Time);
		}
	
	//闹钟显示调试
	if(Trg&0x02)
	{
		mode=++mode%4;
		Clock_set++;
	}
	//闹钟设置下的时分秒的加减
	if(mode!=0&&Clock_set==1&&Trg&0x04)
		{
			Clock[2] = ++Clock[2] % 24;
			if(Clock[2]>=24)
						Clock[2]=0;
		}
	if(mode!=0&&Clock_set==1&&Trg&0x08)
		{
			Clock[2] = --Clock[2] % 24;
			if(Clock[2]<=0)
						Clock[2]=23;
		}
	if(mode!=0&&Clock_set==2&&Trg&0x04)
		{
			Clock[1] = ++Clock[1] % 60;
			if(Clock[1]>=60)
						Clock[1]=0;
		}
	if(mode!=0&&Clock_set==2&&Trg&0x08)
		{
			Clock[1] = --Clock[1] % 60;
			if(Clock[1]<=0)
						Clock[1]=59;
		}
	if(mode!=0&&Clock_set==3&&Trg&0x04)
		{
			Clock[0] = ++Clock[0] % 60;
			if(Clock[0]>=60)
						Clock[0]=0;
		}
	if(mode!=0&&Clock_set==3&&Trg&0x08)
		{
			Clock[0] = --Clock[0] % 60;
			if(Clock[0]<=0)
						Clock[0]=59;
		}
	//温度信号控制
	if(flag==1&&mode==0&&Trg&0x08)
	{
		Delay10ms();
		if(Trg&0x08)
		{
			temp_flag++;
		}
	}
}

void time_math()		//时间显示，数值赋予，其中number[2]和number[5]固定为小横线
{
		number[0]=Time[0]>=0 ? Time[0]%10 : 0;
		number[1]=Time[0]>=10 ? Time[0]/10 : 0;
		number[3]=Time[1]>=0 ? Time[1]%10 : 0;
		number[4]=Time[1]>=10 ? Time[1]/10 : 0;
		number[6]=Time[2]>=0 ? Time[2]%10 : 0;
		number[7]=Time[2]>=10 ? Time[2]/10 : 0;
		number[2]=number[5]=17;
}
//时间设置下数码管灭掉的三种情况
void time_math_shi()
{
		number[0]=Time[0]>=0 ? Time[0]%10 : 0;
		number[1]=Time[0]>=10 ? Time[0]/10 : 0;
		number[3]=Time[1]>=0 ? Time[1]%10 : 0;
		number[4]=Time[1]>=10 ? Time[1]/10 : 0;
		number[6]=16;
		number[7]=16;
		number[2]=number[5]=17;
}
void time_math_fen()
{
		number[0]=Time[0]>=0 ? Time[0]%10 : 0;
		number[1]=Time[0]>=10 ? Time[0]/10 : 0;
		number[3]=16;
		number[4]=16;
		number[6]=Time[2]>=0 ? Time[2]%10 : 0;
		number[7]=Time[2]>=10 ? Time[2]/10 : 0;
		number[2]=number[5]=17;
}
void time_math_miao()
{
		number[0]=16;
		number[1]=16;
		number[3]=Time[1]>=0 ? Time[1]%10 : 0;
		number[4]=Time[1]>=10 ? Time[1]/10 : 0;
		number[6]=Time[2]>=0 ? Time[2]%10 : 0;
		number[7]=Time[2]>=10 ? Time[2]/10 : 0;
		number[2]=number[5]=17;
}


void time_math2()		//闹钟设置，数值赋予，其中number[2]和number[5]固定为小横线
{
	number[0]=Clock[0]>=0 ? Clock[0]%10 : 0;
	number[1]=Clock[0]>=10 ? Clock[0]/10 : 0;
	number[3]=Clock[1]>=0 ? Clock[1]%10 : 0;
	number[4]=Clock[1]>=10 ? Clock[1]/10 : 0;
	number[6]=Clock[2]>=0 ? Clock[2]%10 : 0;
	number[7]=Clock[2]>=10 ? Clock[2]/10 : 0;
	number[2]=number[5]=17;
}
//闹钟设置下数码管灭掉的三种情况
void time_math2_shi()
{
	number[0]=Clock[0]>=0 ? Clock[0]%10 : 0;
	number[1]=Clock[0]>=10 ? Clock[0]/10 : 0;
	number[3]=Clock[1]>=0 ? Clock[1]%10 : 0;
	number[4]=Clock[1]>=10 ? Clock[1]/10 : 0;
	number[6]=16;
	number[7]=16;
	number[2]=number[5]=17;
}
void time_math2_fen()
{
	number[0]=Clock[0]>=0 ? Clock[0]%10 : 0;
	number[1]=Clock[0]>=10 ? Clock[0]/10 : 0;
	number[3]=16;
	number[4]=16;
	number[6]=Clock[2]>=0 ? Clock[2]%10 : 0;
	number[7]=Clock[2]>=10 ? Clock[2]/10 : 0;
	number[2]=number[5]=17;
}
void time_math2_miao()
{
	number[0]=16;
	number[1]=16;
	number[3]=Clock[1]>=0 ? Clock[1]%10 : 0;
	number[4]=Clock[1]>=10 ? Clock[1]/10 : 0;
	number[6]=Clock[2]>=0 ? Clock[2]%10 : 0;
	number[7]=Clock[2]>=10 ? Clock[2]/10 : 0;
	number[2]=number[5]=17;
}


void main()
{
	init();
	Timer0Init();
	setTime(initTime);		//设置初始时间23：59：50
	flag=1;		//默认为运行状态
	mode=0;		//默认是时间显示模式
	led_flag=0;
	while(DS18B20_Get()*0.0625==100);//等待默认数据100显示结束
	while(1)
	{
		int a=520;
		tt = DS18B20_Get();	//获取温度
		tt = tt * 0.0625;
		//不同模式（mode）下的功能
				if(mode==0&&temp_flag%2==0)	//时间设置模式
			{
				if(flag==0&&Time_set==1)
				{
					while(a--)
					{
						time_math_shi();
					}
				}
				else if(flag==0&&Time_set==2)
				{
					while(a--)
					{
						time_math_fen();
					}
				}
				else if(flag==0&&Time_set==3)
				{
					while(a--)
					{
						time_math_miao();
					}
				}
				time_math();
				if(key_flag==1)		//对按键判断并作出决策
				{
					key_flag=0;
					button();
					key_function();
				}
			}
			else if(mode!=0&&temp_flag%2==0)//闹钟设置模式
			{
				if(mode==1)
				{
					while(a--)
					{
						time_math2_shi();
					}
				}
				else if(mode==2)
				{
					while(a--)
					{
						time_math2_fen();
					}
				}
				else if(mode==3)
				{
					while(a--)
					{
						time_math2_miao();
					}
				}
				time_math2();
				if(key_flag==1)		//对按键判断并作出决策
				{
					key_flag=0;
					button();
					key_function();
				}
			}
		//对秒数，时间设置的选择重置，闹钟设置的重置
		if(rd_count>=100)		//每过1s，对应时间变化
		{
			rd_count=0;
			if(flag==1)
				getTime(Time);
		}
		if(Time_set>=4)
		{
			flag=1;
			Time_set=0;
		}
		if(Clock_set>=4)
		{
			Clock_set=0;
		}
		//查询闹钟时间是否到了
		if(Time[1]==Clock[1]&&Time[2]==Clock[2])
		{
			if(Time[0]==Clock[0]||Time[0]-1==Clock[0]||Time[0]-2==Clock[0]||Time[0]-3==Clock[0]||Time[0]-4==Clock[0])
				led_flag=1;
			if(key_flag==1)		//对按键判断并作出决策
				{
					key_flag=0;
					button();
					key_function();
				}
		}
		else
		{
			led_flag=0;
		}
		//显示温度
		if(temp_flag%2==1)
		{
			if(key_flag==1)		//对按键判断并作出决策
				{
					key_flag=0;
					button();
					key_function();
				}
			number[7]=number[6]=number[5]=number[4]=number[3]=16;
			number[2]=tt/10;
			number[1]=tt%10;
			number[0]=12;
		}
	}
}