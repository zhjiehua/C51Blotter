#include "io.h"
#include "common.h"
#include "uart.h"

//IO初始化
void IO_Init(void)
{
	P0M1 = 0x0F;    //双双双双高高高高
	P0M0 = 0x00;
	P0 = 0xFF;    //EN_MOT06,EN_MOT05,EN_MOT04,EN_MOT01,EAIN8,EAIN7,EAIN6,EAIN5
	//----------------------
	P1M1 = 0x00;    //双双双双双双双双
	P1M0 = 0x00;    
	P1 = 0xFF;    //XTAL1,XTAL2,EN_MOT11,EN_MOT10,EN_MOT09,EN_MOT08,LCD_DIN,LCD_DOUT
	//----------------------
	P2M1 = 0xC1;   //高高双双双双双开
	P2M0 = 0x01;   //
	P2 = 0xFF;    //EAIN2,EAIN1,S_DIN2,S_DIN1,C_PWM3,C_PWM2,C_PWM1,WSDA
	//----------------------
	P3M1 = 0x00;    //推推推推双双双双
	P3M0 = 0xF0;
	P3 = 0x0F;    //S_SLEEP1,S_DIR1,S_STEP1,S_STEP2,INT_RTC,INT_INPUT,M_TXD,M_RXD
	//----------------------
	P4M1 = 0x71;    //双高高开推推推开
	P4M0 = 0x1F;
	P4 = 0xF1;    //EN_MOT07,EAIN4,EAIN3,MSCL,DRV_BEEP,DRV_REL,S_DIR2,LED1
	
	P5M1 = 0x20;    //双双开双双双推推
	P5M0 = 0x23;
	P5 = 0xFC;    //---,---,LED2,EN_MOT12,EN_MOT03,EN_MOT02,S_ENBL1,S_ENBL2

//	PCON = 0x00;
//	SCON = 0x40; //RS232 MODE 1
//	//TMOD = 0x00; //T0,TIMER0,16位计时,T1 TIMER0,16位自动重装
//	//TCON = 0x00; //
//	//AUXR = 0xD5; //T0/T1/T2不分频,;T2为串口1的波特率发生器

	AUXR1 = 0x00; //使用DPTR0,CCP在P24P25P26,串口1在P30/P31
	P_SW2 = 0x00; //串口2在P10/P11
	P1ASF = 0x00; //无模拟输入
	ADC_CONTR = 0x00; //AD未启动
//	//P4SW = 0x70; //P46P45P44设为IO口
//	//WAKE_CLKO = 0x00; //不输出时钟
	CLK_DIV = 0x00; //时钟不分频,不输出主时钟

//	IE = 0x00; //
//	IE2 = 0x00; //
//	//IPH0 = 0x15;
//	IP = 0x10; //---PCA,LVD,ADC,UART,T1,EX1,T0,EX0,UART高优先级
//	//BRT = 220;                  ;独立波特率,9600
//	//-------------------------------------------------------------
//	//T2H = 0xFE;            //9600波特率(11.0592MHz,1T)
//	//T2L = 0xE0;
//	
//	T2H = 0xFF;            //9600波特率(8MHz,1T)
//	T2L = 0x30;
//	
//	//T2H = 0xFD;            //9600波特率(24MHz,1T)
//	//T2L = 0x8F;
}

/*
 * 蜂鸣器驱动
 * 参数：mode: 0:一直响
 *		 	   1:鸣叫5声
 */
void beepAlarm(unsigned char time)
{
	if(time == 0)
		DRV_BEEP = 1;  //一直响
	else
	{
		DRV_BEEP = 1;
		os_wait(K_TMO, 50*time, 0);  //等待100个时钟滴答(ticks),即1s
		DRV_BEEP = 0;
		os_wait(K_TMO, 50, 0);  //等待100个时钟滴答(ticks),即1s
	}
}
