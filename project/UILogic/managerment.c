#include "managerment.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "../CPrintf.h"
#include "../Peripheral/stepmotor.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* 定义项目管理结构体                                                   */
/************************************************************************/
ProjectMan_TypeDef ProjectMan;
ProjectMan_TypeDef *pProjectMan = &ProjectMan;


void tipsPageProcess(void)
{
	//提示页面分类按钮处理
	switch(pProjectMan->tipsSource)
	{
	//tips1页面
	case TIPSSOURCE_FILLTIPS:  //提示是否灌注管道
		if(pProjectMan->tipsButton != TIPS_NONE)
		{
			if(pProjectMan->tipsButton == TIPS_OK)  //跳到泵选择页面
			{
				//跳到泵选择页面选择要灌注的管道
				SetScreen(SELECTPUMPPAGE_INDEX);
				BatchBegin(SELECTPUMPPAGE_INDEX);
				BatchSetButtonValue(PUMPSEL_PUMP1_BUTTON, pProjectMan->pumpSelPumpSel&PUMP1_MASK);
				BatchSetButtonValue(PUMPSEL_PUMP2_BUTTON, pProjectMan->pumpSelPumpSel&PUMP2_MASK);
				BatchSetButtonValue(PUMPSEL_PUMP3_BUTTON, pProjectMan->pumpSelPumpSel&PUMP3_MASK);
				BatchSetButtonValue(PUMPSEL_PUMP4_BUTTON, pProjectMan->pumpSelPumpSel&PUMP4_MASK);
				BatchSetButtonValue(PUMPSEL_PUMP5_BUTTON, pProjectMan->pumpSelPumpSel&PUMP5_MASK);
				BatchSetButtonValue(PUMPSEL_PUMP6_BUTTON, pProjectMan->pumpSelPumpSel&PUMP6_MASK);
				BatchSetButtonValue(PUMPSEL_PUMP7_BUTTON, pProjectMan->pumpSelPumpSel&PUMP7_MASK);
				BatchSetButtonValue(PUMPSEL_PUMP8_BUTTON, pProjectMan->pumpSelPumpSel&PUMP8_MASK);
				BatchEnd();
			}
			else  //跳到运行页面，放置槽板
			{
				//跳回到运行页面，执行动作
				SetScreen(RUNNINGPAGE_INDEX);
				pProjectMan->proStatus = PROJECTSTATUS_PLACEPLATE;
			}
			pProjectMan->tipsButton = TIPS_NONE;
			pProjectMan->tipsSource = TIPSSOURCE_NONE;
		}
		break;
	case TIPSSOURCE_RUNNINGPAUSE:  //暂停按钮按下
		{
			if(pProjectMan->tipsButton != TIPS_NONE)
			{
				if(pProjectMan->tipsButton == TIPS_OK)
				{
					//跳到暂停页面
					pProjectMan->pCurJumptoAction = pProjectMan->pCurRunningAction;
					SetTextValue(PAUSEPAGE_INDEX, PAUSE_ACTIONNAME_EDIT, pProjectMan->pCurJumptoAction->name);
					SetScreen(PAUSEPAGE_INDEX);
					pProjectMan->preProStatus = pProjectMan->proStatus;
					pProjectMan->proStatus = PROJECTSTATUS_PAUSE;

					cDebug("======== PAUSE the program!\n");
				}
				else
				{
					//跳回到运行页面，执行动作
					SetScreen(RUNNINGPAGE_INDEX);
				}
				pProjectMan->tipsButton = TIPS_NONE;
				pProjectMan->tipsSource = TIPSSOURCE_NONE;
			}
		}
		break;
	case TIPSSOURCE_RUNNINGSTOP:  //停止按钮按下
		{
			if(pProjectMan->tipsButton != TIPS_NONE)
			{
				if(pProjectMan->tipsButton == TIPS_OK)
				{
					//跳到项目页面
					SetScreen(PROJECTPAGE_INDEX);
					pProjectMan->runningType = RUNNING_NONE;
					pProjectMan->proStatus = PROJECTSTATUS_IDLE;

					cDebug("======== STOP the program!\n");
				}
				else
				{
					//跳回到运行页面，执行动作
					SetScreen(RUNNINGPAGE_INDEX);
				}
				pProjectMan->tipsButton = TIPS_NONE;
				pProjectMan->tipsSource = TIPSSOURCE_NONE;
			}
		}
		break;
	
	//tips1页面
	case TIPSSOURCE_PUTWASTETANK:
		if(pProjectMan->tipsButton == TIPS_OK)
		{
			//跳回到运行页面，执行动作
			SetScreen(RUNNINGPAGE_INDEX);

			pProjectMan->tipsButton = TIPS_NONE;
			pProjectMan->tipsSource = TIPSSOURCE_NONE;
		}
		break;

	case TIPSSOURCE_NONE:
		break;
	default:
		break;
	}
}


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
	pm->proStatus = PROJECTSTATUS_IDLE;
	pm->preProStatus = PROJECTSTATUS_IDLE;
	pm->tipsSource = TIPSSOURCE_NONE;
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
//	//处理UI数据
//	uint8 size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE); //从缓冲区中获取一条指令
//	if(size>0)//接受到指令
//	{
//		ProcessMessage((PCTRL_MSG)cmd_buffer, size);//处理指令
//	}
//
//	tipsPageProcess();

	//purgeProgram();

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
		break;
	default:
		break;
	}
}

#ifdef __cplusplus
}
#endif
