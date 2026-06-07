#include <STC89C5xRC.H>
#include "intrins.h"

sbit DIO = P1^2;
sbit CLK = P1^3;


void Delay10us() {          // 12MHz??,?10us
    unsigned char i;
    _nop_();
    i = 2;
    while (--i);
}

unsigned char code num[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void delay_us(unsigned int time)
{
    unsigned int i;
    for(i = 0; i < time; i++)
    {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }
}

void delayms(unsigned int time)
{
    unsigned char i, j;
    while(time--)
    {
        i = 2;
        j = 239;
        do
        {
            while (--j);
        } while (--i);
    }
}

void write(unsigned char dat)
{
    unsigned char i;
    
    for(i = 0; i < 8; i++)
    {
        CLK = 0;
        _nop_();
        if(dat & 0x01)
            DIO = 1;
        else
            DIO = 0;
        _nop_();
        CLK = 1;
        _nop_();
        dat >>= 1;
    }
    
    // ACK
    CLK = 0;
    _nop_();
    _nop_();
    DIO = 1;
    _nop_();
    _nop_();
    CLK = 1;
    _nop_();
    _nop_();
    CLK = 0;
}

void start(void)
{
    CLK = 1;
    DIO = 1;
    delay_us(2);
    DIO = 0;
    delay_us(2);
		CLK = 0;
		delay_us(2);
}

void stop(void)
{
    CLK = 0;
    delay_us(2);
    DIO = 0;
    delay_us(2);
    CLK = 1;
    delay_us(2);
    DIO = 1;
    delay_us(2);
}

void displayctrl(unsigned char bri, unsigned char on_off)
{
    unsigned char cmd = 0x80;
    if(on_off)
    {
        cmd = 0x88 | (bri & 0x07);
    }
    start();
    write(cmd);
    stop();
}

void display(unsigned char *dat)
{
    unsigned char i;
    
    start();
    write(0x40);
    stop();
    
    start();
    write(0xC0);
    for(i = 0; i < 4; i++)
    {
        write(dat[i]);
    }
    stop();
    
    displayctrl(7, 1);
}


void timestart()
{
	TMOD = 0x01;
	TH0 = (65536-50000)/256;
	TL0 = (65536-50000)%256;
	TR0=1;
}


void main()
{
	
	unsigned int time,k;
	unsigned char arr[4];
	displayctrl(0, 0);
  delayms(100);
	timestart();
	TF0 =0;
	k=0;
	time = 0;
	while(time<=1000)
	{
		k=0;
		while(k<20)
		{
			while(TF0==0);
			TH0 = (65536-50000)/256;
			TL0 = (65536-50000)%256;
			TF0 = 0;
			k++;
		}
		
		time++;
		arr[3]=num[(time%10)];
		arr[2]=num[(time%100)/10];
		arr[1]=num[(time%1000)/100];
		arr[0]=num[time/1000];
		
		display(arr);
		delayms(200);
	}
}
