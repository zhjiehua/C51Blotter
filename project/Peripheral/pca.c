#include "pca.h"
#include "io.h"

#define F1200Hz (65536-FOSC/2/1200)
#define F19_2KHz (65536-FOSC/2/19200)
#define F38_4KHz (65536-FOSC/2/38400)    //1T模式
//#define F38_4KHz (65536-FOSC/2/12/38400) //12T模式

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

//PCA初始化
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

    CCON = 0x00;                    //初始化PCA控制寄存器
                                    //PCA定时器停止
                                    //清除CF标志
                                    //初始化PCA控制寄存器
    CL = 0;                         //复位PCA寄存器
    CH = 0;
//	CL = 0xF0;
//	CH = 0xF0;
	
	//CMOD = 0x08;                    //设置PCA时钟源SYSclk
    //CMOD = 0x02;                    //设置PCA时钟源SYSclk/2
	//CMOD = 0x0A;                    //设置PCA时钟源SYSclk/4
	//CMOD = 0x0C;                    //设置PCA时钟源SYSclk/6
	CMOD = 0x0E;                    //设置PCA时钟源SYSclk/8
	//CMOD = 0x00;                    //设置PCA时钟源SYSclk/12
                                    //禁止PCA定时器溢出中断
    PCA_PWM0 = 0x00;                //PCA模块0工作于8位PWM
    CCAP0H = CCAP0L = 0x88;         //PWM0的占空比为87.5% ((100H-20H)/100H)
    CCAPM0 = 0x42;                  //PCA模块0为8位PWM模式

    PCA_PWM1 = 0x40;                //PCA模块1工作于7位PWM
    CCAP1H = CCAP1L = 0x20;         //PWM1的占空比为75% ((80H-20H)/80H)
    //CCAPM1 = 0x42;                  //PCA模块1为7位PWM模式
	CCAPM1 = 0x40;                  //PCA模块1为7位PWM模式，引脚不输出PWM

    PCA_PWM2 = 0x80;                //PCA模块2工作于6位PWM
    CCAP2H = CCAP2L = 0x20;         //PWM2的占空比为50% ((40H-20H)/40H)
    //CCAPM2 = 0x42;                  //PCA模块2为6位PWM模式
	CCAPM1 = 0x40;                  //PCA模块1为7位PWM模式，引脚不输出PWM

	S_SLEEP1 = 1;
	S_ENABLE1 = 1;
	S_DIR1 = 0;

    CR = 1;                         //PCA定时器开始工作
#endif

	S_SLEEP1 = 1;
	S_ENABLE1 = 1;
	S_DIR1 = 0;
/********************************************************************************/
	//定时器T1输出PWM
//	AUXR |= 0x40;                   //定时器1为1T模式
  AUXR &= ~0x40;                  //定时器1为12T模式

    //TMOD = 0x00;                    //设置定时器为模式1(16位自动重装载)
    
    TMOD &= ~0x40;                  //C/T1=0, 对内部时钟进行时钟输出
//  TMOD |= 0x40;                   //C/T1=1, 对T1引脚的外部时钟进行时钟输出

    TL1 = F38_4KHz;                 //初始化计时值
    TH1 = F38_4KHz >> 8;
	ET1 = 1;  						//使能定时器1中断
    TR1 = 1; 						//启动定时器1
    INT_CLKO &= ~0x02;              //除能定时器1的时钟输出功能
	//INT_CLKO |= 0x02;
/********************************************************************************/
	S_ENABLE2 = 0;
	S_DIR2 = 0;

	pStepMotor->speedStatus = SPEED_NONE;
	pStepMotor->maxSpeed = 1 * STEPMOTOR_PULSEPERROUND; //1转/秒
	pStepMotor->minSpeed = 0.2 * STEPMOTOR_PULSEPERROUND; //0.2转/秒
	pStepMotor->curSpeed = pStepMotor->minSpeed;
	pStepMotor->desSpeed = pStepMotor->minSpeed;
	pStepMotor->pDriveStepMotor = driveStepMotor;
	pStepMotor->pStepMotorEnable = stepMotorEnable;

	pStepMotor->pStepMotorEnable(ENABLE); //使能步进电机
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
		if(pStepMotor->stepMotorControl != 1) //主动停止
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
//			INT_CLKO &= ~0x02;  //停止PWM输出
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