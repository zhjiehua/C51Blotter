#include "managerment.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "../CPrintf.h"
#include "../Peripheral/stepmotor.h"
#include "../Peripheral/io.h"
#include "../Peripheral/24cxx.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* 定义项目管理结构体                                                   */
/************************************************************************/
ProjectMan_TypeDef ProjectMan;
ProjectMan_TypeDef *pProjectMan = &ProjectMan;

void initProjectMan(ProjectMan_TypeDef *pm)
{
	pm->pCurRunningProject = NULL;
	pm->pCurRunningAction = NULL;
	pm->curTank = 1;
	pm->RTCTimeout = 0;
	pm->curLoopTime = 1;

	pm->pCurProject = &project[0];
	pm->startTank = 1;
	pm->endTank = 1;

	pm->pCurEditProject = &project[0];
	pm->pCurEditAction = &(pm->pCurEditProject->action[0]);

	pm->pCurJumptoAction = pm->pCurRunningAction;
	
	pm->backflowPumpSel = 0x00;
	pm->purgePumpSel = 0x00;
	pm->pumpSelPumpSel = 0x00;

	pm->caliPumpSel = 1;
	pm->pCaliPumpPara = caliPumpPara;
	pm->caliAmount = pm->pCaliPumpPara[pm->caliPumpSel];

	pm->tipsButton = TIPS_NONE;
	pm->runningType = RUNNING_NONE;
	pm->exception = EXCEPTION_NONE;
	pm->exceptionButtonFlag = EXCEPTION_NONE;
	pm->rotateFlag = 0;
	pm->jumpTo = 0;
}

/************************************************************************/
/* 串口接收缓冲区                                                       */
/************************************************************************/
uint8 cmd_buffer[CMD_MAX_SIZE];

#define EEPROM_DEFAULT 0x11223346

void initUI(void)
{
	//初始化项目结构体
	char str[NAME_SIZE];
	int16 i;
	uint32 j;
	uint32_t dat;

	AT24CXX_Read(RESET_DEFAULT, (uint8_t*)(&dat), sizeof(uint32_t));//是否第一次开机
	if(dat != EEPROM_DEFAULT) //是，初始化EEPROM中的数据
	{
		cDebug("RESET_DEFAULT\n");

		//初始化校准参数
		initCaliPumpPara(4.0);
		AT24CXX_Write(CALIBPARA_BASEADDR, (uint8_t*)caliPumpPara, CALIBPARA_SIZE);

		//初始化项目参数
		for(i=PROJECT_COUNT-1;i>=0;i--)
		{
			memset(str, '\0', NAME_SIZE);
			sprintf(str, "project%d", i);
			initProjectStruct(&project[0], i, str);

			cDebug("project[0].name = %s\n", project[0].name);
			cDebug("project[0].index = %d\n", (uint16_t)project[0].index);

			AT24CXX_Write(PROJECT_BASEADDR+i*PROJECT_SIZE, (uint8_t*)(&project[0]), PROJECT_SIZE);	
		}
		
		dat = EEPROM_DEFAULT;
		AT24CXX_Write(RESET_DEFAULT, (uint8_t*)&dat, sizeof(uint32_t));	
	}
	else //否，从EEPROM中读取数据
	{
		cDebug("read data from EEPROM\n");
		AT24CXX_Read(CALIBPARA_BASEADDR, (uint8_t*)caliPumpPara, CALIBPARA_SIZE); //读出校准参数
//		for(i=0;i<PUMP_COUNT;i++)
//			cDebug("caliPumpPara[%d] = %f\n", i, caliPumpPara[i]);
		AT24CXX_Read(PROJECT_BASEADDR + PROJECT_SIZE, (uint8_t*)&project[0], PROJECT_SIZE);  //读出第一个项目参数
		//cDebug("PROJECT_SIZE = %d\n", PROJECT_SIZE);
		cDebug("project[0].name = %s\n", project[0].name);
		cDebug("project[0].index = %d\n", (uint16_t)project[0].index);	
	}

	//初始化项目管理结构体
	initProjectMan(pProjectMan);

	/*清空串口接收缓冲区*/
	queue_reset();

	//延时一段时间
	for(j=0;j<65536;j++);

	cDebug("initUI success\n");

	//发送握手命令
	SetHandShake();//发送握手命令
	SetHandShake();//发送握手命令，第一个握手命令会丢失
	//SetHandShake();//发送握手命令
}

//void uartInterrupt(uint8 data)
//{
//	queue_push(data);
//}

//void loopForever(void)
//{
//	/************************************************************************/
//	/*处理流程                                                              */
//	/************************************************************************/
//	//项目运行
//	switch(pProjectMan->runningType)
//	{
//	case RUNNING_NONE:
//		break;
//	case RUNNING_PROJECT: //项目运行中
//	{
//		projectProgram();
//		pProjectMan->runningType = RUNNING_NONE;
//	}
//	break;
//	case RUNNING_BACKFLOW:
//		break;
//	case RUNNING_PURGE:	 //执行清洗程序
//		purgeProgram();
//		pProjectMan->runningType = RUNNING_NONE;  //程序执行完成要清除标志	
//		break;
//	case RUNNING_CALIBRATION:
//		break;
//	case RUNNING_HOME:
//		//回原点
//		pStepMotor->Home();
//		SetScreen(MAINPAGE_INDEX);
//		pProjectMan->runningType = RUNNING_NONE;
//		beepAlarm(1);
//		break;
//	default:
//		break;
//	}
//}

#ifdef __cplusplus
}
#endif
