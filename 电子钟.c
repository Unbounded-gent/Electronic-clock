#include<STC15F2K60S2.h>
#include<ds1302.h>
#include<onewire.h>
#define uint unsigned int
#define uchar unsigned char
uchar code t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1
uchar code t_com[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};    //λ��
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
//��������

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


//�رշ���������ʼ��
void init()
{
	P2=0xa0;P0=0x00;P2=0x00;
	P2=0x80;P0=0xff;P2=0x00;
}
//�������δ���
void button()
{
	uchar Readdata = P3^0xff;
	Trg=Readdata & (Readdata ^ Cont);
	Cont=Readdata;
}
//��ʱ����ʼ��
void Timer0Init(void)		//1����@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xCD;		//���ö�ʱ��ֵ
	TH0 = 0xD4;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	EA=1;
	ET0=1;
}

void Time_0() interrupt 1
{
	static uint count=0,C=0,led=0;
	count++;C++;
	if(count>=10)		//ÿ10������һ���Ƿ��а�������
	{
		count=0;
		key_flag=1;
	}
	//����ܶ�̬��ʾ
	if(C>=2)
	{
		C=0;
		P0=t_com[p];P2=0xc0;P2=0x00;
		P0=~t_display[number[p]];P2=0xe0;P2=0x00;
		p++;
	}
	if(p>=8)
		p=0;
	rd_count++;		//ʱ�ӵ����ʱ
	
	//�˴�Ϊled��˸����
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
	if(led>=2000)	//���ܴ������⣬�˴�led����˸�ظ����飬���Խ�ԭ����5s��5000ms����Ϊ��2s��2000ms��
	{							//��ʵ����˸��ʱ��ȷʵΪ5s��
		led=0;
		led_flag=0;
		P2 = 0x80;P0 = 0xff;P2 = 0;
	}
}

void key_function()
{
	//ʱ����ʾ����
	if(Trg&0x01)
	{
		if(flag==1&&Time_set==0)
		{
			flag=0;
			Time_Stop(Time);
		}
		Time_set++;
	}
	//ʱ����ʾģʽ�µ�ʱ����ļӼ�
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
	
	//������ʾ����
	if(Trg&0x02)
	{
		mode=++mode%4;
		Clock_set++;
	}
	//���������µ�ʱ����ļӼ�
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
	//�¶��źſ���
	if(flag==1&&mode==0&&Trg&0x08)
	{
		Delay10ms();
		if(Trg&0x08)
		{
			temp_flag++;
		}
	}
}

void time_math()		//ʱ����ʾ����ֵ���裬����number[2]��number[5]�̶�ΪС����
{
		number[0]=Time[0]>=0 ? Time[0]%10 : 0;
		number[1]=Time[0]>=10 ? Time[0]/10 : 0;
		number[3]=Time[1]>=0 ? Time[1]%10 : 0;
		number[4]=Time[1]>=10 ? Time[1]/10 : 0;
		number[6]=Time[2]>=0 ? Time[2]%10 : 0;
		number[7]=Time[2]>=10 ? Time[2]/10 : 0;
		number[2]=number[5]=17;
}
//ʱ�������������������������
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


void time_math2()		//�������ã���ֵ���裬����number[2]��number[5]�̶�ΪС����
{
	number[0]=Clock[0]>=0 ? Clock[0]%10 : 0;
	number[1]=Clock[0]>=10 ? Clock[0]/10 : 0;
	number[3]=Clock[1]>=0 ? Clock[1]%10 : 0;
	number[4]=Clock[1]>=10 ? Clock[1]/10 : 0;
	number[6]=Clock[2]>=0 ? Clock[2]%10 : 0;
	number[7]=Clock[2]>=10 ? Clock[2]/10 : 0;
	number[2]=number[5]=17;
}
//���������������������������
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
	setTime(initTime);		//���ó�ʼʱ��23��59��50
	flag=1;		//Ĭ��Ϊ����״̬
	mode=0;		//Ĭ����ʱ����ʾģʽ
	led_flag=0;
	while(DS18B20_Get()*0.0625==100);//�ȴ�Ĭ������100��ʾ����
	while(1)
	{
		int a=520;
		tt = DS18B20_Get();	//��ȡ�¶�
		tt = tt * 0.0625;
		//��ͬģʽ��mode���µĹ���
				if(mode==0&&temp_flag%2==0)	//ʱ������ģʽ
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
				if(key_flag==1)		//�԰����жϲ���������
				{
					key_flag=0;
					button();
					key_function();
				}
			}
			else if(mode!=0&&temp_flag%2==0)//��������ģʽ
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
				if(key_flag==1)		//�԰����жϲ���������
				{
					key_flag=0;
					button();
					key_function();
				}
			}
		//��������ʱ�����õ�ѡ�����ã��������õ�����
		if(rd_count>=100)		//ÿ��1s����Ӧʱ��仯
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
		//��ѯ����ʱ���Ƿ���
		if(Time[1]==Clock[1]&&Time[2]==Clock[2])
		{
			if(Time[0]==Clock[0]||Time[0]-1==Clock[0]||Time[0]-2==Clock[0]||Time[0]-3==Clock[0]||Time[0]-4==Clock[0])
				led_flag=1;
			if(key_flag==1)		//�԰����жϲ���������
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
		//��ʾ�¶�
		if(temp_flag%2==1)
		{
			if(key_flag==1)		//�԰����жϲ���������
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