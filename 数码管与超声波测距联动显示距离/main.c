#include <STC89C5xRC.H>
#include "intrins.h"

sbit DIO = P1^2;
sbit CLK = P1^3;
sbit TRIG = P1^4;
sbit ECHO = P1^5;

void Delay10us() {          // 12MHz??,?10us
    unsigned char i;
    _nop_();
    i = 2;
    while (--i);
}

void Trigger() {
    TRIG = 0;               // ???????
    TRIG = 1;               // ??
    Delay10us();            // ????10us
    TRIG = 0;               // ??,????
}

void Timer0_Init() {
    TMOD &= 0xF0;           // ??T1??
    TMOD |= 0x01;           // T0??1:16????
    TH0 = 0; TL0 = 0;       // ??
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

void main()
{
	
	unsigned int time, distance;
  unsigned int timeout; 
	unsigned char dis[4];  
  Timer0_Init();
	displayctrl(0, 0);
  delayms(100);

    while(1) {
        Trigger();          

        
        timeout = 0;
        while(ECHO == 0 && timeout++ < 60000);
        if(timeout >= 60000) continue;

        TR0 = 1;            

        
        timeout = 0;
        while(ECHO == 1 && timeout++ < 60000);
        TR0 = 0;            

        if(timeout >= 60000) continue;

        
        time = (TH0 << 8) | TL0;
        TH0 = 0; TL0 = 0;   

  
        distance = time / 58;
				
				dis[0]=num[distance/10];
				dis[1]=num[distance%10];
				dis[2]=num[0];
				dis[3]=num[0];
				
				if(distance>40)
					P2=7;
				else if(distance>20)
					P2=3;
				else if(distance>10)
					P2=1;
				else 
					P2=0;
				
				display(dis);
				delayms(200);
        
    }
    
}