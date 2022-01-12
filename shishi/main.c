#include "reg51.h"
#include "lcd.h"

void upTime();
void  month29(int y);
void week7(int day, int month, int year);
void movePoint();
void isPoint();
void display();
void add_second();
void add_minute();
void add_hour();
void add_day();
void add_month();

typedef unsigned char u8;
typedef unsigned int u16;

u8 Weeks[7][10] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
u16 Months[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
u16 texts[6] = {2022,12,31,23,59,55};
int Week = 5 ;

sbit k1 = P3^1;
sbit k3 = P3^2;
sbit k2 = P3^0;
sbit k4 = P3^3;
int inum = 0; //��ʱ������
int flag = 1;
int setIndex = 0;
u16 add[6] = {0x83,0x86,0x89,0x80+0x40+5,0x80+0x40+8,0x80+0x40+11};

// ���ö�ʱ��������1000��������1ms����ʱ����һ���ж�
void TimeOut0()
{
	TMOD|=0x01;
	TH0=0xFC;
	TL0=0x18;									                                                                                                                                                                                                     
	ET0=1;
	EA=1;
	TR0=1;
}

void delay(int j)
{
 	while(j--);
}

//�жϰ�������
void keydown()
{
	if(k3==0)
	{
		delay(1000);
		if(k3==0)
		{
			flag = !flag;
			isPoint();
		}

		while(!k3);
	}
	if(k1==0&&!flag)
	{
		delay(1000);
		if(k1==0)
		{
			setIndex ++  ;
			if(setIndex==6)setIndex=0;
			LcdWriteCom(add[setIndex]);
		}

		while(!k1);
	}
	if(k2==0&&!flag)
	{
		delay(1000);
		if(k2==0)
		{
			if(setIndex==0)
			{
				texts[0]++;
				week7(texts[2],texts[1],texts[0]);
			}
			else if(setIndex==1)
			{
				add_month();
				week7(texts[2],texts[1],texts[0]);
			}
			else if(setIndex==2)add_day();
			else if(setIndex==3)add_hour();
			else if(setIndex==4)add_minute();
			else if(setIndex==5)add_second();
			display();
			LcdWriteCom(add[setIndex]);
		}

		while(!k2);
	}
	// ��չ k4 ��ťΪ��һ������ť
	if(k4==0&&!flag)
	{
		delay(1000);
		if(k4==0)
		{
			if(setIndex<3)
			{
				//��Ӧ���ܵ��� 0�� 0 �� 0��
				if(texts[setIndex]==1)texts[setIndex]=2;
			}
			else
			{
				// �������� 0 ʱ 0�� 0��
				if(texts[setIndex]==0)texts[setIndex]=1;
			}
			texts[setIndex]--;
			month29(texts[0]);
			week7(texts[2],texts[1],texts[0]);
			display();
			LcdWriteCom(add[setIndex]);
			
		}

		while(!k4);
	}
	
}

void display()
{
	//LcdWriteCom(0x80);
	//��
	 LcdWriteCom(0x80);
	LcdWriteData('0'+texts[0]/1000);
	LcdWriteData('0'+texts[0]/100%10);
	LcdWriteData('0'+texts[0]/10%10);
	LcdWriteData('0'+texts[0]%10);
	LcdWriteData('-');
	//��
	LcdWriteData('0'+texts[1]/10);
	LcdWriteData('0'+texts[1]%10);
	LcdWriteData('-');
	//��
	LcdWriteData('0'+texts[2]/10);
	LcdWriteData('0'+texts[2]%10);
	LcdWriteData(' ');
	LcdWriteData(' ');
	LcdWriteData(' ');
	//��
	LcdWriteData(Weeks[Week][0]);
	LcdWriteData(Weeks[Week][1]);
	LcdWriteData(Weeks[Week][2]);

    LcdWriteCom(0x80+0X40);
	LcdWriteData(' ');
	LcdWriteData(' ');;
	LcdWriteData(' ');
	LcdWriteData(' ');

	//ʱ

	LcdWriteData('0'+texts[3]/10);				
	LcdWriteData('0'+texts[3]%10);				 
	LcdWriteData('-');
	//��
	LcdWriteData('0'+texts[4]/10);				
	LcdWriteData('0'+texts[4]%10);	
	LcdWriteData('-');
	//��
	LcdWriteData('0'+texts[5]/10);				
	LcdWriteData('0'+texts[5]%10);

}

void main()
{
	LcdInit();
	TimeOut0();
	display();

	while(1)
	{
		keydown();
	}
}

// �жϣ������ʱ���������ж�  ��һ�����һ���ж�
void Time0() interrupt 1
{
	if(flag)
	{
		
		TH0=0xFC;
		TL0=0x18;
		inum++;
		if(inum==1000)
		{
			inum=0;
			upTime();
		}
	}
}

//����ʱ��
void upTime()
{
	add_second();
	display();
}
void add_second()
{
	texts[5]++;
	if(texts[5]>=60)
	{
		 texts[5] = 0 ;
		 add_minute();
	}
}
void add_minute()
{
	texts[4]++;
	if(texts[4]>=60)
	{
		 texts[4] = 0;
		 add_hour();
	}
}
void add_hour()
{
	 texts[3]++;
	if(texts[3]>=24)
	{
		 texts[3] = 0 ;
		 add_day();
	}
}
void add_day()
{
	texts[2]++;
	Week++;
	if(Week>=7)Week=0;
	if(texts[2]>=Months[texts[1]-1]+1)
	{
		 texts[2] = 1;
		 add_month();
	}
}
void add_month()
{
	texts[1]++;
	month29(texts[0]);
	if(texts[1]>=13)
	{
		 texts[0]++;
		 texts[1]=1;
	}
}
//���������ж�����
void  month29(int y)
{
	if(y%400==0||(y%4==0&&y%100!=0))
	{
		Months[1]=29;
	}
	else
	{
		Months[1]=28;
	}
}
//���������ж��ܼ�
void week7(int day, int month, int year)
{
    /* �������֮ǰ����ݵ��������� */
    u16 days = 365 * (year - 1971) + (year - 1969) / 4;
    /* ��������У������·�֮ǰ���·ݵ��������� */
	int i = 0;
    for (; i < month - 1; ++i) {
        days += Months[i];
    }
    if ((year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) && month >= 3) {
        days += 1;
    }
    /* �����·��е��������� */
    days += day;
	//���������±�
	Week = (days+3)%7;
}
//�򿪹�꣬��ʾ���ڸ���ĳλ
void isPoint()
{
	if(!flag)
	{
		LcdWriteCom(0x0E);
		setIndex=0;
		LcdWriteCom(add[setIndex]);
	}
	else LcdWriteCom(0x0C);


}