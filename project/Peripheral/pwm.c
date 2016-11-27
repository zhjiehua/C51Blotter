#include "pwm.h"

pMotorFun_Typedef pDCMotorEnable[DCMOTOR_COUNT];

static void dcMotor1Enable(Status sta){ sta ? (EN_MOT01 = 0) : (EN_MOT01 = 1);}
static void dcMotor2Enable(Status sta){ sta ? (EN_MOT02 = 0) : (EN_MOT02 = 1);}
static void dcMotor3Enable(Status sta){ sta ? (EN_MOT03 = 0) : (EN_MOT03 = 1);}
static void dcMotor4Enable(Status sta){ sta ? (EN_MOT04 = 0) : (EN_MOT04 = 1);}
static void dcMotor5Enable(Status sta){ sta ? (EN_MOT05 = 0) : (EN_MOT05 = 1);}
static void dcMotor6Enable(Status sta){ sta ? (EN_MOT06 = 0) : (EN_MOT06 = 1);}
static void dcMotor7Enable(Status sta){ sta ? (EN_MOT07 = 0) : (EN_MOT07 = 1);}
static void dcMotor8Enable(Status sta){ sta ? (EN_MOT08 = 0) : (EN_MOT08 = 1);}
static void dcMotor9Enable(Status sta){ sta ? (EN_MOT09 = 0) : (EN_MOT09 = 1);}
static void dcMotor10Enable(Status sta){ sta ? (EN_MOT10 = 0) : (EN_MOT10 = 1);}
static void dcMotor11Enable(Status sta){ sta ? (EN_MOT11 = 0) : (EN_MOT11 = 1);}
static void dcMotor12Enable(Status sta){ sta ? (EN_MOT12 = 0) : (EN_MOT12 = 1);}

//PWM��ʼ��
void PWM_Init(void)
{
	uint8_t i;

	pDCMotorEnable[0] = dcMotor1Enable;
	pDCMotorEnable[1] = dcMotor2Enable;
	pDCMotorEnable[2] = dcMotor3Enable;
	pDCMotorEnable[3] = dcMotor4Enable;
	pDCMotorEnable[4] = dcMotor5Enable;
	pDCMotorEnable[5] = dcMotor6Enable;
	pDCMotorEnable[6] = dcMotor7Enable;
	pDCMotorEnable[7] = dcMotor8Enable;
	pDCMotorEnable[8] = dcMotor9Enable;
	pDCMotorEnable[9] = dcMotor10Enable;
	pDCMotorEnable[10] = dcMotor11Enable;
	pDCMotorEnable[11] = dcMotor12Enable;

	//ֹͣ����ֱ�����
//	EN_MOT01 = 1;
//	EN_MOT02 = 1;
//	EN_MOT03 = 1;
//	EN_MOT04 = 1;
//	EN_MOT05 = 1;
//	EN_MOT06 = 1;
//	EN_MOT07 = 1;
//	EN_MOT08 = 1;
//	EN_MOT09 = 1;
//	EN_MOT10 = 1;
//	EN_MOT11 = 1;
//	EN_MOT12 = 1;
	for(i=0;i<DCMOTOR_COUNT;i++)
	{
		pDCMotorEnable[i](DISABLE);
	}

	P_SW2 |= 0x80;  				//ʹ�ܷ���XSFR
	PWMCFG = 0x00;                  //����PWM�������ʼ��ƽΪ�͵�ƽ
    PWMCKS = 0x00;                  //ѡ��PWM��ʱ��ΪFosc/(0+1)
    PWMC = CYCLE;                   //����PWM����

    PWM3T1 = 0x0000;                //����PWM3��1�η�ת��PWM����
    PWM3T2 = CYCLE * DUTY / 100;    //����PWM3��2�η�ת��PWM����
                                    //ռ�ձ�Ϊ(PWM3T2-PWM3T1)/PWMC
    PWM3CR = 0x00;                  //ѡ��PWM3�����P2.1,��ʹ��PWM3�ж�

#ifdef PWM4OUT
	PWM4T1 = 0x0000;                //����PWM4��1�η�ת��PWM����
    PWM4T2 = CYCLE * DUTY / 100;    //����PWM4��2�η�ת��PWM����
                                    //ռ�ձ�Ϊ(PWM4T2-PWM4T1)/PWMC
    PWM4CR = 0x00;                  //ѡ��PWM4�����P2.2,��ʹ��PWM4�ж�

	PWMCR = 0x06;                   //ʹ��PWM3 PWM4�ź����
#else
    PWMCR = 0x02;                   //ʹ��PWM3�ź����
#endif

    PWMCR |= 0x80;                  //ʹ��PWMģ��
    P_SW2 &= ~0x80;
}

void PWM3_SetPwmWide(unsigned short Wide)
{
    if (Wide == 0)
    {
        PWMCR &= ~0x02;
        PWM3 = 0;
    }
    else if (Wide == CYCLE)
    {
        PWMCR &= ~0x02;
        PWM3 = 1;
    }
    else
    {
        P_SW2 |= 0x80;
        PWM3T1 = Wide;
        P_SW2 &= ~0x80;
        PWMCR |= 0x02;
    }
}


