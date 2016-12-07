#include "managerment.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "../CPrintf.h"
#include "../Peripheral/stepmotor.h"
#include "../Peripheral/io.h"

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
	
	pm->backflowPumpSel = 0xF0;
	pm->purgePumpSel = 0xC3;
	pm->pumpSelPumpSel = 0x0F;

	pm->caliPumpSel = 1;
	pm->pCaliPumpPara = caliPumpPara;
	pm->caliAmount = pm->pCaliPumpPara[pm->caliPumpSel];

	pm->tipsButton = TIPS_NONE;
	pm->runningType = RUNNING_NONE;
	pm->exception = EXCEPTION_NONE;
	pm->exceptionButtonFlag = EXCEPTION_NONE;
	pm->rotateFlag = 0;
	pm->jumpTo = 0;
//	pm->proStatus = PROJECTSTATUS_IDLE;
//	pm->preProStatus = PROJECTSTATUS_IDLE;
//	pm->tipsSource = TIPSSOURCE_NONE;
}

/************************************************************************/
/* 串口接收缓冲区                                                       */
/************************************************************************/
uint8 cmd_buffer[CMD_MAX_SIZE];

void initUI(void)
{
	//初始化项目结构体
	char str[NAME_SIZE];
	uint16 i;
	uint32 j;
	
	for(i=0;i<PROJECT_COUNT;i++)
	{
		memset(str, '\0', NAME_SIZE);
		sprintf(str, "project%d", i);
		initProjectStruct(&project[i], str);
	}

	//初始化校准参数
	initCaliPumpPara(4.0);

	//初始化项目管理结构体
	initProjectMan(pProjectMan);

	/*清空串口接收缓冲区*/
	queue_reset();

	//延时一段时间
	for(j=0;j<65536;j++);

	//发送握手命令
	SetHandShake();//发送握手命令
}

//void uartInterrupt(uint8 data)
//{
//	queue_push(data);
//}

void loopForever(void)
{
	/************************************************************************/
	/*处理流程                                                              */
	/************************************************************************/
	//项目运行
	switch(pProjectMan->runningType)
	{
	case RUNNING_NONE:
		break;
	case RUNNING_PROJECT: //项目运行中
	{
		projectProgram();
		pProjectMan->runningType = RUNNING_NONE;
	}
	break;
	case RUNNING_BACKFLOW:
		break;
	case RUNNING_PURGE:	 //执行清洗程序
		purgeProgram();
		pProjectMan->runningType = RUNNING_NONE;  //程序执行完成要清除标志	
		break;
	case RUNNING_CALIBRATION:
		break;
	case RUNNING_HOME:
		//回原点
		pStepMotor->Home();
		SetScreen(MAINPAGE_INDEX);
		pProjectMan->runningType = RUNNING_NONE;
		beepAlarm(1);
		break;
	default:
		break;
	}
}

#ifdef __cplusplus
}
#endif
