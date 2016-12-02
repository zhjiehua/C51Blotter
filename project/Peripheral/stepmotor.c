#include "stepmotor.h"
#include "io.h"
#include "uart.h"
#include "sensor.h"

static StepMotor_TypeDef stepMotor;
StepMotor_TypeDef *pStepMotor = &stepMotor;

////快速
//const SpeedLevel_TypeDef speedLevel[] = {
//	{F0R1, 5*SPEED_CONST},
//	{F0R2, 5*SPEED_CONST},
//	{F0R3, 7*SPEED_CONST},
//	{F0R5, 7*SPEED_CONST},
//	{F0R7, 10*SPEED_CONST},
//	{F1R,  15*SPEED_CONST},
//	{F1R3, 20*SPEED_CONST},
//	{F1R6, 25*SPEED_CONST},
//	{F2R,  30*SPEED_CONST},
//	{F2R5, 40*SPEED_CONST},
//	{F3R,  50*SPEED_CONST},
//	{F3R5, 60*SPEED_CONST},
//	{F4R,  70*SPEED_CONST},
//};

//定位速度
const SpeedLevel_TypeDef speedLevel[] = {
	{STEPMOTOR_FREQ(0.02), 0.02*SPEED_CONST},
	{STEPMOTOR_FREQ(0.05), 0.05*SPEED_CONST},
	{STEPMOTOR_FREQ(0.1),  0.1*SPEED_CONST},
	{STEPMOTOR_FREQ(0.15), 0.15*SPEED_CONST},
	{STEPMOTOR_FREQ(0.3), 0.3*SPEED_CONST},
	{STEPMOTOR_FREQ(0.5), 0.4*SPEED_CONST},
	{STEPMOTOR_FREQ(0.7), 0.7*SPEED_CONST},
	{STEPMOTOR_FREQ(0.9), 0.9*SPEED_CONST},
	{STEPMOTOR_FREQ(1.0), 1.0*SPEED_CONST},
};

const uint8_t speedLevelSize = (sizeof(speedLevel)/sizeof(speedLevel[0]));

//设置步进电机方向
static void StepMotor_SetDir(Direction_TypeDef dir)
{
	dir ? (S_DIR2 = 1) : (S_DIR2 = 0);	//设置方向
}

//设置步进电机速度
static void StepMotor_SetSpeed(uint8_t speedIndex)
{
	pStepMotor->desSpeedIndex = speedIndex;	 //设置目标速度
}

//设置步进电机命令
static void StepMotor_SetCMD(Status sta)
{
	if(sta == ENABLE)
	{
		pStepMotor->speedStatus = SPEED_ACC;  //加速
		pStepMotor->curSpeedIndex = 0;  //起始速度为最小速度

		//TR1 = 1;  //启动定时器1
		pStepMotor->control = 0x01;	
	}
	else
	{
		pStepMotor->speedStatus = SPEED_DEC;  //减速，减速到最小速度后会关掉定时器

		//TR1 = 0;  //停止定时器1
		pStepMotor->control = 0x00;
	}
}

//设置步进电机命令
static void StepMotor_Stop(void)
{
	pStepMotor->speedStatus = SPEED_STOP;
	
	pStepMotor->control = 0x00;
	TF1 = 1; //进入定时器1中断		
}

static void StepMotor_SetTimer(Status sta)
{
	sta ? (TR1 = 1) : (TR1 = 0);	
}

//步进电机初始化
void StepMotor_Init(void)
{
	pStepMotor->speedStatus = SPEED_NONE;
	pStepMotor->pSpeedLevel = speedLevel;
	pStepMotor->curSpeedIndex = 0;
	pStepMotor->desSpeedIndex = 0;

	pStepMotor->SetSpeed = StepMotor_SetSpeed;
	pStepMotor->SetDir = StepMotor_SetDir;
	pStepMotor->SetCMD = StepMotor_SetCMD;
	pStepMotor->Stop = StepMotor_Stop;
	pStepMotor->SetTimer = StepMotor_SetTimer;

	pStepMotor->control = 0x00;


	S_SLEEP1 = 1;
	S_ENABLE1 = 1;
	//S_DIR1 = 1;
/********************************************************************************/
	//定时器T1输出PWM
//	AUXR |= 0x40;                   //定时器1为1T模式
	AUXR &= ~0x40;                  //定时器1为12T模式

    //TMOD = 0x00;                    //设置定时器为模式1(16位自动重装载)
    
    TMOD &= ~0x40;                  //C/T1=0, 对内部时钟进行时钟输出
//  TMOD |= 0x40;                   //C/T1=1, 对T1引脚的外部时钟进行时钟输出

    TL1 = pStepMotor->pSpeedLevel[0].speed;                 //初始化计时值
    TH1 = pStepMotor->pSpeedLevel[0].speed >> 8;
	ET1 = 1;  						//使能定时器1中断
    TR1 = 0; 						//停止定时器1
    //INT_CLKO &= ~0x02;              //除能定时器1的时钟输出功能
	INT_CLKO |= 0x02;
/********************************************************************************/
	S_ENABLE2 = 1; //使能电机控制
	S_DIR2 = 0;
}

/* Timer1 interrupt routine */
void tm1_isr() interrupt 3 using 3
{
	static uint16_t cnt = 0;

	if(pStepMotor->speedStatus == SPEED_ACC) //加速
	{
		cnt++;
		if(cnt >= pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speedConst)
		{
			cnt = 0;
			pStepMotor->curSpeedIndex++;

			//Uart_SendData(0x01);
		}

		if(pStepMotor->curSpeedIndex >= pStepMotor->desSpeedIndex) //加速完成
		{
			pStepMotor->curSpeedIndex = pStepMotor->desSpeedIndex;
			pStepMotor->speedStatus = SPEED_NONE;
		}		
	}
	else if(pStepMotor->speedStatus == SPEED_DEC)//减速
	{
		cnt++;
		if(cnt >= pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speedConst)
		{
			cnt = 0;
			pStepMotor->curSpeedIndex--;

			//Uart_SendData(0x02);
		}

		if(pStepMotor->curSpeedIndex <= 0)  //减速完后停止
		{
			pStepMotor->curSpeedIndex = 0;
			pStepMotor->speedStatus = SPEED_NONE;
			
			cnt = 0;
			TR1 = 0;  //停止定时器1
		}
	}
	else if(pStepMotor->speedStatus == SPEED_STOP)
	{
		pStepMotor->curSpeedIndex = 0;
		pStepMotor->speedStatus = SPEED_NONE;

		cnt = 0;
		TR1 = 0;  //停止定时器1
		//TF1 = 0;

		Uart_SendData(0x03);
	}

	//更新定时器
	TH1 = pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speed >> 8;
	TL1 = pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speed;
}

void StepMotor_Test(void)
{
	if(pStepMotor->control == 0x01) //启动定时器
	{
		pSensor->SetPos(1);
		pSensor->SetCheckEdge(FALLINGEDGE);

		pStepMotor->SetTimer(ENABLE); //启动定时器1

		while(!pSensor->IsOnPos(SENSOR1));
		//pStepMotor->SetCMD(DISABLE);
		pStepMotor->Stop();
	}
}