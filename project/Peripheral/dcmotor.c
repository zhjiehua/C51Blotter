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

//����ֱ��������ٶ�
//��ʵ������PWM��ռ�ձȣ�0~100,100Ϊ���ռ�ձ�
//����DCMOTOR10��CW����ʱ��100��ʾ��ת��0Ϊ����ٶȣ�CCW����ʱ��0��ʾ��ת��100Ϊ����ٶ�
//����DCMOTOR1~9��100��ʾ��ת��0Ϊ����ٶȣ�
static void DCMotor_SetSpeed(DCMotorEnum_TypeDef num, uint16_t speed)
{
#if 0  //�ı�PWM����
//	PWMCR &= ~0x80;                 //����PWMģ��
//	P_SW2 |= 0x80;  				//ʹ�ܷ���XSFR
//	PWMC = speed;                   //����PWM����
//	PWM3T2 = speed * DUTY / 100;    //����PWM3��2�η�ת��PWM����
//#ifdef PWM4OUT
//    PWM4T2 = speed * DUTY / 100;    //����PWM4��2�η�ת��PWM����
//	PWMCR = 0x06;                   //ʹ��PWM3 PWM4�ź����
//#else
//    PWMCR = 0x02;                   //ʹ��PWM3�ź����
//#endif
//    P_SW2 &= ~0x80;
//	PWMCR |= 0x80;                  //ʹ��PWMģ��
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
				PWM3T1 = 0x0000;                //����PWM3��1�η�ת��PWM����
		    	PWM3T2 = CYCLE * speed / 100;    //����PWM3��2�η�ת��PWM����
		                                    //ռ�ձ�Ϊ(PWM3T2-PWM3T1)/PWMC
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
				PWM4T1 = 0x0000;                //����PWM4��1�η�ת��PWM����
		    	PWM4T2 = CYCLE * speed / 100;    //����PWM4��2�η�ת��PWM����
		                                    //ռ�ձ�Ϊ(PWM4T2-PWM4T1)/PWMC
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
				PWM5T1 = 0x0000;                //����PWM5��1�η�ת��PWM����
		    	PWM5T2 = CYCLE * speed / 100;    //����PWM5��2�η�ת��PWM����
		                                    //ռ�ձ�Ϊ(PWM5T2-PWM5T1)/PWMC
		        P_SW2 &= ~0x80;
		        PWMCR |= 0x08;
		    }
			break;
		default:
			break;
	}	
}

//���÷�Һ�ڵ����ֱ����10���ķ���
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
		case DCMOTOR10:	  //Ŀǰֻ�з�Һ���з�������
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
		//pDCMotor->SetSpeed(PUMP_WASTE, 100); //CW����ʱ��100��ʾ��ת��0Ϊ����ٶ�
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
		//pDCMotor->SetSpeed(PUMP_WASTE, 100);  //CCW����ʱ��0��ʾ��ת��100Ϊ����ٶ�
		pDCMotor->SetDir(PUMP_WASTE, CCW);
		pDCMotor->SetCMD(PUMP_WASTE, ENABLE);

		while(!pDCMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_DOWN))
				pDCMotor->UpdatePos();	
		}

		pDCMotor->SetCMD(PUMP_WASTE, DISABLE);
		pDCMotor->SetDir(PUMP_WASTE, CW);//��Һ�õ��ֹͣ��ʱ��Ҫ���߷�������		
	}
}

//PWM��ʼ��
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
		pDCMotor->SetCMD(i, DISABLE); //ֹͣ����늙C
	}
	pDCMotor->SetDir(PUMP_WASTE, CW); //��Һ�õ��ֹͣ��ʱ��Ҫ���߷�������

	P_SW2 |= 0x80;  				//ʹ�ܷ���XSFR
	PWMCFG = 0x00;                  //����PWM�������ʼ��ƽΪ�͵�ƽ
    PWMCKS = 0x00;                  //ѡ��PWM��ʱ��ΪFosc/(0+1)
    PWMC = CYCLE;                   //����PWM����

    PWM3T1 = 0x0000;                //����PWM3��1�η�ת��PWM����
    PWM3T2 = CYCLE * DUTY / 100;    //����PWM3��2�η�ת��PWM����
                                    //ռ�ձ�Ϊ(PWM3T2-PWM3T1)/PWMC
    PWM3CR = 0x00;                  //ѡ��PWM3�����P2.1,��ʹ��PWM3�ж�

	PWM4T1 = 0x0000;                //����PWM4��1�η�ת��PWM����
    PWM4T2 = CYCLE * DUTY / 100;    //����PWM4��2�η�ת��PWM����
                                    //ռ�ձ�Ϊ(PWM4T2-PWM4T1)/PWMC
    PWM4CR = 0x00;                  //ѡ��PWM4�����P2.2,��ʹ��PWM4�ж�

	PWM5T1 = 0x0000;                //����PWM5��1�η�ת��PWM����
    PWM5T2 = CYCLE * DUTY / 100;    //����PWM5��2�η�ת��PWM����
                                    //ռ�ձ�Ϊ(PWM5T2-PWM5T1)/PWMC
    PWM5CR = 0x00;                  //ѡ��PWM5�����P2.3,��ʹ��PWM5�ж�

	PWMCR = 0x0E;                   //ʹ��PWM3 PWM4 PWM5�ź����

    PWMCR |= 0x80;                  //ʹ��PWMģ��
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