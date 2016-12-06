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
/* ������Ŀ����ṹ��                                                   */
/************************************************************************/
ProjectMan_TypeDef ProjectMan;
ProjectMan_TypeDef *pProjectMan = &ProjectMan;


void tipsPageProcess(void)
{
	//��ʾҳ����ఴť����
	switch(pProjectMan->tipsSource)
	{
	//tips1ҳ��
	case TIPSSOURCE_FILLTIPS:  //��ʾ�Ƿ��ע�ܵ�
		if(pProjectMan->tipsButton != TIPS_NONE)
		{
			if(pProjectMan->tipsButton == TIPS_OK)  //������ѡ��ҳ��
			{
				//������ѡ��ҳ��ѡ��Ҫ��ע�Ĺܵ�
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
			else  //��������ҳ�棬���ò۰�
			{
				//���ص�����ҳ�棬ִ�ж���
				SetScreen(RUNNINGPAGE_INDEX);
				pProjectMan->proStatus = PROJECTSTATUS_PLACEPLATE;
			}
			pProjectMan->tipsButton = TIPS_NONE;
			pProjectMan->tipsSource = TIPSSOURCE_NONE;
		}
		break;
	case TIPSSOURCE_RUNNINGPAUSE:  //��ͣ��ť����
		{
			if(pProjectMan->tipsButton != TIPS_NONE)
			{
				if(pProjectMan->tipsButton == TIPS_OK)
				{
					//������ͣҳ��
					pProjectMan->pCurJumptoAction = pProjectMan->pCurRunningAction;
					SetTextValue(PAUSEPAGE_INDEX, PAUSE_ACTIONNAME_EDIT, pProjectMan->pCurJumptoAction->name);
					SetScreen(PAUSEPAGE_INDEX);
					pProjectMan->preProStatus = pProjectMan->proStatus;
					pProjectMan->proStatus = PROJECTSTATUS_PAUSE;

					cDebug("======== PAUSE the program!\n");
				}
				else
				{
					//���ص�����ҳ�棬ִ�ж���
					SetScreen(RUNNINGPAGE_INDEX);
				}
				pProjectMan->tipsButton = TIPS_NONE;
				pProjectMan->tipsSource = TIPSSOURCE_NONE;
			}
		}
		break;
	case TIPSSOURCE_RUNNINGSTOP:  //ֹͣ��ť����
		{
			if(pProjectMan->tipsButton != TIPS_NONE)
			{
				if(pProjectMan->tipsButton == TIPS_OK)
				{
					//������Ŀҳ��
					SetScreen(PROJECTPAGE_INDEX);
					pProjectMan->runningType = RUNNING_NONE;
					pProjectMan->proStatus = PROJECTSTATUS_IDLE;

					cDebug("======== STOP the program!\n");
				}
				else
				{
					//���ص�����ҳ�棬ִ�ж���
					SetScreen(RUNNINGPAGE_INDEX);
				}
				pProjectMan->tipsButton = TIPS_NONE;
				pProjectMan->tipsSource = TIPSSOURCE_NONE;
			}
		}
		break;
	
	//tips1ҳ��
	case TIPSSOURCE_PUTWASTETANK:
		if(pProjectMan->tipsButton == TIPS_OK)
		{
			//���ص�����ҳ�棬ִ�ж���
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
/* ���ڽ��ջ�����                                                       */
/************************************************************************/
uint8 cmd_buffer[CMD_MAX_SIZE];

void initUI(void)
{
	//��ʼ����Ŀ�ṹ��
	char str[NAME_SIZE];
	uint16 i;
	uint32 j;
	
	for(i=0;i<PROJECT_COUNT;i++)
	{
		memset(str, '\0', NAME_SIZE);
		sprintf(str, "project%d", i);
		initProjectStruct(&project[i], str);
	}

	//��ʼ��У׼����
	initCaliPumpPara(4.0);

	//��ʼ����Ŀ����ṹ��
	initProjectMan(pProjectMan);

	/*��մ��ڽ��ջ�����*/
	queue_reset();

	//��ʱһ��ʱ��
	for(j=0;j<65536;j++);

	//������������
	SetHandShake();//������������
}

//void uartInterrupt(uint8 data)
//{
//	queue_push(data);
//}

void loopForever(void)
{
//	//����UI����
//	uint8 size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE); //�ӻ������л�ȡһ��ָ��
//	if(size>0)//���ܵ�ָ��
//	{
//		ProcessMessage((PCTRL_MSG)cmd_buffer, size);//����ָ��
//	}
//
//	tipsPageProcess();

	//purgeProgram();

	/************************************************************************/
	/*��������                                                              */
	/************************************************************************/
	//��Ŀ����
	switch(pProjectMan->runningType)
	{
	case RUNNING_NONE:
		break;
	case RUNNING_PROJECT: //��Ŀ������
	{
		projectProgram();
		pProjectMan->runningType = RUNNING_NONE;
	}
	break;
	case RUNNING_BACKFLOW:
		break;
	case RUNNING_PURGE:	 //ִ����ϴ����
		purgeProgram();
		pProjectMan->runningType = RUNNING_NONE;  //����ִ�����Ҫ�����־	
		break;
	case RUNNING_CALIBRATION:
		break;
	case RUNNING_HOME:
		//��ԭ��
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
