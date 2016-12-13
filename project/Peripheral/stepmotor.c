#include "stepmotor.h"
#include "io.h"
#include "uart.h"
#include "sensor.h"
#include "stdlib.h"
#include "../CPrintf.h"

static StepMotor_TypeDef stepMotor;
StepMotor_TypeDef *pStepMotor = &stepMotor;

//各重要点在转盘回原点后的绝对位置
const uint8_t AbsCoordinate[10] = 
{
	20, // POS_PUMP1 = 
	21,	// POS_PUMP2 = 
	22,	// POS_PUMP3 = 
	23,	// POS_PUMP4 = 
	24,	// POS_PUMP5 = 
	25,	// POS_PUMP6 = 
	26,	// POS_PUMP7 = 
	27,	// POS_PUMP8 = 

	48, //废液口 POS_WASTE = 
	3,  //手动点 POS_HANDLE = 	
};

////定位速度
//const SpeedLevel_TypeDef speedLevel[] = {
//	{STEPMOTOR_FREQ(0.02), 0.02*SPEED_CONST},
//	{STEPMOTOR_FREQ(0.05), 0.05*SPEED_CONST},
//	{STEPMOTOR_FREQ(0.1),  0.1*SPEED_CONST},
//	{STEPMOTOR_FREQ(0.15), 0.15*SPEED_CONST},
//	{STEPMOTOR_FREQ(0.3), 0.3*SPEED_CONST},
//	{STEPMOTOR_FREQ(0.5), 0.5*SPEED_CONST},
//	{STEPMOTOR_FREQ(0.7), 0.7*SPEED_CONST},
//	{STEPMOTOR_FREQ(0.9), 0.9*SPEED_CONST},
//	{STEPMOTOR_FREQ(1.0), 1.0*SPEED_CONST},
//	{STEPMOTOR_FREQ(1.2), 1.2*SPEED_CONST},
//	{STEPMOTOR_FREQ(1.4), 1.4*SPEED_CONST},
//	{STEPMOTOR_FREQ(1.6), 1.6*SPEED_CONST},
//	{STEPMOTOR_FREQ(1.8), 1.8*SPEED_CONST},
//	{STEPMOTOR_FREQ(2.0), 2.0*SPEED_CONST},
//};

//定位速度
const SpeedLevel_TypeDef speedLevel[] = {
	{STEPMOTOR_FREQ(0.10), 0.10*SPEED_CONST},
	{STEPMOTOR_FREQ(0.15), 0.15*SPEED_CONST},
	{STEPMOTOR_FREQ(0.20), 0.20*SPEED_CONST},
	{STEPMOTOR_FREQ(0.30), 0.30*SPEED_CONST},
	{STEPMOTOR_FREQ(0.45), 0.45*SPEED_CONST},
	{STEPMOTOR_FREQ(0.60), 0.60*SPEED_CONST},
	{STEPMOTOR_FREQ(0.80), 0.80*SPEED_CONST},
	{STEPMOTOR_FREQ(1.00), 1.00*SPEED_CONST},
	{STEPMOTOR_FREQ(1.20), 1.20*SPEED_CONST},
	{STEPMOTOR_FREQ(1.40), 1.40*SPEED_CONST},
	{STEPMOTOR_FREQ(1.60), 1.60*SPEED_CONST},
	{STEPMOTOR_FREQ(1.80), 1.80*SPEED_CONST},
	{STEPMOTOR_FREQ(2.00), 2.00*SPEED_CONST},
	{STEPMOTOR_FREQ(2.20), 2.20*SPEED_CONST},
};

const uint8_t speedLevelSize = (sizeof(speedLevel)/sizeof(speedLevel[0]));

//设置步进电机方向
static void StepMotor_SetDir(Direction_TypeDef dir)
{
	//dir ? (S_DIR2 = 1; pStepMotor->direction = CCW) : (S_DIR2 = 0 ; pStepMotor->direction = CCW);	//设置方向
	if(dir == CW)
	{
		S_DIR2 = 0;
		pStepMotor->direction = CW;
	}
	else
	{
		S_DIR2 = 1;
		pStepMotor->direction = CCW;
	}
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

		TR1 = 1;  //启动定时器1
		//pStepMotor->control = 0x01;	
	}
	else
	{
		pStepMotor->speedStatus = SPEED_DEC;  //减速，减速到最小速度

		//TR1 = 0;  //停止定时器1
		//pStepMotor->control = 0x00;
	}
}

//设置步进电机停止命令
static void StepMotor_Stop(void)
{
	pStepMotor->speedStatus = SPEED_STOP;
	
	TF1 = 1; //进入定时器1中断		
}

static void StepMotor_StopAndAlign(uint8_t len)
{
	pStepMotor->SetPos(len);
	while(!pStepMotor->IsOnPos())
	{
		if(pSensor->GetStatus(SENSOR_POS))
			pStepMotor->UpdatePos();	
	} 
	pStepMotor->Stop();	  //终点位置到立即停止
	while(!pStepMotor->IsStop());//等到电机停止
}

static void StepMotor_SetPos(uint8_t pos)
{
	pStepMotor->curCount = 0;
	pStepMotor->desCount = pos;	
}

static void StepMotor_UpdatePos(void)
{
	pStepMotor->curCount++;

	if(pStepMotor->direction == CW)
	{
		pStepMotor->curPos++;
		if(pStepMotor->curPos >= TANK_COUNT)
			pStepMotor->curPos = 0;	
	}
	else
	{
		pStepMotor->curPos--;
		if(pStepMotor->curPos < 0)
			pStepMotor->curPos = TANK_COUNT-1;
	}
	
	//Uart_SendData(pStepMotor->curPos);			
}

static uint8_t StepMotor_IsOnPos(void)
{
	return (pStepMotor->curCount >= pStepMotor->desCount); 		
}

static uint8_t StepMotor_IsStop(void)
{
	return (TR1 == 0); 		
}

//转盘回原点
static void StepMotor_Home(void)
{
	cDebug("Home\n");

	pSensor->SetCheckEdge(FALLINGEDGE);

	pStepMotor->SetPos(1);
	pStepMotor->SetSpeed(SPEDD_HOME);  //第8级速度
	pStepMotor->SetDir(CCW);
	pStepMotor->SetCMD(ENABLE);

	while(!pStepMotor->IsOnPos())
	{
		if(pSensor->GetStatus(SENSOR_HOME))
			pStepMotor->UpdatePos();	
	} 
	pStepMotor->SetCMD(DISABLE);   //原点传感器检测到立即减速

	pStepMotor->SetPos(3);
	while(!pStepMotor->IsOnPos())
	{
		if(pSensor->GetStatus(SENSOR_POS))
			pStepMotor->UpdatePos();	
	} 
	pStepMotor->Stop();	  //第3个位置检测到立即停止
	while(!pStepMotor->IsStop());	

	pStepMotor->curPos = 0;  //设置当前位置为0
}

//返回转盘转动坐标系的（相对）位置
static uint8_t StepMotor_Abs2Rel(uint8_t absCoord)
{
	return ((absCoord + pStepMotor->curPos) % TANK_COUNT);	
}

//转盘转动坐标定位，dis为距离
static void StepMotor_Position(Direction_TypeDef dir, uint8_t dis)
{
	pSensor->SetCheckEdge(FALLINGEDGE);
	
	//设置步进电机方向
	if(dir == CW) 
		pStepMotor->SetDir(CW);
	else
		pStepMotor->SetDir(CCW);
	
	//
	if(dis == 1)
	{
		pStepMotor->SetSpeed(SPEED_POSITION1);  //第5级速度
		pStepMotor->SetPos(1);
		pStepMotor->SetCMD(ENABLE);	
		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		} 
		pStepMotor->Stop();   //终点到立即停止	
	}
	else if(dis == 2)
	{
		pStepMotor->SetSpeed(SPEED_POSITION2);  //第8级速度
		pStepMotor->SetPos(1);
		pStepMotor->SetCMD(ENABLE);	
		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		}  
		pStepMotor->SetCMD(DISABLE);   //前1个位置到立即减速
		
		pStepMotor->SetPos(1);
		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		} 
		pStepMotor->Stop();	  //终点位置到立即停止	
	}
	else if(dis > 2)
	{
		pStepMotor->SetSpeed(SPEDD_POSITION);  //第8级速度
		pStepMotor->SetPos(dis - 2);
		pStepMotor->SetCMD(ENABLE);	
		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		} 
		pStepMotor->SetCMD(DISABLE);   //前2个位置到立即减速
		
		pStepMotor->SetPos(2);
		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		} 
		pStepMotor->Stop();	  //终点位置到立即停止	
	}
	
	//等到电机真正停止
	while(!pStepMotor->IsStop());

	//偏移			
}

//转盘相对坐标定位，srcTank要转到desTank的位置					   
static void StepMotor_RelativePosition(uint8_t desTank, uint8_t srcTank)
{
	int8_t len;
	uint8_t dis;
	Direction_TypeDef dir;

	len = desTank - srcTank;  //反转为主
	//len = srcTank - desTank;	//正转为主

	if(len == 0)
		return;

//	if(abs(len) > TANK_COUNT/2)
//	{
//		if(len > 0) //原来是顺时针
//			dir = CW;
//		else  //原来是逆时针
//			dir = CCW;
//
//		dis = TANK_COUNT - abs(len);
//	}
//	else
//	{
//		if(len > 0)
//			dir = CCW;
//		else
//			dir = CW;
//			
//		dis = abs(len);	
//	}

	dir = CCW;
	if(len < 0)
		dis = TANK_COUNT + len;
	else
		dis = len;

	pStepMotor->Position(dir, dis);

//	for(i=0;i<dis;i++)
//	{
//		if(dir == CW)
//		{
//			pStepMotor->curPos++;
//			if(pStepMotor->curPos >= TANK_COUNT)
//				pStepMotor->curPos = 0;
//		}
//		else
//		{
//			pStepMotor->curPos--;
//			if(pStepMotor->curPos < 0)
//				pStepMotor->curPos = TANK_COUNT-1;
//		}
//	}	
}

//步进电机初始化
void StepMotor_Init(void)
{
	pStepMotor->offset = STEPMOTOR_OFFSET;
	pStepMotor->speedStatus = SPEED_NONE;
	pStepMotor->pSpeedLevel = speedLevel;
	pStepMotor->curSpeedIndex = 0;
	pStepMotor->desSpeedIndex = 0;

	pStepMotor->SetSpeed = StepMotor_SetSpeed;
	pStepMotor->SetDir = StepMotor_SetDir;
	pStepMotor->SetCMD = StepMotor_SetCMD;
	pStepMotor->Stop = StepMotor_Stop;

	pStepMotor->SetPos = StepMotor_SetPos;
	pStepMotor->UpdatePos = StepMotor_UpdatePos;
	pStepMotor->IsOnPos = StepMotor_IsOnPos;
	pStepMotor->IsStop = StepMotor_IsStop;
	pStepMotor->StopAndAlign = StepMotor_StopAndAlign;

	pStepMotor->Home = StepMotor_Home;
	pStepMotor->Abs2Rel = StepMotor_Abs2Rel;
	pStepMotor->Position = StepMotor_Position;
	pStepMotor->RelativePosition = StepMotor_RelativePosition;

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

	switch(pStepMotor->speedStatus)
	{				  
		case SPEED_ACC: //加速
		{
			cnt++;
			if(cnt >= pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speedConst)
			{
				cnt = 0;
				pStepMotor->curSpeedIndex++;
			}
	
			if(pStepMotor->curSpeedIndex >= pStepMotor->desSpeedIndex) //加速完成
			{
				pStepMotor->curSpeedIndex = pStepMotor->desSpeedIndex;
				pStepMotor->speedStatus = SPEED_NONE;
			}		
		}
		break;
		case SPEED_DEC: //减速
		{
			cnt++;
			if(cnt >= pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speedConst)
			{
				cnt = 0;
				pStepMotor->curSpeedIndex--;
			}
	
			if(pStepMotor->curSpeedIndex <= 0)  //减速到最小速度
			{
				pStepMotor->curSpeedIndex = 0;
				pStepMotor->speedStatus = SPEED_NONE;
				
				cnt = 0;
				//TR1 = 0;  //停止定时器1
			}
		}
		break;
		case SPEED_STOP: 
		{
//			pStepMotor->curSpeedIndex = 0;
//			pStepMotor->speedStatus = SPEED_POSOFFSET;
//	
//			cnt = 0;
//			//TR1 = 0;  //停止定时器1

			cnt++;
			if(cnt >= pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speedConst)
			{
				cnt = 0;
				pStepMotor->curSpeedIndex--;
			}
	
			if(pStepMotor->curSpeedIndex <= 0)  //减速到最小速度
			{
				pStepMotor->curSpeedIndex = 0;			
				cnt = 0;
			}
			pStepMotor->speedStatus = SPEED_POSOFFSET;
		}
		break;
		case SPEED_POSOFFSET:
		{
			static uint16_t offset = 0;

			offset++;
			if(offset++ >= pStepMotor->offset)
			{
				pStepMotor->curSpeedIndex = 0;
				pStepMotor->speedStatus = SPEED_NONE;
				TR1 = 0;  //停止定时器1
				offset = 0;
				cnt = 0;
				break;	
			}

			cnt++;
			if(cnt >= pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speedConst)
			{
				cnt = 0;
				pStepMotor->curSpeedIndex--;
			}
	
			if(pStepMotor->curSpeedIndex <= 0)  //减速到最小速度
			{
				pStepMotor->curSpeedIndex = 0;
				cnt = 0;
			}
		}
		break;
		case SPEED_NONE:
		break;
		default:
		break;
	}

	//更新定时器
	TH1 = pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speed >> 8;
	TL1 = pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speed;
}

//void StepMotor_Test(void)
//{
//	if(pStepMotor->control == 0x01) //启动定时器
//	{
//		pStepMotor->SetPos(1);
//		pSensor->SetCheckEdge(FALLINGEDGE);
//
//		//pStepMotor->SetTimer(ENABLE); //启动定时器1
//
//		while(!pStepMotor->IsOnPos())
//		{
//			if(pSensor->GetStatus(SENSOR_POS))
//				pStepMotor->UpdatePos();	
//		}
//		//pStepMotor->SetCMD(DISABLE);
//		pStepMotor->Stop();
//	}
//}