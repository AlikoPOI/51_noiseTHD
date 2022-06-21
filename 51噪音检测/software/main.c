#include <reg51.h>
#include <string.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int
	
uchar code table0[] = {"noiseTHD  set:20"}; // 每行的字符数据
uchar code table1[] = {"dB:00"};
uchar code table2[] = {"0123456789"};
sbit lcdrs = P2^5; // LCD数据/命令选择端
sbit lcdrw = P2^6; // LCD读写选择端
sbit lcden = P2^7; // LCD使能端，高脉冲有效
sbit k1 = P0^0;
sbit beep = P0^1;
uchar num, ge = 0, shi = 0;
short int i=0;

void init();
void displayInit();
void sigScan();
void writeCommand(uchar command);
void delay(uchar xms);
void writeData(uchar dat);
void InitTimer0(void);

void main()
{
	init();
	InitTimer0();
	displayInit();
	beep = 0;
	while(1)
	{	
		sigScan();
		if(shi>=2)
			{
				beep = 1;
				delay(1000);
				beep = 0;
			}
	}
}

/*初始化LCD1602的设置*/
void init()
{
	lcden = 0; // 拉低使能端，准备产生使能高脉冲信号
	writeCommand(0x38); // 显示模式设置(16x2， 5x7点阵，8位数据接口)
	writeCommand(0x0c); // 开显示，不显示光标
	writeCommand(0x06); // 写一个字符后地址指针自动加1
	writeCommand(0x01); // 显示清零，数据指针清零
}

/*LCD上电界面*/
void displayInit()
{
	writeCommand(0x80); // 将数据指针定位到第一行首
	for(num = 0; num < strlen(table0); num++)
	{
		writeData(table0[num]);
		delay(5);
	}
	
	writeCommand(0x80 + 0x40); // 将数据指针定位到第二行首
	for(num = 0; num < strlen(table1); num++)
	{
		writeData(table1[num]);
		delay(5);
	}
}

/*按键扫描*/
void sigScan()
{
	writeCommand(0x80 + 0x40 + strlen(table1) - 0x01);
	if(!k1)
	{

//			while(!k1);
			ge++;
			if(ge == 10)
			{
				ge = 0;
				shi++;
				if(shi == 10)
				{
					shi = 0;
					displayInit();
				}else{
					writeCommand(0x80 + 0x40 + strlen(table1) - 0x02);
					writeData(table2[shi]);
					writeCommand(0x80 + 0x40 + strlen(table1) - 0x01);
					writeData(table2[ge]);
				}
			}else{
				writeData(table2[ge]);
			}
		}
}




/*写指令函数*/
void writeCommand(uchar command)
{
	lcdrs = 0; // 命令选择
	lcdrw = 0;
	P1 = command;
	delay(5);
	
	lcden = 1; // 产生一个正脉冲使能信号
	delay(5);
	lcden = 0;
}

/*写数据函数*/
void writeData(uchar dat)
{
	lcdrs = 1; // 数据选择
	lcdrw = 0;
	P1 = dat;
	delay(5);
	
	lcden = 1;
	delay(5);
	lcden = 0;
}

/*延时函数*/
void delay(uchar xms)
{
	uint i, j;
	for(i = xms; i > 0; i--)
		for(j = 110; j > 0; j--);
}


//*****timer*****//
void InitTimer0(void)
{
    TMOD = 0x01;
    TH0 = 0x3C;
    TL0 = 0x0B0;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

void Timer0Interrupt(void) interrupt 1
{
    TH0 = 0x3C;
    TL0 = 0x0B0;
    //add your code here!
		i++;
	if(i>=20)
	{
		ge=0;
		shi=0;
		i=0;
	}
}

//*****timer*****//