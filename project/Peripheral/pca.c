#include "pca.h"
#include "io.h"

static StepMotor_TypeDef stepMotor;
StepMotor_TypeDef *pStepMotor = &stepMotor;

void Delay100us(uint16_t num)		//@20.000MHz
{
	unsigned char i, j;

	i = num;
	j = 480;
	do
	{
		while (--j);
	} while (--i);
}

//PCA��ʼ��
void PCA_Init(void)
{
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

	S_ENABLE2 = 0;
	S_DIR2 = 0;

	pStepMotor->curSpeed = 1;
	pStepMotor->pDriveStepMotor = driveStepMotor;
	pStepMotor->pStepMotorEnable = stepMotorEnable;
}			 

void stepMotorEnable(Status sta)
{
	sta ? (S_ENABLE2 = 1) : (S_ENABLE2 = 0);
}

void driveStepMotor(void)
{
	while(1)
	{
		if(S_ENABLE2 != 1) //����ֹͣ
		{
			S_STEP2 = 0;
			break;
		}

		S_STEP2 = 1;
		Delay100us(stepMotor.curSpeed);
		S_STEP2 = 0;
		Delay100us(stepMotor.curSpeed);
	}
}