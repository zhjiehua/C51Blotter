#ifdef __cplusplus	  
extern "C" {
#endif

#include "project.h"
#include "managerment.h"
#include "CPrintf.h"
#include "stdio.h"

#include "../Peripheral/dcmotor.h"
#include "../Peripheral/stepmotor.h"
#include "../Peripheral/io.h"

/************************************************************************/
/* У׼ҳ�������б��ַ���                                             */
/************************************************************************/
const char caliPumpMenuText[8][3] = {
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
};

/************************************************************************/
/* �����༭ҳ�������б��ַ���                                             */
/************************************************************************/
const char actionPumpMenuText[8][3] = {
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
};

const char actionTipsMenuText[3][9] = {
	"None",
	"Sample",
	"Membrane",
};

const char actionVoiceMenuText[4][7] = {
	"None",
	"Short",
	"Middle",
	"Long",
};

const char actionSpeedMenuText[3][7] = {
	"Slow",
	"Middle",
	"Fast",
};

/************************************************************************/
/* ������ʼ��Demo                                                       */
/************************************************************************/
const Action_TypeDef actionDemo = {
	PUMP1,
	SAMPLE_TIPS,
	SHORT_VOICE,
	1.0,
	1,
	SLOW_SPEED,
	{0, 1},
	1,
	"action0"
};

//����12����Ŀ
Project_TypeDef project[PROJECT_COUNT];

//�����䶯��У׼�����б�
float caliPumpPara[PUMP_COUNT];

//��ʼ����Ŀ�ṹ��
void initProjectStruct(Project_TypeDef* pro, char *name)
{
	uint16 i;
	char str[NAME_SIZE];

	memset(pro->name, '\0', NAME_SIZE);
	memcpy(pro->name, name, NAME_SIZE);

	for(i=0;i<ACTION_COUNT_PER_PROJECT;i++)
	{
		memset(str, '\0', NAME_SIZE);
		memset(pro->action[i].name, '\0', NAME_SIZE);
		sprintf(str, "action%d", i);

		memcpy((void *)(&(pro->action[i])), (void *)(&actionDemo), sizeof(actionDemo));

		memcpy(pro->action[i].name, str, NAME_SIZE);
	}
}

//��ʼ��У׼����
void initCaliPumpPara(float para)
{
	uint16 i;
	for(i=0;i<PUMP_COUNT;i++)
		caliPumpPara[i] = para;
}

/******************************************************************************************************/

//��ע�ܵ�
void fillTube(void)
{
	uint16 i = 0;
	uint8_t relCoord;

	//��ʾ�Ƿ��ע�ܵ�
	SetScreen(TIPS2PAGE_INDEX);//��ת����ʾ2ҳ��
	SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, "Whether fill the tube?");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	if(pProjectMan->tipsButton == TIPS_CANCEL)
	{
		pProjectMan->tipsButton = TIPS_NONE;
		SetScreen(RUNNINGPAGE_INDEX);//��ת������ҳ��
		return;
	}
	pProjectMan->tipsButton = TIPS_NONE;

	//״̬��ʾ��ע�ܵ���
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In filling tube����");

	SetScreen(SELECTPUMPPAGE_INDEX);//��ת����ѡ��ҳ��
	BatchBegin(SELECTPUMPPAGE_INDEX);//���±�ѡ��ҳ��ؼ�״̬
	BatchSetButtonValue(PUMPSEL_PUMP1_BUTTON, pProjectMan->pumpSelPumpSel&PUMP1_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP2_BUTTON, pProjectMan->pumpSelPumpSel&PUMP2_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP3_BUTTON, pProjectMan->pumpSelPumpSel&PUMP3_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP4_BUTTON, pProjectMan->pumpSelPumpSel&PUMP4_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP5_BUTTON, pProjectMan->pumpSelPumpSel&PUMP5_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP6_BUTTON, pProjectMan->pumpSelPumpSel&PUMP6_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP7_BUTTON, pProjectMan->pumpSelPumpSel&PUMP7_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP8_BUTTON, pProjectMan->pumpSelPumpSel&PUMP8_MASK);
	BatchEnd();

	while(pProjectMan->tipsButton == TIPS_NONE) //�ȴ���ѡ�����
		beepAlarm(1);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(RUNNINGPAGE_INDEX);//��ת������ҳ��

	if(pProjectMan->pumpSelPumpSel == 0x00)
		return;

	//��ԭ��
	//pStepMotor->Home();

	//���ֶ����ԭλ
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]Ϊ�ֶ���

	//��ʾ����Һ�����ڲ۰��1��λ��
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ1ҳ��
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the WASTE TANK in the Plate1");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(RUNNINGPAGE_INDEX);//��ת������ҳ��
	
	//��עҪ��ע�Ĺܵ�
	for(i=0;i<8;i++)
	{
		if(pProjectMan->pumpSelPumpSel & (0x01<<i))
		{
			//����Һ��ת����Ӧ�ı�λ��
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
			pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]Ϊ��Һ��

			pDCMotor->SetSpeed(i, 10);
			pDCMotor->SetCMD(i, ENABLE);				
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);	
			pDCMotor->SetCMD(i, DISABLE);

			while(1)
			{
				//��ʾ�Ƿ������ע�ܵ�
				SetScreen(TIPS2PAGE_INDEX);//��ת����ʾ2ҳ��
				SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, "Whether continue fill the tube?");
				while(pProjectMan->tipsButton == TIPS_NONE)
					beepAlarm(1);
				SetScreen(RUNNINGPAGE_INDEX);//��ת������ҳ��
				if(pProjectMan->tipsButton == TIPS_CANCEL)
				{
					pProjectMan->tipsButton = TIPS_NONE;
					break;	
				}
				else
				{
					pProjectMan->tipsButton = TIPS_NONE;
					//������ע�ܵ�
					pDCMotor->SetSpeed(i, 10);
					pDCMotor->SetCMD(i, ENABLE);				
					os_wait(K_TMO, 6000, 0);	
					pDCMotor->SetCMD(i, DISABLE);
				}
			}
		}
	}
	
	//��ȡ��Һ
	wasteFluidAbsorb();

	//����Һ��ת���ֶ���λ��
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]Ϊ��Һ��

	//��ʾ�ƶ���Һ��
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ1ҳ��
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please take away the Waste Tank");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	SetScreen(RUNNINGPAGE_INDEX);//��ת������ҳ��
	pProjectMan->tipsButton = TIPS_NONE;

	//��״̬��ʾ
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");
}

//���ð忨
void placePlate(void)
{
	uint8_t startPlate, endPlate;
	uint8_t relCoord;
	uint8_t str[50];
	uint16_t i;
	startPlate = (pProjectMan->startTank-1)/TANK_PER_PLATE+1;
	endPlate = (pProjectMan->endTank-1)/TANK_PER_PLATE+1;
	
	//״̬��ʾ���ò۰���
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In placing Plate����");

	//for(i=startPlate;i<=endPlate;i++)
	for(i=endPlate;i>=startPlate;i--)
	{
		//����Ӧ�۰�ת���ֶ���
		relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);	
		pStepMotor->RelativePosition(relCoord, TANK_PER_PLATE*(i-1)+3);

		//��ʾ���ò۰�
		SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ1ҳ��
		sprintf(str, "Please place the PLATE in the Plate%d", i);
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, str);
		while(pProjectMan->tipsButton == TIPS_NONE)
			beepAlarm(1);
		SetScreen(RUNNINGPAGE_INDEX);//��ת������ҳ��
		pProjectMan->tipsButton = TIPS_NONE;		
	}

	//��״̬��ʾ
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");

	//SetScreen(RUNNINGPAGE_INDEX);//��ת������ҳ��	
}

//��Һ
void imbibition(void)
{
	uint16_t i;
	uint8_t relCoord;

	if(pProjectMan->pCurRunningAction->imbiAmount)//��Һ������0
	{
		//״̬��ʾ��Һ��
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In imbibing����");

		//������ձ�
		pDCMotor->SetCMD(PUMP_VACUUM, ENABLE);
		
		//for(i=pProjectMan->startTank;i<=pProjectMan->endTank;i++)
		for(i=pProjectMan->endTank;i>=pProjectMan->startTank;i--)
		{
			//������Һ�༭������Ϊ��ǰ��
			SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_IMBITANK_EDIT, i);			

			//����Ӧ��ת����Һ��λ��
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[8]);	
			pStepMotor->RelativePosition(relCoord, i);
			
			//���·�Һ��
			pDCMotor->WastePump_SetPos(DOWN);
			
			//��ʱ��Ӧʱ��
			os_wait(K_TMO, 600*pProjectMan->pCurRunningAction->imbiAmount, 0);
			
			//�����Һ��
			pDCMotor->WastePump_SetPos(UP);	
		}
		
		//ֹͣ��ձ�
		pDCMotor->SetCMD(PUMP_VACUUM, DISABLE);
		
		//������Һ�༭������Ϊ0
		SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_IMBITANK_EDIT, 0);		
	}

	//��״̬��ʾ
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");	
}

//������ʾ
void hint(void)
{
	uint16_t i;
	uint8_t relCoord;
	uint8_t str[50];

	if(pProjectMan->pCurRunningAction->tips != NO_TIPS)//����ʾ
	{
		//״̬��ʾ��Һ��
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In hinting����");
		
		//for(i=pProjectMan->startTank;i<=pProjectMan->endTank;i++)
		for(i=pProjectMan->endTank;i>=pProjectMan->startTank;i--)
		{	
			//����Ӧ��ת���ֶ���λ��
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);	
			pStepMotor->RelativePosition(relCoord, i);
			
			//��ʾ������Ʒ��Ĥ��
			SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ1ҳ��
			if(pProjectMan->pCurRunningAction->tips == SAMPLE_TIPS)
			{
				sprintf(str, "Please place the SAMPLE in the Tank%d", i);
				SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, str);				
			}
			else
			{
				sprintf(str, "Please place the MEMBRANE in the Tank%d", i);
				SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, str);
			}
			while(pProjectMan->tipsButton == TIPS_NONE)
				beepAlarm(pProjectMan->pCurRunningAction->voice+1);
			SetScreen(RUNNINGPAGE_INDEX);//��ת������ҳ��
			pProjectMan->tipsButton = TIPS_NONE;
		}	
	}

	//��״̬��ʾ
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");
	SetScreen(RUNNINGPAGE_INDEX);//��ת������ҳ��	
}

//��ע
void adding(void)
{
	uint8_t relCoord, time;
	uint8_t str[50];
	uint16_t i;

	if(pProjectMan->pCurRunningAction->pump != 8)//��ѡ��ã����8Ϊ0����ʾ�ޱ�
	{
		//״̬��ʾ��Һ��
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In adding����");

		//���±ü�������Ϊ��ǰѡ��ıñ��
		SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_PUMP_EDIT, pProjectMan->pCurRunningAction->pump+1);		

		//for(i=pProjectMan->startTank;i<=pProjectMan->endTank;i++)
		for(i=pProjectMan->endTank;i>=pProjectMan->startTank;i--)
		{
			//���¼�ע�༭������Ϊ��ǰ��
			SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_ADDTANK_EDIT, i);			

			//����Ӧ��ת����Һ��λ��
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[pProjectMan->pCurRunningAction->pump]);	
			pStepMotor->RelativePosition(relCoord, i);
			
			//�䶯�ü�ע
			pDCMotor->SetCMD(pProjectMan->pCurRunningAction->pump, ENABLE);
			time = pProjectMan->pCurRunningAction->addAmount*pProjectMan->pCaliPumpPara[pProjectMan->pCurRunningAction->pump];
			os_wait(K_TMO, 150*time, 0);
			pDCMotor->SetCMD(pProjectMan->pCurRunningAction->pump, DISABLE);	
		}	
	}

	//���¼�ע�༭������Ϊ0
	SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_ADDTANK_EDIT, 0);
	//���±ü�������Ϊ0
	SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_PUMP_EDIT, 0);

	//��״̬��ʾ
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");	
}

//����
void incubation(void)
{
	if(pProjectMan->pCurRunningAction->shakeTime.hour > 0
		|| pProjectMan->pCurRunningAction->shakeTime.minute > 0)//����ʱ�����0
	{
		//״̬��ʾ������
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In incubating����");

		//����RTC
		StartTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC);

	   	//����ҡ���ٶ�
		switch(pProjectMan->pCurRunningAction->shakeSpeed)
		{
			case 0:	 //��
				pStepMotor->SetSpeed(5);
			break;
			case 1:	 //��
				pStepMotor->SetSpeed(9);
			break;
			case 2:	 //��
				pStepMotor->SetSpeed(13);
			break;
			default: //Ĭ��
				pStepMotor->SetSpeed(5);
			break;
		}

		//ʹ����ͣ ֹͣ��ť
		SetControlVisiable(RUNNINGPAGE_INDEX, RUNNING_PAUSE_BUTTON, 1);
		SetControlVisiable(RUNNINGPAGE_INDEX, RUNNING_STOP_BUTTON, 1);

		//ת��ת�̲��ȴ�����ʱ�䵽
		pStepMotor->SetCMD(ENABLE);

		while(pProjectMan->RTCTimeout == 0) //�ȴ�ʱ�䵽
		{
			//����Ƿ�����ͣ
			if(pProjectMan->exception == EXCEPTION_PAUSE)
			{
				//��ͣRTC
				PauseTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC);

				//ֹͣ����׼��
				pStepMotor->StopAndAlign(2);

				//�ȵ��ָ�
				while(pProjectMan->exception != EXCEPTION_NONE)
				{
					if(pProjectMan->rotateFlag == 1)
					{
						//��һ����
						pStepMotor->Position(CCW, 1);
						pProjectMan->rotateFlag = 0;
					}
				}

				if(pProjectMan->jumpTo == 1) //��ͣҳ�����ת�����õ�
				{
					pProjectMan->curLoopTime = pProjectMan->pCurRunningAction->loopTime; //�����˳�ѭ��	
					pStepMotor->Home();	//��ԭ��
					//pProjectMan->jumpTo = 0;
					return;
				}

				//����ҡ���ٶ�
				switch(pProjectMan->pCurRunningAction->shakeSpeed)
				{
					case 0:	 //��
						pStepMotor->SetSpeed(5);
					break;
					case 1:	 //��
						pStepMotor->SetSpeed(9);
					break;
					case 2:	 //��
						pStepMotor->SetSpeed(13);
					break;
					default: //Ĭ��
						pStepMotor->SetSpeed(5);
					break;
				}

				//ת��ת�̲��ȴ�����ʱ�䵽
				pStepMotor->SetCMD(ENABLE);
				pProjectMan->RTCTimeout = 0;
				//�ָ�RTC
				StartTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC);
			}
			else if(pProjectMan->exception == EXCEPTION_STOP)
			{
				StopTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC);

				//ֹͣ����׼��
				pStepMotor->StopAndAlign(2);

				pProjectMan->RTCTimeout = 0;

				os_delete_task(TASK_PROJECT);	//ɾ���Լ�	
			}
		}
		pProjectMan->RTCTimeout = 0;		

		//������ͣ ֹͣ��ť
		SetControlVisiable(RUNNINGPAGE_INDEX, RUNNING_PAUSE_BUTTON, 0);
		SetControlVisiable(RUNNINGPAGE_INDEX, RUNNING_STOP_BUTTON, 0);

		//������Ҫ��ԭ��
		pStepMotor->Home();	
	}

	//��״̬��ʾ
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");	
}

//ִ�ж���
void execAction(Action_TypeDef act)
{
	 ;
}

/****************************************************************************************************/

void projectProgram(void)
{
	uint16_t i;
	uint32_t rtcTime;
	pProjectMan->curTank = pProjectMan->startTank;
	pProjectMan->curLoopTime = 1;
	pProjectMan->RTCTimeout = 0;

	//��״̬��ʾ
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");

	//�ȵ�ת��ֹͣ
	while(!pStepMotor->IsStop());

	//״̬��ʾ��Һ��
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In preparing����");

	//��ԭ��
	pStepMotor->Home();

	//��ע�ܵ�
	fillTube();

	//���ð忨
	placePlate();

	//״̬��ʾ��Һ��
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "Preparation finish");

	//ִ�ж���
	for(i=0;i<ACTION_COUNT_PER_PROJECT;i++)
	{
		if(pProjectMan->jumpTo == 1)
		{
			if(&pProjectMan->pCurRunningProject->action[i] != pProjectMan->pCurRunningAction)
				continue;
			else
				pProjectMan->jumpTo = 0;	
		}
		pProjectMan->pCurRunningAction = &pProjectMan->pCurRunningProject->action[i];		
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_ACTION_EDIT, pProjectMan->pCurRunningAction->name);
		SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_PUMP_EDIT, 0);
		SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_ADDTANK_EDIT, 0);
		SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_IMBITANK_EDIT, 0);
		SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_LOOPTIME_EDIT, 1);
		SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_TOTALLOOPTIME_EDIT, pProjectMan->pCurRunningAction->loopTime);
		rtcTime = pProjectMan->pCurProject->action[0].shakeTime.hour*3600
					+ pProjectMan->pCurProject->action[0].shakeTime.minute*60;
		SeTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC, rtcTime);

		for(pProjectMan->curLoopTime=1;pProjectMan->curLoopTime<=pProjectMan->pCurRunningAction->loopTime;pProjectMan->curLoopTime++)
		{
			SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_LOOPTIME_EDIT, pProjectMan->curLoopTime);
			
			//��Һ
			imbibition();

			//������ʾ
			hint();

			//��ע
			adding();

			//����
			incubation();
		}
	}

	//״̬��ʾ���
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "Finish!");
	
	//��ʾ�������
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ1ҳ��
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Project has Finish!");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(pProjectMan->pCurRunningAction->voice+1);
	pProjectMan->tipsButton = TIPS_NONE;

	//������Ŀҳ��
	SetScreen(PROJECTPAGE_INDEX);	
}

/****************************************************************************************************/

//��ȡ��Һ
void wasteFluidAbsorb(void)
{
	uint8_t relCoord;

	//״̬��ʾ������
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In absorbing waste fluid����");
	SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "In absorbing waste fluid����");

	//����Һ��ת����Һ��λ��
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[8]); //��Һ��λ��
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //����Һ��ת����Һ��

	//������ձ�
	pDCMotor->SetCMD(PUMP_VACUUM, ENABLE);

	//���·�Һ��
	pDCMotor->WastePump_SetPos(DOWN);

	//��ʱһС��ʱ��
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);

	//�����Һ��
	pDCMotor->WastePump_SetPos(UP);

	//ֹͣ��ձ�
	pDCMotor->SetCMD(PUMP_VACUUM, DISABLE);

	//״̬��ʾ������
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");
	SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "");
}

//��ϴ����
void purgeProgram(void)
{
	uint16_t i;
	uint8_t relCoord;
	uint8_t str[50];
				
	if(pProjectMan->purgePumpSel == 0x00)
	{
		SetButtonValue(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0x00); //���ð���Ϊ����״̬
		return;
	}

	SetControlVisiable(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP1_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP2_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP3_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP4_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP5_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP6_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP7_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP8_BUTTON, 0);

	SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "In preparing����");

	//��ʾ��Ҫ��ϴ�Ĺܵ���������ˮ��
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ1ҳ��
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the tube that to be washed in the distilled water");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	//��ԭ��
	pStepMotor->Home();

	//���ֶ����ԭλ
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]Ϊ�ֶ���

	//��ʾ����Һ�����ڲ۰��1��λ��
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ1ҳ��
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the Waste Tank to the plate1");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��
	
	SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "Preparation finish");

	//��ϴҪ��ϴ�Ĺܵ�
	for(i=0;i<8;i++)
	{
		if(pProjectMan->purgePumpSel & (0x01<<i))
		{
			sprintf(str, "Washing the tube %d", i+1);
			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, str);

			//����Һ��ת����Ӧ�ı�λ��
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
			pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[8]Ϊ��Һ��

			pDCMotor->SetSpeed(i, 10);
			pDCMotor->SetCMD(i, ENABLE);				
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);	
			pDCMotor->SetCMD(i, DISABLE);

			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "");
		}
	}
	
	//��ȡ��Һ
	wasteFluidAbsorb();

	//��ʾ��Ҫ��ϴ�Ĺܵ����������ﴦ
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ2ҳ��
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the tubes that to be washed in the clean and dry place");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	//���Ҫ��ϴ�Ĺܵ�
	for(i=0;i<8;i++)
	{
		if(pProjectMan->purgePumpSel & (0x01<<i))
		{
			sprintf(str, "Pumping water from the tube %d", i+1);
			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, str);

			//����Һ��ת����Ӧ�ı�λ��
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
			pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[8]Ϊ3�Ųۣ�����Һ�ۣ�

			pDCMotor->SetSpeed(i, 10);
			pDCMotor->SetCMD(i, ENABLE);			
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);
			pDCMotor->SetCMD(i, DISABLE);

			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "");
		}
	}

	//��ȡ��Һ
	wasteFluidAbsorb();

	//����Һ��ת���ֶ���
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]); //�ֶ���λ��
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //����Һ��ת���ֶ���

	//��ʾȡ�߷�Һ��
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ2ҳ��
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please take away the Waste Tank");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	//��ʾ��ϴ���
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ2ҳ��
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Wash finish");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "Wash finish");

	SetButtonValue(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0x00); //���ð���Ϊ����״̬
	SetControlVisiable(PURGEPAGE_INDEX, PURGE_START_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP1_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP2_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP3_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP4_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP5_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP6_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP7_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP8_BUTTON, 1);	
}

/****************************************************************************************/

void homeProgram(void)
{
	//��ԭ��
	pStepMotor->Home();
	SetScreen(MAINPAGE_INDEX);
	beepAlarm(1);
}

/****************************************************************************************/
void calibraProgram(void)
{
	;
}

#ifdef __cplusplus
}
#endif
