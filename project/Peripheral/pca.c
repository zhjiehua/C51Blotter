#include "pca.h"
#include "io.h"

#define F1200Hz (65536-FOSC/2/1200)
#define F19_2KHz (65536-FOSC/2/19200)
#define F38_4KHz (65536-FOSC/2/38400)    //1Tģʽ
//#define F38_4KHz (65536-FOSC/2/12/38400) //12Tģʽ

static StepMotor_TypeDef stepMotor;
StepMotor_TypeDef *pStepMotor = &stepMotor;

void Delay1us()		//@20.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 2;
	while (--i);
}

void Delay10us()		//@20.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 47;
	while (--i);
}

void Delay100us()		//@20.000MHz
{
	unsigned char i, j;

	i = 2;
	j = 239;
	do
	{
		while (--j);
	} while (--i);
}

void DelayNus(uint16_t n)		//@20.000MHz
{
	uint16_t i;

	for(i=0;i<n;i++)
		Delay1us();	
}

//PCA��ʼ��
void PCA_Init(void)
{
	uint16_t ret = F38_4KHz;
	ret = F19_2KHz;
	ret = F1200Hz;
#if 0
//	ACC = P_SW1;
//  ACC &= ~(CCP_S0 | CCP_S1);      //CCP_S0=0 CCP_S1=0
//  P_SW1 = ACC;                    //(P1.2/ECI, P1.1/CCP0, P1.0/CCP1, P3.7/CCP2)
    
	ACC = P_SW1;
	ACC &= ~(CCP_S0 | CCP_S1);      //CCP_S0=1 CCP_S1=0
	ACC |= CCP_S0;                  //(P3.4/ECI_2, P3.5/CCP0_2, P3.6/CCP1_2, P3.7/CCP2_2)
	P_SW1 = ACC;  
  
//  ACC = P_SW1;
//  ACC &= ~(CCP_S0 | CCP_S1);      //CCP_S0=0 CCP_S1=1
//  ACC |= CCP_S1;                  //(P2.4/ECI_3, P2.5/CCP0_3, P2.6/CCP1_3, P2.7/CCP2_3)
//  P_SW1 = ACC;  

    CCON = 0x00;                    //��ʼ��PCA���ƼĴ���
                                    //PCA��ʱ��ֹͣ
                                    //���CF��־
                                    //��ʼ��PCA���ƼĴ���
    CL = 0;                         //��λPCA�Ĵ���
    CH = 0;
//	CL = 0xF0;
//	CH = 0xF0;
	
	//CMOD = 0x08;                    //����PCAʱ��ԴSYSclk
    //CMOD = 0x02;                    //����PCAʱ��ԴSYSclk/2
	//CMOD = 0x0A;                    //����PCAʱ��ԴSYSclk/4
	//CMOD = 0x0C;                    //����PCAʱ��ԴSYSclk/6
	CMOD = 0x0E;                    //����PCAʱ��ԴSYSclk/8
	//CMOD = 0x00;                    //����PCAʱ��ԴSYSclk/12
                                    //��ֹPCA��ʱ������ж�
    PCA_PWM0 = 0x00;                //PCAģ��0������8λPWM
    CCAP0H = CCAP0L = 0x88;         //PWM0��ռ�ձ�Ϊ87.5% ((100H-20H)/100H)
    CCAPM0 = 0x42;                  //PCAģ��0Ϊ8λPWMģʽ

    PCA_PWM1 = 0x40;                //PCAģ��1������7λPWM
    CCAP1H = CCAP1L = 0x20;         //PWM1��ռ�ձ�Ϊ75% ((80H-20H)/80H)
    //CCAPM1 = 0x42;                  //PCAģ��1Ϊ7λPWMģʽ
	CCAPM1 = 0x40;                  //PCAģ��1Ϊ7λPWMģʽ�����Ų����PWM

    PCA_PWM2 = 0x80;                //PCAģ��2������6λPWM
    CCAP2H = CCAP2L = 0x20;         //PWM2��ռ�ձ�Ϊ50% ((40H-20H)/40H)
    //CCAPM2 = 0x42;                  //PCAģ��2Ϊ6λPWMģʽ
	CCAPM1 = 0x40;                  //PCAģ��1Ϊ7λPWMģʽ�����Ų����PWM

	S_SLEEP1 = 1;
	S_ENABLE1 = 1;
	S_DIR1 = 0;

    CR = 1;                         //PCA��ʱ����ʼ����
#endif

	S_SLEEP1 = 1;
	S_ENABLE1 = 1;
	S_DIR1 = 0;
/********************************************************************************/
	//��ʱ��T1���PWM
//	AUXR |= 0x40;                   //��ʱ��1Ϊ1Tģʽ
  AUXR &= ~0x40;                  //��ʱ��1Ϊ12Tģʽ

    //TMOD = 0x00;                    //���ö�ʱ��Ϊģʽ1(16λ�Զ���װ��)
    
    TMOD &= ~0x40;                  //C/T1=0, ���ڲ�ʱ�ӽ���ʱ�����
//  TMOD |= 0x40;                   //C/T1=1, ��T1���ŵ��ⲿʱ�ӽ���ʱ�����

    TL1 = F38_4KHz;                 //��ʼ����ʱֵ
    TH1 = F38_4KHz >> 8;
	ET1 = 1;  						//ʹ�ܶ�ʱ��1�ж�
    TR1 = 1; 						//������ʱ��1
    INT_CLKO &= ~0x02;              //���ܶ�ʱ��1��ʱ���������
	//INT_CLKO |= 0x02;
/********************************************************************************/
	S_ENABLE2 = 0;
	S_DIR2 = 0;

	pStepMotor->speedStatus = SPEED_NONE;
	pStepMotor->maxSpeed = 1 * STEPMOTOR_PULSEPERROUND; //1ת/��
	pStepMotor->minSpeed = 0.2 * STEPMOTOR_PULSEPERROUND; //0.2ת/��
	pStepMotor->curSpeed = pStepMotor->minSpeed;
	pStepMotor->desSpeed = pStepMotor->minSpeed;
	pStepMotor->pDriveStepMotor = driveStepMotor;
	pStepMotor->pStepMotorEnable = stepMotorEnable;

	pStepMotor->pStepMotorEnable(ENABLE); //ʹ�ܲ������
	pStepMotor->stepMotorControl = 0;
}			 

void stepMotorEnable(Status sta)
{
	sta ? (S_ENABLE2 = 1) : (S_ENABLE2 = 0);
}

void driveStepMotor(void)
{
	uint8_t speed = 0xF0;
	uint8_t mode = 0;

	while(1)
	{
		if(pStepMotor->stepMotorControl != 1) //����ֹͣ
		{
			S_STEP2 = 0;
			mode = 1;
		}

//		if(mode==0 || mode==1)
//		{
//			S_STEP2 = 1;
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			S_STEP2 = 0;
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//			DelayNus(speed);
//		}
//		else
		{
			S_STEP2 = 1;
			DelayNus(speed);
			S_STEP2 = 0;
			DelayNus(speed);	
		}

		if(mode == 0)
			speed--;
		if(mode == 1)
			speed++;
		if(speed<=stepMotor.curSpeed && mode==0)
			mode = 2;
		if(speed>=0xF0 && mode==1)
			break;
	}
}


//uint16_t reload = F1200Hz;
uint16_t reload = F19_2KHz;
/* Timer1 interrupt routine */
void tm1_isr() interrupt 3 using 3
{
	static uint8_t cnt = 0;
    //uint16_t reload, hz;

//	if(pStepMotor->speedStatus == SPEED_ACC)
//	{
//		if(pStepMotor->curSpeed < pStepMotor->desSpeed)
//			pStepMotor->curSpeed++;
//		else if(pStepMotor->curSpeed >= pStepMotor->desSpeed)
//			pStepMotor->speedStatus = SPEED_NONE;
//	}
//	if(pStepMotor->speedStatus == SPEED_DEC)
//	{
//		if(pStepMotor->curSpeed > pStepMotor->minSpeed)
//			pStepMotor->curSpeed--;
//		else if(pStepMotor->curSpeed <= pStepMotor->minSpeed)
//		{
//			pStepMotor->speedStatus = SPEED_NONE;
//			INT_CLKO &= ~0x02;  //ֹͣPWM���
//		}
//	}

	//hz = pStepMotor->curSpeed * STEPMOTOR_PULSEPERROUND;
	//reload = 65536 - FOSC/2/12/hz;


			
	if(reload >= F38_4KHz)
	   reload = F38_4KHz;
	else
	{
	   	cnt++;
		if(cnt >= 0x01)
		{
			cnt = 0;
			reload += 1;
		}
	}
	TH1 = reload >> 8;
	TL1 = reload;
}