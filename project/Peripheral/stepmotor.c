#include "stepmotor.h"
#include "io.h"
#include "uart.h"
#include "sensor.h"
#include "stdlib.h"

static StepMotor_TypeDef stepMotor;
StepMotor_TypeDef *pStepMotor = &stepMotor;

//const uint8_t AbsCoordinate[10] = 
//{
//	31,  // POS_PUMP1 = 
//	30,	// POS_PUMP2 = 
//	29,	// POS_PUMP3 = 
//	28,	// POS_PUMP4 = 
//	27,	// POS_PUMP5 = 
//	26,	// POS_PUMP6 = 
//	25,	// POS_PUMP7 = 
//	24,	// POS_PUMP8 = 
//
//	3, //��Һ�� POS_WASTE = 
//	8, //�ֶ��� POS_HANDLE = 	
//};

const uint8_t AbsCoordinate[10] = 
{
	20,  // POS_PUMP1 = 
	21,	// POS_PUMP2 = 
	22,	// POS_PUMP3 = 
	23,	// POS_PUMP4 = 
	24,	// POS_PUMP5 = 
	25,	// POS_PUMP6 = 
	26,	// POS_PUMP7 = 
	27,	// POS_PUMP8 = 

	48, //��Һ�� POS_WASTE = 
	3, //�ֶ��� POS_HANDLE = 	
};

////����
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

//��λ�ٶ�
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

//���ò����������
static void StepMotor_SetDir(Direction_TypeDef dir)
{
	//dir ? (S_DIR2 = 1; pStepMotor->direction = CCW) : (S_DIR2 = 0 ; pStepMotor->direction = CCW);	//���÷���
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

//���ò�������ٶ�
static void StepMotor_SetSpeed(uint8_t speedIndex)
{
	pStepMotor->desSpeedIndex = speedIndex;	 //����Ŀ���ٶ�
}

//���ò����������
static void StepMotor_SetCMD(Status sta)
{
	if(sta == ENABLE)
	{
		pStepMotor->speedStatus = SPEED_ACC;  //����
		pStepMotor->curSpeedIndex = 0;  //��ʼ�ٶ�Ϊ��С�ٶ�

		TR1 = 1;  //������ʱ��1
		//pStepMotor->control = 0x01;	
	}
	else
	{
		pStepMotor->speedStatus = SPEED_DEC;  //���٣����ٵ���С�ٶ�

		//TR1 = 0;  //ֹͣ��ʱ��1
		//pStepMotor->control = 0x00;
	}
}

//���ò������ֹͣ����
static void StepMotor_Stop(void)
{
	pStepMotor->speedStatus = SPEED_STOP;
	
	TF1 = 1; //���붨ʱ��1�ж�		
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

//ת�̻�ԭ��
static void StepMotor_Home(void)
{
	pSensor->SetCheckEdge(FALLINGEDGE);

	pStepMotor->SetPos(1);
	pStepMotor->SetSpeed(8);  //��8���ٶ�
	pStepMotor->SetDir(CCW);
	pStepMotor->SetCMD(ENABLE);

	while(!pStepMotor->IsOnPos())
	{
		if(pSensor->GetStatus(SENSOR_HOME))
			pStepMotor->UpdatePos();	
	} 
	pStepMotor->SetCMD(DISABLE);   //ԭ�㴫������⵽��������

	pStepMotor->SetPos(3);
	while(!pStepMotor->IsOnPos())
	{
		if(pSensor->GetStatus(SENSOR_POS))
			pStepMotor->UpdatePos();	
	} 
	pStepMotor->Stop();	  //��3��λ�ü�⵽����ֹͣ
	while(!pStepMotor->IsStop());	

	pStepMotor->curPos = 0;  //���õ�ǰλ��Ϊ0
}

//����ת��ת������ϵ�ģ���ԣ�λ��
static uint8_t StepMotor_Abs2Rel(uint8_t absCoord)
{
	return ((absCoord + pStepMotor->curPos) % TANK_COUNT);	
}

//ת��ת�����궨λ��disΪ����
static void StepMotor_Position(Direction_TypeDef dir, uint8_t dis)
{
	pSensor->SetCheckEdge(FALLINGEDGE);
	
	//���ò����������
	if(dir == CW) 
		pStepMotor->SetDir(CW);
	else
		pStepMotor->SetDir(CCW);
	
	//
	if(dis == 1)
	{
		pStepMotor->SetSpeed(2);  //��5���ٶ�
		pStepMotor->SetPos(1);
		pStepMotor->SetCMD(ENABLE);	
		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		} 
		pStepMotor->Stop();   //�յ㵽����ֹͣ	
	}
	else if(dis == 2)
	{
		pStepMotor->SetSpeed(5);  //��8���ٶ�
		pStepMotor->SetPos(1);
		pStepMotor->SetCMD(ENABLE);	
		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		}  
		pStepMotor->SetCMD(DISABLE);   //ǰ1��λ�õ���������
		
		pStepMotor->SetPos(1);
		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		} 
		pStepMotor->Stop();	  //�յ�λ�õ�����ֹͣ	
	}
	else if(dis > 2)
	{
		pStepMotor->SetSpeed(8);  //��8���ٶ�
		pStepMotor->SetPos(dis - 2);
		pStepMotor->SetCMD(ENABLE);	
		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		} 
		pStepMotor->SetCMD(DISABLE);   //ǰ2��λ�õ���������
		
		pStepMotor->SetPos(2);
		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		} 
		pStepMotor->Stop();	  //�յ�λ�õ�����ֹͣ	
	}
	
	//�ȵ��������ֹͣ
	while(!pStepMotor->IsStop());

	//ƫ��			
}

//ת��������궨λ��srcTankҪת��desTank��λ��					   
static void StepMotor_RelativePosition(uint8_t desTank, uint8_t srcTank)
{
	int8_t len;
	uint8_t dis;
	Direction_TypeDef dir;

	len = desTank - srcTank;  //��תΪ��
	//len = srcTank - desTank;	//��תΪ��

	if(len == 0)
		return;

//	if(abs(len) > TANK_COUNT/2)
//	{
//		if(len > 0) //ԭ����˳ʱ��
//			dir = CW;
//		else  //ԭ������ʱ��
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

//���������ʼ��
void StepMotor_Init(void)
{
	pStepMotor->offset = 450;
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

	pStepMotor->Home = StepMotor_Home;
	pStepMotor->Abs2Rel = StepMotor_Abs2Rel;
	pStepMotor->Position = StepMotor_Position;
	pStepMotor->RelativePosition = StepMotor_RelativePosition;

	pStepMotor->control = 0x00;

	S_SLEEP1 = 1;
	S_ENABLE1 = 1;
	//S_DIR1 = 1;
/********************************************************************************/
	//��ʱ��T1���PWM
//	AUXR |= 0x40;                   //��ʱ��1Ϊ1Tģʽ
	AUXR &= ~0x40;                  //��ʱ��1Ϊ12Tģʽ

    //TMOD = 0x00;                    //���ö�ʱ��Ϊģʽ1(16λ�Զ���װ��)
    
    TMOD &= ~0x40;                  //C/T1=0, ���ڲ�ʱ�ӽ���ʱ�����
//  TMOD |= 0x40;                   //C/T1=1, ��T1���ŵ��ⲿʱ�ӽ���ʱ�����

    TL1 = pStepMotor->pSpeedLevel[0].speed;                 //��ʼ����ʱֵ
    TH1 = pStepMotor->pSpeedLevel[0].speed >> 8;
	ET1 = 1;  						//ʹ�ܶ�ʱ��1�ж�
    TR1 = 0; 						//ֹͣ��ʱ��1
    //INT_CLKO &= ~0x02;              //���ܶ�ʱ��1��ʱ���������
	INT_CLKO |= 0x02;
/********************************************************************************/
	S_ENABLE2 = 1; //ʹ�ܵ������
	S_DIR2 = 0;
}

/* Timer1 interrupt routine */
void tm1_isr() interrupt 3 using 3
{
	static uint16_t cnt = 0;

	switch(pStepMotor->speedStatus)
	{				  
		case SPEED_ACC: //����
		{
			cnt++;
			if(cnt >= pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speedConst)
			{
				cnt = 0;
				pStepMotor->curSpeedIndex++;
	
				//Uart_SendData(0x01);
			}
	
			if(pStepMotor->curSpeedIndex >= pStepMotor->desSpeedIndex) //�������
			{
				pStepMotor->curSpeedIndex = pStepMotor->desSpeedIndex;
				pStepMotor->speedStatus = SPEED_NONE;
			}		
		}
		break;
		case SPEED_DEC: //����
		{
			cnt++;
			if(cnt >= pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speedConst)
			{
				cnt = 0;
				pStepMotor->curSpeedIndex--;
	
				//Uart_SendData(0x02);
			}
	
			if(pStepMotor->curSpeedIndex <= 0)  //���ٵ���С�ٶ�
			{
				pStepMotor->curSpeedIndex = 0;
				pStepMotor->speedStatus = SPEED_NONE;
				
				cnt = 0;
				//TR1 = 0;  //ֹͣ��ʱ��1
			}
		}
		break;
		case SPEED_STOP: 
		{
			pStepMotor->curSpeedIndex = 0;
			pStepMotor->speedStatus = SPEED_POSOFFSET;
	
			cnt = 0;
			//TR1 = 0;  //ֹͣ��ʱ��1
	
			//TF1 = 0;
			//Uart_SendData(0x03);
		}
		break;
		case SPEED_POSOFFSET:
		{
			if(cnt++ >= pStepMotor->offset)
			{
				pStepMotor->speedStatus = SPEED_NONE;
				TR1 = 0;  //ֹͣ��ʱ��1
				cnt = 0;	
			}
		}
		break;
		case SPEED_NONE:
		break;
		default:
		break;
	}

	//���¶�ʱ��
	TH1 = pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speed >> 8;
	TL1 = pStepMotor->pSpeedLevel[pStepMotor->curSpeedIndex].speed;
}

void StepMotor_Test(void)
{
	if(pStepMotor->control == 0x01) //������ʱ��
	{
		pStepMotor->SetPos(1);
		pSensor->SetCheckEdge(FALLINGEDGE);

		//pStepMotor->SetTimer(ENABLE); //������ʱ��1

		while(!pStepMotor->IsOnPos())
		{
			if(pSensor->GetStatus(SENSOR_POS))
				pStepMotor->UpdatePos();	
		}
		//pStepMotor->SetCMD(DISABLE);
		pStepMotor->Stop();
	}
}