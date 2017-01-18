#ifdef __cplusplus	  
extern "C" {
#endif

#include "project.h"
#include "managerment.h"
#include "../CPrintf.h"
#include "stdio.h"

#include "../Peripheral/dcmotor.h"
#include "../Peripheral/stepmotor.h"
#include "../Peripheral/io.h"

/************************************************************************/
/* 校准页面下拉列表字符串                                             */
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
/* 动作编辑页面下拉列表字符串                                             */
/************************************************************************/
const char actionPumpMenuText[9][3] = {
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"0",
};

const char actionTipsMenuText[3][9] = {
	"None",
	"Sample",
	"Membrane",
};
const char actionTipsMenuTextCh[3][5] = {
	"无",
	"样本",
	"膜条",
};

const char actionVoiceMenuText[4][7] = {
	"None",
	"Short",
	"Middle",
	"Long",
};
const char actionVoiceMenuTextCh[4][3] = {
	"无",
	"短",
	"中",
	"长",
};

const char actionSpeedMenuText[3][7] = {
	"Slow",
	"Middle",
	"Fast",
};
const char actionSpeedMenuTextCh[3][5] = {
	"慢速",
	"中速",
	"快速",
};

//char actSpeedMenuTextCh[3][5] = {
//	"慢速",
//	"中速",
//	"快速",
//};

/************************************************************************/
/* 信息页面下拉列表字符串                                             */
/************************************************************************/
const char langMenuText[2][8] = {
	"English",
	"Chinese",
};
const char langMenuTextCh[2][5] = {
	"英文",
	"中文",
};
/************************************************************************/
/* 动作初始化Demo                                                       */
/************************************************************************/
const Action_TypeDef actionDemo = {
	PUMP0,
	NO_TIPS,
	NO_VOICE,
	0,
	0,
	SLOW_SPEED,
	{0, 0},
	1,
	1,
	"action0"
};

//定义12个项目
//Project_TypeDef project[PROJECT_COUNT];
Project_TypeDef project[1];

//定义蠕动泵校准参数列表
float caliPumpPara[PUMP_COUNT];

//初始化项目结构体
void initProjectStruct(Project_TypeDef* pro, uint8 index, uint8 *name)
{
	uint16 i;
	char str[NAME_SIZE];

	memset(pro->name, '\0', NAME_SIZE);
	memcpy(pro->name, name, NAME_SIZE);	//修改项目名
	pro->index = index;

	for(i=0;i<ACTIONS_PER_PROJECT;i++)
	{
		memset(str, '\0', NAME_SIZE);
		memset(pro->action[i].name, '\0', NAME_SIZE);
		sprintf(str, "action%d", i);

		memcpy((void *)(&(pro->action[i])), (void *)(&actionDemo), sizeof(actionDemo));

		memcpy(pro->action[i].name, str, NAME_SIZE);  //修改动作名
		pro->action[i].index = i;//修改序号
	}
}

//初始化校准参数
void initCaliPumpPara(float para)
{
	uint16 i;
	for(i=0;i<PUMP_COUNT;i++)
		caliPumpPara[i] = para;
}

/******************************************************************************************************/

//灌注管道
void fillTube(void)
{
	uint16 i = 0;
	uint8_t relCoord;

	//提示是否灌注管道
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(TIPS2PAGE_INDEX);//跳转到提示2页面
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, "Whether fill the tube?");
	else
		SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, "是否灌注管道？");

	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	if(pProjectMan->tipsButton == TIPS_CANCEL)
	{
		pProjectMan->tipsButton = TIPS_NONE;
		SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
		return;
	}
	pProjectMan->tipsButton = TIPS_NONE;

	//状态显示灌注管道中
	if(pProjectMan->lang == 0)
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In filling tube……");
	else
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "灌注管道中……");

	SetScreen(SELECTPUMPPAGE_INDEX);//跳转到泵选择页面
	BatchBegin(SELECTPUMPPAGE_INDEX);//更新泵选择页面控件状态
	BatchSetButtonValue(PUMPSEL_PUMP1_BUTTON, pProjectMan->pumpSelPumpSel&PUMP1_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP2_BUTTON, pProjectMan->pumpSelPumpSel&PUMP2_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP3_BUTTON, pProjectMan->pumpSelPumpSel&PUMP3_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP4_BUTTON, pProjectMan->pumpSelPumpSel&PUMP4_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP5_BUTTON, pProjectMan->pumpSelPumpSel&PUMP5_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP6_BUTTON, pProjectMan->pumpSelPumpSel&PUMP6_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP7_BUTTON, pProjectMan->pumpSelPumpSel&PUMP7_MASK);
	BatchSetButtonValue(PUMPSEL_PUMP8_BUTTON, pProjectMan->pumpSelPumpSel&PUMP8_MASK);
	BatchEnd();

	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE); //等待泵选择完成
		
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面

	if(pProjectMan->pumpSelPumpSel == 0x00)
		return;

	//回原点
	//pStepMotor->Home();

	//将手动点回原位
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]为手动点

	//提示将废液槽置于槽板号1的位置
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the WASTE TANK in the Plate1");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "请把废液槽置于板号1位置");
	
	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
	
	//灌注要灌注的管道
	//pStepMotor->offset = STEPMOTOR_PUMP_OFFSET;
	pStepMotor->offset = pProjectMan->posCali2;
	for(i=0;i<8;i++)
	{
		if(pProjectMan->pumpSelPumpSel & (0x01<<i))
		{
			//将废液槽转到对应的泵位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
			pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]为废液槽
			
			os_wait(K_TMO, 6000, 0);

			pDCMotor->SetSpeed(i, 10);
			pDCMotor->SetCMD(i, ENABLE);				
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);				
			pDCMotor->SetCMD(i, DISABLE);

			while(1)
			{
				//提示是否继续灌注管道
				SetScreen(TIPS2PAGE_INDEX);//跳转到提示2页面
				if(pProjectMan->lang == 0)
					SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, "Whether continue fill the tube?");
				else
					SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, "是否继续灌注管道？");
				
				beepAlarm(1);
				while(pProjectMan->tipsButton == TIPS_NONE);
					
				SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
				if(pProjectMan->tipsButton == TIPS_CANCEL)
				{
					pProjectMan->tipsButton = TIPS_NONE;
					break;	
				}
				else
				{
					pProjectMan->tipsButton = TIPS_NONE;
					//继续灌注管道
					pDCMotor->SetSpeed(i, 10);
					pDCMotor->SetCMD(i, ENABLE);				
					os_wait(K_TMO, 6000, 0);	
					pDCMotor->SetCMD(i, DISABLE);
				}
			}

			os_wait(K_TMO, 6000, 0);
		}
	}
	//pStepMotor->offset = STEPMOTOR_OFFSET;
	pStepMotor->offset = pProjectMan->posCali1;
	
	//吸取废液
	wasteFluidAbsorb();

	//将废液槽转到手动点位置
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]为废液槽

	//提示移动废液槽
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please take away the Waste Tank");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "请移走废液槽");
	
	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
	pProjectMan->tipsButton = TIPS_NONE;

	//清状态显示
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");
}

//放置板卡
void placePlate(void)
{
	uint8_t startPlate, endPlate;
	uint8_t relCoord;
	uint8_t str[50];
	uint16_t i;
	startPlate = (pProjectMan->startTank-1)/TANK_PER_PLATE+1;
	endPlate = (pProjectMan->endTank-1)/TANK_PER_PLATE+1;
	
	//状态显示放置槽板中
	if(pProjectMan->lang == 0)
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In placing Plate……");
	else
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "放置槽板中……");

	//for(i=startPlate;i<=endPlate;i++)
	for(i=endPlate;i>=startPlate;i--)
	{
		//将对应槽板转到手动点
		relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);	
		pStepMotor->RelativePosition(relCoord, TANK_PER_PLATE*(i-1)+3);

		//提示放置槽板
		SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
		if(pProjectMan->lang == 0)
			sprintf(str, "Please place the PLATE in the Plate%d", i);
		else
			sprintf(str, "请将槽板置于【板号%d】位置", i);
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, str);

		beepAlarm(1);
		while(pProjectMan->tipsButton == TIPS_NONE);
			
		SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
		pProjectMan->tipsButton = TIPS_NONE;		
	}

	//清状态显示
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");

	//SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面	
}

//吸液
void imbibition(void)
{
	uint16_t i;
	uint8_t relCoord;

	if(pProjectMan->pCurRunningAction->imbiAmount)//吸液量大于0
	{
		//状态显示吸液中
		if(pProjectMan->lang == 0)
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In imbibing……");
		else
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "吸液中……");

		//启动真空泵
		pDCMotor->SetCMD(PUMP_VACUUM, ENABLE);
		//松开夹管阀
		pDCMotor->SetSpeed(PUMP_PINCH, 0);

		//延时，抽空废液瓶的空气形成气压差
		os_wait(K_TMO, 6000, 0);
		os_wait(K_TMO, 6000, 0);
		os_wait(K_TMO, 6000, 0);
		os_wait(K_TMO, 6000, 0);
		os_wait(K_TMO, 6000, 0);
		os_wait(K_TMO, 6000, 0);

		//for(i=pProjectMan->startTank;i<=pProjectMan->endTank;i++)
		for(i=pProjectMan->endTank;i>=pProjectMan->startTank;i--)
		{
			//更新吸液编辑框内容为当前槽
			SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_IMBITANK_EDIT, i);			

			//将对应槽转到吸液口位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[8]);	
			pStepMotor->RelativePosition(relCoord, i);

			//放下废液口
			pDCMotor->WastePump_SetPos(DOWN);
			//cDebug("DOWN\n");
			
			//延时对应时间
			os_wait(K_TMO, 6000*pProjectMan->pCurRunningAction->imbiAmount, 0);
			os_wait(K_TMO, 6000*pProjectMan->pCurRunningAction->imbiAmount, 0);
			os_wait(K_TMO, 6000*pProjectMan->pCurRunningAction->imbiAmount, 0);
			os_wait(K_TMO, 6000*pProjectMan->pCurRunningAction->imbiAmount, 0);

			//提起废液口
			pDCMotor->WastePump_SetPos(UP);
			//cDebug("UP\n");	
		}
		
		//延时，吸取剩下的液体
		os_wait(K_TMO, 6000, 0);
		os_wait(K_TMO, 6000, 0);
		os_wait(K_TMO, 6000, 0);
		os_wait(K_TMO, 6000, 0);
		os_wait(K_TMO, 6000, 0);
		os_wait(K_TMO, 6000, 0);

		//关闭夹管阀
		pDCMotor->SetSpeed(PUMP_PINCH, 100);
		//停止真空泵
		pDCMotor->SetCMD(PUMP_VACUUM, DISABLE);
		
		//更新吸液编辑框内容为0
		SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_IMBITANK_EDIT, 0);		
	}

	//清状态显示
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");	
}

//动作提示
void hint(void)
{
	uint16_t i;
	uint8_t relCoord;
	uint8_t str[50];

	if(pProjectMan->pCurRunningAction->tips != NO_TIPS)//有提示
	{
		//状态显示吸液中
		if(pProjectMan->lang == 0)
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In hinting……");
		else
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "提示中……");
		
		//for(i=pProjectMan->startTank;i<=pProjectMan->endTank;i++)
		for(i=pProjectMan->endTank;i>=pProjectMan->startTank;i--)
		{	
			//将对应槽转到手动点位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);	
			pStepMotor->RelativePosition(relCoord, i);
			
			//提示放置样品或膜条
			SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
			if(pProjectMan->pCurRunningAction->tips == SAMPLE_TIPS)
			{
				if(pProjectMan->lang == 0)
					sprintf(str, "Please place the SAMPLE in the Tank%d", i);
				else
					sprintf(str, "请将【样本】置于【槽号%d】位置", i);
				SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, str);				
			}
			else
			{
				if(pProjectMan->lang == 0)
					sprintf(str, "Please place the MEMBRANE in the Tank%d", i);
				else
					sprintf(str, "请将【膜条】置于【槽号%d】位置", i);
				SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, str);
			}

			while(pProjectMan->tipsButton == TIPS_NONE)
				beepAlarm(pProjectMan->pCurRunningAction->voice+1);

			SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
			pProjectMan->tipsButton = TIPS_NONE;
		}	
	}

	//清状态显示
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");
	SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面	
}

//加注
void adding(void)
{
	uint8_t relCoord, time;
	uint8_t str[50];
	uint16_t i;

	if(pProjectMan->pCurRunningAction->pump != 8)//有选择泵，编号8为0，表示无泵
	{
		//状态显示吸液中
		if(pProjectMan->lang == 0)
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In adding……");
		else
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "加注中……");

		//更新泵辑框内容为当前选择的泵编号
		SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_PUMP_EDIT, pProjectMan->pCurRunningAction->pump+1);		

		//for(i=pProjectMan->startTank;i<=pProjectMan->endTank;i++)
		//pStepMotor->offset = STEPMOTOR_PUMP_OFFSET;
		pStepMotor->offset = pProjectMan->posCali2;
		for(i=pProjectMan->endTank;i>=pProjectMan->startTank;i--)
		{
			//更新加注编辑框内容为当前槽
			SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_ADDTANK_EDIT, i);			

			//将对应槽转到吸液口位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[pProjectMan->pCurRunningAction->pump]);	
			pStepMotor->RelativePosition(relCoord, i);
			
			os_wait(K_TMO, 6000, 0);

			//蠕动泵加注
			time = pProjectMan->pCurRunningAction->addAmount*pProjectMan->pCaliPumpPara[pProjectMan->pCurRunningAction->pump];
			//SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_IMBITANK_EDIT, pProjectMan->pCurRunningAction->addAmount);
			//SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_IMBITANK_EDIT, 10*time);
			//cDebug("adding: %d mL\n", pProjectMan->pCurRunningAction->addAmount);
			pDCMotor->SetCMD(pProjectMan->pCurRunningAction->pump, ENABLE);
			os_wait(K_TMO, 10*time, 0);
			os_wait(K_TMO, 10*time, 0);
			os_wait(K_TMO, 10*time, 0);
			os_wait(K_TMO, 10*time, 0);
			os_wait(K_TMO, 10*time, 0);
			pDCMotor->SetCMD(pProjectMan->pCurRunningAction->pump, DISABLE);
			
			os_wait(K_TMO, 6000, 0);	
		}
		//pStepMotor->offset = STEPMOTOR_OFFSET;
		pStepMotor->offset = pProjectMan->posCali1;	
	}

	//更新加注编辑框内容为0
	SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_ADDTANK_EDIT, 0);
	//更新泵辑框内容为0
	SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_PUMP_EDIT, 0);

	//清状态显示
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");	
}

//孵育
void incubation(void)
{
	if(pProjectMan->pCurRunningAction->shakeTime.hour > 0
		|| pProjectMan->pCurRunningAction->shakeTime.minute > 0)//孵育时间大于0
	{
		//状态显示孵育中
		if(pProjectMan->lang == 0)
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In incubating……");
		else
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "孵育中……");

		//启动RTC
		StartTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC);

	   	//设置摇动速度
		switch(pProjectMan->pCurRunningAction->shakeSpeed)
		{
			case 0:	 //慢
				pStepMotor->SetSpeed(SPEDD_SLOW);
			break;
			case 1:	 //中
				pStepMotor->SetSpeed(SPEDD_MIDDLE);
			break;
			case 2:	 //快
				pStepMotor->SetSpeed(SPEDD_FAST);
			break;
			default: //默认
				pStepMotor->SetSpeed(SPEDD_SLOW);
			break;
		}

		//使能暂停 停止按钮
		SetControlVisiable(RUNNINGPAGE_INDEX, RUNNING_PAUSE_BUTTON, 1);
		SetControlVisiable(RUNNINGPAGE_INDEX, RUNNING_STOP_BUTTON, 1);

		//转动转盘并等待孵育时间到
		pStepMotor->SetCMD(ENABLE);

		while(pProjectMan->RTCTimeout == 0) //等待时间到
		{
			//检查是否有暂停
			if(pProjectMan->exception == EXCEPTION_PAUSE)
			{
				//pProjectMan->exception = EXCEPTION_NONE;

				//暂停RTC
				PauseTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC);

				//停止并对准槽
				switch(pProjectMan->pCurRunningAction->shakeSpeed)
				{
					case 0:	 //慢
						pStepMotor->StopAndAlign(2);
					break;
					case 1:	 //中
						pStepMotor->StopAndAlign(4);
					break;
					case 2:	 //快
						pStepMotor->StopAndAlign(7);
					break;
					default: //默认
						pStepMotor->StopAndAlign(2);
					break;
				}
				//pStepMotor->StopAndAlign(2);

				//等到恢复
				while(pProjectMan->exception != EXCEPTION_NONE)
				{
					if(pProjectMan->rotateFlag == 1)
					{
						//下一个槽
						pStepMotor->Position(CCW, 1);
						pProjectMan->rotateFlag = 0;
					}
				}

				if(pProjectMan->jumpTo == 1) //暂停页面的跳转功能用到
				{
					pProjectMan->curLoopTime = pProjectMan->pCurRunningAction->loopTime; //用于退出循环	
					pStepMotor->Home();	//回原点
					//pProjectMan->jumpTo = 0;
					return;
				}

				//设置摇动速度
				switch(pProjectMan->pCurRunningAction->shakeSpeed)
				{
					case 0:	 //慢
						pStepMotor->SetSpeed(SPEDD_SLOW);
					break;
					case 1:	 //中
						pStepMotor->SetSpeed(SPEDD_MIDDLE);
					break;
					case 2:	 //快
						pStepMotor->SetSpeed(SPEDD_FAST);
					break;
					default: //默认
						pStepMotor->SetSpeed(SPEDD_SLOW);
					break;
				}

				//转动转盘并等待孵育时间到
				pStepMotor->SetCMD(ENABLE);
				pProjectMan->RTCTimeout = 0;
				//恢复RTC
				StartTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC);
			}
			else if(pProjectMan->exception == EXCEPTION_STOP)
			{
				//pProjectMan->exception = EXCEPTION_NONE;

				StopTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC);

				//停止并对准槽
				switch(pProjectMan->pCurRunningAction->shakeSpeed)
				{
					case 0:	 //慢
						pStepMotor->StopAndAlign(2);
					break;
					case 1:	 //中
						pStepMotor->StopAndAlign(4);
					break;
					case 2:	 //快
						pStepMotor->StopAndAlign(7);
					break;
					default: //默认
						pStepMotor->StopAndAlign(2);
					break;
				}
				//pStepMotor->StopAndAlign(2);

				pProjectMan->RTCTimeout = 0;

				os_delete_task(TASK_PROJECT);	//删除自己	
			}
		}
		pProjectMan->RTCTimeout = 0;		

		//停止并对准槽
		switch(pProjectMan->pCurRunningAction->shakeSpeed)
		{
			case 0:	 //慢
				pStepMotor->StopAndAlign(2);
			break;
			case 1:	 //中
				pStepMotor->StopAndAlign(4);
			break;
			case 2:	 //快
				pStepMotor->StopAndAlign(7);
			break;
			default: //默认
				pStepMotor->StopAndAlign(2);
			break;
		}

		//除能暂停 停止按钮
		SetControlVisiable(RUNNINGPAGE_INDEX, RUNNING_PAUSE_BUTTON, 0);
		SetControlVisiable(RUNNINGPAGE_INDEX, RUNNING_STOP_BUTTON, 0);

		//结束需要回原点
		pStepMotor->Home();	
	}

	//清状态显示
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");	
}

//执行动作
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

	//清状态显示
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");

	//等到转盘停止
	while(!pStepMotor->IsStop());

	//状态显示吸液中
	if(pProjectMan->lang == 0)
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In preparing……");
	else
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "准备中……");

	//回原点
	pStepMotor->Home();

	//灌注管道
	fillTube();

	//放置板卡
	placePlate();

	//状态显示吸液中
	if(pProjectMan->lang == 0)
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "Preparation finish!");
	else
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "准备完成！");

	//执行动作
	for(i=0;i<ACTIONS_PER_PROJECT;i++)
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
		rtcTime = pProjectMan->pCurProject->action[i].shakeTime.hour*3600
					+ pProjectMan->pCurProject->action[i].shakeTime.minute*60;
		SeTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC, rtcTime);

		for(pProjectMan->curLoopTime=1;pProjectMan->curLoopTime<=pProjectMan->pCurRunningAction->loopTime;pProjectMan->curLoopTime++)
		{
			SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_LOOPTIME_EDIT, pProjectMan->curLoopTime);
			
			//吸液
			imbibition();

			//动作提示
			hint();

			//加注
			adding();

			//孵育
			incubation();
		}
	}

	//状态显示完成
	if(pProjectMan->lang == 0)
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "Finish!");
	else
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "完成!");
	
	//回原点
	pStepMotor->Home();

	//提示动作完成
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Project has Finish!");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "项目完成!");
	
	beepAlarm(5);
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	pProjectMan->tipsButton = TIPS_NONE;

	//跳到项目页面
	SetScreen(PROJECTPAGE_INDEX);	
}

/****************************************************************************************************/

//吸取废液
void wasteFluidAbsorb(void)
{
	uint8_t relCoord;

	//状态显示孵育中
	if(pProjectMan->lang == 0)
	{
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In absorbing waste fluid……");
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "In absorbing waste fluid……");
	}
	else
	{
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "吸取废液中……");
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "吸取废液中……");
	}

	//将废液槽转到废液口位置
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[8]); //废液口位置
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //将废液槽转到废液口

	//启动真空泵
	pDCMotor->SetCMD(PUMP_VACUUM, ENABLE);
	//松开夹管阀
	pDCMotor->SetSpeed(PUMP_PINCH, 0);

	//延时，抽空废液瓶的空气形成气压差
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);

	//放下废液口
	pDCMotor->WastePump_SetPos(DOWN);

	//延时一小段时间
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);

	//提起废液口
	pDCMotor->WastePump_SetPos(UP);

	//延时，吸取剩下的液体
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);
	os_wait(K_TMO, 6000, 0);

	//关闭夹管阀
	pDCMotor->SetSpeed(PUMP_PINCH, 100);
	//停止真空泵
	pDCMotor->SetCMD(PUMP_VACUUM, DISABLE);

	//状态显示孵育中
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");
	SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "");
}

//清洗程序
void purgeProgram(void)
{
	uint16_t i;
	uint8_t relCoord;
	uint8_t str[50];
				
	if(pProjectMan->purgePumpSel == 0x00)
	{
		SetButtonValue(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0x00); //设置按键为弹起状态
		return;
	}

	SetControlVisiable(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0);
	SetControlVisiable(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP1_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP2_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP3_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP4_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP5_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP6_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP7_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP8_BUTTON, 0);

	if(pProjectMan->lang == 0)
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "In preparing……");
	else
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "准备中……");

	//提示将要清洗的管道置于蒸馏水中
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the tube that to be washed in the distilled water");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "请将要清洗的管道置于蒸馏水中");

	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	//回原点
	pStepMotor->Home();

	//将手动点回原位
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]为手动点

	//提示将废液槽置于槽板号1的位置
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the Waste Tank to the plate1");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "请将废液槽置于【板号1】位置");

	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	if(pProjectMan->lang == 0)	
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "Preparation finish!");
	else
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "准备完成！");

	//清洗要清洗的管道
	//pStepMotor->offset = STEPMOTOR_PUMP_OFFSET;
	pStepMotor->offset = pProjectMan->posCali2;
	for(i=0;i<8;i++)
	{
		if(pProjectMan->purgePumpSel & (0x01<<i))
		{
			if(pProjectMan->lang == 0)
				sprintf(str, "Washing the tube %d ……", i+1);
			else
				sprintf(str, "清洗【管道%d】中……", i+1);
			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, str);

			//将废液槽转到对应的泵位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
			pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[8]为废液口

			pDCMotor->SetSpeed(i, 10);
			pDCMotor->SetCMD(i, ENABLE);				
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);	
			pDCMotor->SetCMD(i, DISABLE);

			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "");
		}
	}
	//pStepMotor->offset = STEPMOTOR_OFFSET;
	pStepMotor->offset = pProjectMan->posCali1;
	
	//吸取废液
	wasteFluidAbsorb();

	//提示将要清洗的管道置于清洁干燥处
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示2页面
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the tubes that to be washed in the clean and dry place");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "请将要清洗的管道置于清洁干燥处");

	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	//抽干要清洗的管道
	//pStepMotor->offset = STEPMOTOR_PUMP_OFFSET;
	pStepMotor->offset = pProjectMan->posCali2;
	for(i=0;i<8;i++)
	{
		if(pProjectMan->purgePumpSel & (0x01<<i))
		{
			if(pProjectMan->lang == 0)
				sprintf(str, "Pumping water from the tube %d", i+1);
			else
				sprintf(str, "抽空【管道%d】中……", i+1);
			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, str);

			//将废液槽转到对应的泵位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
			pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[8]为3号槽（即废液槽）

			pDCMotor->SetSpeed(i, 10);
			pDCMotor->SetCMD(i, ENABLE);			
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);
			pDCMotor->SetCMD(i, DISABLE);

			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "");
		}
	}
	//pStepMotor->offset = STEPMOTOR_OFFSET;
	pStepMotor->offset = pProjectMan->posCali1;

	//吸取废液
	wasteFluidAbsorb();

	//将废液槽转到手动点
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]); //手动点位置
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //将废液槽转到手动点

	//提示取走废液槽
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示2页面
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please take away the Waste Tank");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "请移走废液槽");
	
	beepAlarm(1);	
	while(pProjectMan->tipsButton == TIPS_NONE);
	
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	//提示清洗完成
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示2页面
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Wash finish!");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "清洗完成!");

	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
	
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	if(pProjectMan->lang == 0)
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "Wash finish!");
	else
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, "清洗完成!");

	SetButtonValue(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0x00); //设置按键为弹起状态
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
	
	//拉起废液口
	pDCMotor->WastePump_SetPos(UP);

	//回原点
	pStepMotor->Home();
	SetScreen(MAINPAGE_INDEX);
	beepAlarm(1);
}

/****************************************************************************************/
void calibraProgram(void)
{
	uint8_t i;
	float time = pProjectMan->pCaliPumpPara[pProjectMan->caliPumpSel];
	//cDebug("========caliPage start to run the PUMP program!\n");

//	cDebug("pProjectMan->caliPumpSel = %d\n", (uint16_t)pProjectMan->caliPumpSel);
//	pDCMotor->SetCMD(pProjectMan->caliPumpSel, ENABLE);
//	os_wait(K_TMO, 1000, 0);
//	pDCMotor->SetCMD(pProjectMan->caliPumpSel, DISABLE);
	
	pDCMotor->SetCMD(pProjectMan->caliPumpSel, ENABLE);
	//直流电机运行40次的1mL，所以共40mL
	for(i=0;i<40;i++)
	{
		os_wait(K_TMO, 10*time, 0);
		os_wait(K_TMO, 10*time, 0);
		os_wait(K_TMO, 10*time, 0);
		os_wait(K_TMO, 10*time, 0);
		os_wait(K_TMO, 10*time, 0);
	}
	pDCMotor->SetCMD(pProjectMan->caliPumpSel, DISABLE);

	SetControlEnable(CALIBRATIONPAGE_INDEX, CALI_PUMPSELECT_BUTTON, 1);
	SetControlEnable(CALIBRATIONPAGE_INDEX, CALI_START_BUTTON, 1);
}

#ifdef __cplusplus
}
#endif
