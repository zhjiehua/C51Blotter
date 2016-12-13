#include "dcmotor.h"
#include "sensor.h"
#include "uart.h"

static DCMotor_TypeDef dcMotor;
DCMotor_TypeDef *pDCMotor = &dcMotor;

static void DCMotor_SetCMD(DCMotorEnum_TypeDef num, Status sta)
{
	switch(num)
	{
		case DCMOTOR1:
			sta ? (EN_MOT01 = 0) : (EN_MOT01 = 1);
			break;
		case DCMOTOR2:
			sta ? (EN_MOT02 = 0) : (EN_MOT02 = 1);
			break;
		case DCMOTOR3:
			sta ? (EN_MOT03 = 0) : (EN_MOT03 = 1);
			break;
		case DCMOTOR4:
			sta ? (EN_MOT04 = 0) : (EN_MOT04 = 1);
			break;
		case DCMOTOR5:
			sta ? (EN_MOT05 = 0) : (EN_MOT05 = 1);
			break;
		case DCMOTOR6:
			sta ? (EN_MOT06 = 0) : (EN_MOT06 = 1);
			break;
		case DCMOTOR7:
			sta ? (EN_MOT07 = 0) : (EN_MOT07 = 1);
			break;
		case DCMOTOR8:
			sta ? (EN_MOT08 = 0) : (EN_MOT08 = 1);
			break;
		case DCMOTOR9:
			sta ? (EN_MOT09 = 0) : (EN_MOT09 = 1);
			break;
		case DCMOTOR10:
			sta ? (EN_MOT10 = 0) : (EN_MOT10 = 1);
			break;
		case DCMOTOR11:
			sta ? (EN_MOT11 = 0) : (EN_MOT11 = 1);
			break;
		case DCMOTOR12:
			sta ? (EN_MOT12 = 0) : (EN_MOT12 = 1);
			break;
		default:
			break;
	}
}

//设置直流电机的速度
//其实是设置PWM的占空比，0~100,100为最大占空比
//对于DCMOTOR10：CW方向时，100表示不转，0为最大速度；CCW方向时，0表示不转，100为最大速度
//对于DCMOTOR1~9：100表示不转，0为最大速度；
static void DCMotor_SetSpeed(DCMotorEnum_TypeDef num, uint16_t speed)
{
#if 0  //改变PWM周期
//	PWMCR &= ~0x80;                 //除能PWM模块
//	P_SW2 |= 0x80;  				//使能访问XSFR
//	PWMC = speed;                   //设置PWM周期
//	PWM3T2 = speed * DUTY / 100;    //设置PWM3第2次反转的PWM计数
//#ifdef PWM4OUT
//    PWM4T2 = speed * DUTY / 100;    //设置PWM4第2次反转的PWM计数
//	PWMCR = 0x06;                   //使能PWM3 PWM4信号输出
//#else
//    PWMCR = 0x02;                   //使能PWM3信号输出
//#endif
//    P_SW2 &= ~0x80;
//	PWMCR |= 0x80;                  //使能PWM模块
#endif
	
	switch(num)
	{
		case DCMOTOR1:
		case DCMOTOR2:
		case DCMOTOR3:
		case DCMOTOR4:
		case DCMOTOR5:
		case DCMOTOR6:
		case DCMOTOR7:
		case DCMOTOR8:
		case DCMOTOR9:
			if(speed == 0)
		    {
		        PWMCR &= ~0x02;
		        PWM3 = 0;
		    }
		    else if(speed == 100)
		    {
		        PWMCR &= ~0x02;
		        PWM3 = 1;
		    }
		    else
		    {
		        P_SW2 |= 0x80;
				PWM3T1 = 0x0000;                //设置PWM3第1次反转的PWM计数
		    	PWM3T2 = CYCLE * speed / 100;    //设置PWM3第2次反转的PWM计数
		                                    //占空比为(PWM3T2-PWM3T1)/PWMC
		        P_SW2 &= ~0x80;
		        PWMCR |= 0x02;
		    }
			break;
		case DCMOTOR10:
			if(speed == 0)
		    {
		        PWMCR &= ~0x04;
		        PWM4 = 0;
		    }
		    else if(speed == 100)
		    {
		        PWMCR &= ~0x04;
		        PWM4 = 1;
		    }
		    else
		    {
		        P_SW2 |= 0x80;
				PWM4T1 = 0x0000;                //设置PWM4第1次反转的PWM计数
		    	PWM4T2 = CYCLE * speed / 100;    //设置PWM4第2次反转的PWM计数
		                                    //占空比为(PWM4T2-PWM4T1)/PWMC
		        P_SW2 &= ~0x80;
		        PWMCR |= 0x04;
		    }
			break;
		case DCMOTOR11:
		case DCMOTOR12:
			if(speed == 0)
		    {
		        PWMCR &= ~0x08;
		        PWM5 = 1;
		    }
		    else if(speed == 100)
		    {
		        PWMCR &= ~0x08;
		        PWM5 = 0;
		    }
		    else
		    {
		        P_SW2 |= 0x80;
				PWM5T1 = 0x0000;                //设置PWM5第1次反转的PWM计数
		    	PWM5T2 = CYCLE * speed / 100;    //设置PWM5第2次反转的PWM计数
		                                    //占空比为(PWM5T2-PWM5T1)/PWMC
		        P_SW2 &= ~0x80;
		        PWMCR |= 0x08;
		    }
			break;
		default:
			break;
	}	
}

//设置废液口电机（直流泵10）的方向
static void DCMotor_SetDir(DCMotorEnum_TypeDef num, Direction_TypeDef dir)
{
	switch(num)
	{
		case DCMOTOR1:
		case DCMOTOR2:
		case DCMOTOR3:
		case DCMOTOR4:
		case DCMOTOR5:
		case DCMOTOR6:
		case DCMOTOR7:
		case DCMOTOR8:
		case DCMOTOR9:
			break;
		case DCMOTOR10:	  //目前只有废液泵有方向设置
			dir ? (MOT10_DIR = 0) : (MOT10_DIR = 1);
			break;
		case DCMOTOR11:
		case DCMOTOR12:
			break;
		default:
			break;
	}	
}

static void DCMotor_SetPos(uint8_t pos)
{
	pDCMotor->curCount = 0;
	pDCMotor->desCount = pos;	
}

static void DCMotor_UpdatePos(void)
{
	pDCMotor->curCount++;		
}

static uint8_t DCMotor_IsOnPos()
{
	return (pDCMotor->curCount >= pDCMotor->desCount); 		
}

static void DCMotor_WastePump_SetPos(Position_TypeDef pos)
{
	if(pos == UP)
	{
		pSensor->SetCheckEdge(RASINGEDGE);

		pDCMotor->SetPos(1);
		pDCMotor->SetSpeed(PUMP_WASTE, 100-30);
		//pDCMotor->SetSpeed(PUMP_WASTE, 100); //CW方向时，100表示不转，0为最大速度
		pDCMotor->SetDir(PUMP_WASTE, CW);
		pDCMotor->SetCMD(PUMP_WASTE, ENABLE);

		while(!pDCMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_UP))
				pDCMotor->UpdatePos();	
		}
		pDCMotor->SetCMD(PUMP_WASTE, DISABLE);		
	}
	else
	{
		pSensor->SetCheckEdge(RASINGEDGE);
	
		pDCMotor->SetPos(1);
		pDCMotor->SetSpeed(PUMP_WASTE, 30);
		//pDCMotor->SetSpeed(PUMP_WASTE, 100);  //CCW方向时，0表示不转，100为最大速度
		pDCMotor->SetDir(PUMP_WASTE, CCW);
		pDCMotor->SetCMD(PUMP_WASTE, ENABLE);

		while(!pDCMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_DOWN))
				pDCMotor->UpdatePos();	
		}

		pDCMotor->SetCMD(PUMP_WASTE, DISABLE);
		pDCMotor->SetDir(PUMP_WASTE, CW);//废液泵电机停止的时候要拉高方向引脚		
	}
}

//PWM初始化
void DCMotor_Init(void)
{ 
	uint8_t i;

	pDCMotor->control = 0x00;
	pDCMotor->SetCMD = DCMotor_SetCMD;
	pDCMotor->SetSpeed = DCMotor_SetSpeed;
	pDCMotor->SetDir = DCMotor_SetDir;

	pDCMotor->SetPos = DCMotor_SetPos;
	pDCMotor->UpdatePos = DCMotor_UpdatePos;
	pDCMotor->IsOnPos = DCMotor_IsOnPos;

	pDCMotor->WastePump_SetPos = DCMotor_WastePump_SetPos;

	for(i=0;i<DCMOTOR_COUNT;i++)
	{
		pDCMotor->SetCMD(i, DISABLE); //停止所有電機
	}
	pDCMotor->SetDir(PUMP_WASTE, CW); //废液泵电机停止的时候要拉高方向引脚

	P_SW2 |= 0x80;  				//使能访问XSFR
	PWMCFG = 0x00;                  //配置PWM的输出初始电平为低电平
    PWMCKS = 0x00;                  //选择PWM的时钟为Fosc/(0+1)
    PWMC = CYCLE;                   //设置PWM周期

    PWM3T1 = 0x0000;                //设置PWM3第1次反转的PWM计数
    PWM3T2 = CYCLE * DUTY / 100;    //设置PWM3第2次反转的PWM计数
                                    //占空比为(PWM3T2-PWM3T1)/PWMC
    PWM3CR = 0x00;                  //选择PWM3输出到P2.1,不使能PWM3中断

	PWM4T1 = 0x0000;                //设置PWM4第1次反转的PWM计数
    PWM4T2 = CYCLE * DUTY / 100;    //设置PWM4第2次反转的PWM计数
                                    //占空比为(PWM4T2-PWM4T1)/PWMC
    PWM4CR = 0x00;                  //选择PWM4输出到P2.2,不使能PWM4中断

	PWM5T1 = 0x0000;                //设置PWM5第1次反转的PWM计数
    PWM5T2 = CYCLE * DUTY / 100;    //设置PWM5第2次反转的PWM计数
                                    //占空比为(PWM5T2-PWM5T1)/PWMC
    PWM5CR = 0x00;                  //选择PWM5输出到P2.3,不使能PWM5中断

	PWMCR = 0x0E;                   //使能PWM3 PWM4 PWM5信号输出

    PWMCR |= 0x80;                  //使能PWM模块
    P_SW2 &= ~0x80;
}

//void DCMotor_Test(void)
//{
//	if(pDCMotor->control == 0x01)
//	{
//		pSensor->SetCheckEdge(RASINGEDGE);
//	
//		pDCMotor->SetPos(1);
//		pDCMotor->SetSpeed(DCMOTOR1, 10);
//		pDCMotor->SetDir(DCMOTOR1, CW);
//		pDCMotor->SetCMD(DCMOTOR1, ENABLE);
//
//		while(!pDCMotor->IsOnPos())
//		{
//			if(pSensor->GetStatus(SENSOR_UP))
//				pDCMotor->UpdatePos();	
//		}
//		pDCMotor->SetCMD(DCMOTOR1, DISABLE);
//
//		//pDCMotor->WastePump_SetPos(UP);
//
//		pDCMotor->control = 0x00;
//	}
//	else if(pDCMotor->control == 0x02)
//	{
//		pSensor->SetCheckEdge(RASINGEDGE);
//	
//		pDCMotor->SetPos(1);
//		pDCMotor->SetSpeed(DCMOTOR1, 90);
//		pDCMotor->SetDir(DCMOTOR1, CW);
//		pDCMotor->SetCMD(DCMOTOR1, ENABLE);
//
//		while(!pDCMotor->IsOnPos())
//		{
//			if(pSensor->GetStatus(SENSOR_UP))
//				pDCMotor->UpdatePos();	
//		}
//		pDCMotor->SetCMD(DCMOTOR1, DISABLE);
//
//		//pDCMotor->WastePump_SetPos(DOWN);
//
//		pDCMotor->control = 0x00;
//	}			
//} 