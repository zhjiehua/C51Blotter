#include "sensor.h"
#include "common.h"
#include "stepmotor.h"
#include "dcmotor.h"
#include "uart.h"

//������״̬
static Sensor_TypeDef sensor;
Sensor_TypeDef *pSensor = &sensor;

static void Sensor_Scan(void)
{
	pSensor->preStatus = pSensor->curStatus;

	EAIN1 ? (pSensor->curStatus |= SENSOR1_MASK) : (pSensor->curStatus &= ~SENSOR1_MASK);
	EAIN2 ? (pSensor->curStatus |= SENSOR2_MASK) : (pSensor->curStatus &= ~SENSOR2_MASK);
	EAIN3 ? (pSensor->curStatus |= SENSOR3_MASK) : (pSensor->curStatus &= ~SENSOR3_MASK);
	EAIN4 ? (pSensor->curStatus |= SENSOR4_MASK) : (pSensor->curStatus &= ~SENSOR4_MASK);
	EAIN5 ? (pSensor->curStatus |= SENSOR5_MASK) : (pSensor->curStatus &= ~SENSOR5_MASK);
	EAIN6 ? (pSensor->curStatus |= SENSOR6_MASK) : (pSensor->curStatus &= ~SENSOR6_MASK);
	EAIN7 ? (pSensor->curStatus |= SENSOR7_MASK) : (pSensor->curStatus &= ~SENSOR7_MASK);
	EAIN8 ? (pSensor->curStatus |= SENSOR8_MASK) : (pSensor->curStatus &= ~SENSOR8_MASK);
	
	pSensor->fallingEdge = (pSensor->curStatus ^ pSensor->preStatus) & pSensor->preStatus;
	pSensor->rasingEdge = (pSensor->curStatus ^ pSensor->preStatus) & pSensor->curStatus;
}

static void Sensor_SetPos(uint8_t pos)
{
	pSensor->curCount = 0;
	pSensor->desCount = pos;	
}

static void Sensor_SetCheckEdge(CheckEdge_TypeDef edge)
{
	pSensor->checkEdge = edge;	
}

static uint8_t Sensor_IsOnPos(SensorEnum_TypeDef sensor)
{
	uint8_t edge, mask;
	pSensor->Scan();  //ɨ��

	if(pSensor->checkEdge == FALLINGEDGE) //����½���
		edge = pSensor->fallingEdge;
	else  //���������
		edge = pSensor->rasingEdge;
			
	if(edge & (0x01<<sensor))
	{
		if(++pSensor->curCount >= pSensor->desCount)
		{
			pSensor->curCount = 0;
			pSensor->desCount = 0;
			return 1;
		}
	}
	return 0;	 		
}

static uint8_t Sensor_GetStatus(SensorEnum_TypeDef sensor)
{
	uint8_t edge, ret;
	pSensor->Scan();  //ɨ��

	if(pSensor->checkEdge == FALLINGEDGE) //����½���
		edge = pSensor->fallingEdge;
	else  //���������
		edge = pSensor->rasingEdge;
			
	if(edge & (0x01<<sensor))
		ret = 1;
	else
		ret = 0;
	return ret;	 		
}

void Sensor_Init(void)
{
	INT0_INPUT = 0;
	//ʹ���ⲿ�ж�
//	INT0_INPUT = 1;
//	IT0 = 1;                    //����INT0���ж����� (1:���½��� 0:�����غ��½���)
//	EX0 = 1;                    //ʹ��INT0���


	pSensor->preStatus = 0x00;
	pSensor->curStatus = 0x00;
	pSensor->fallingEdge = 0x00;
	pSensor->curCount = 0;
	pSensor->desCount = 0;
	pSensor->checkEdge = FALLINGEDGE;

	pSensor->Scan = Sensor_Scan;
	pSensor->SetCheckEdge = Sensor_SetCheckEdge;
	//pSensor->SetPos = Sensor_SetPos;
	//pSensor->IsOnPos = Sensor_IsOnPos;
	pSensor->GetStatus = Sensor_GetStatus;
}

//-----------------------------------------
//�жϷ������
void exint0() interrupt 0 using 3       //INT0�ж����
{
	Uart_SendData(0x05);
	sensorCheck();	
}
