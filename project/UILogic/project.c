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
/* 动作初始化Demo                                                       */
/************************************************************************/
const Action_TypeDef actionDemo = {
	PUMP1,
	SAMPLE_TIPS,
	SHORT_VOICE,
	1.0,
	0,
	SLOW_SPEED,
	{0, 1},
	1,
	"action0"
};

//定义12个项目
Project_TypeDef project[PROJECT_COUNT];

//定义蠕动泵校准参数列表
float caliPumpPara[PUMP_COUNT];

//初始化项目结构体
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

//初始化校准参数
void initCaliPumpPara(float para)
{
	uint16 i;
	for(i=0;i<PUMP_COUNT;i++)
		caliPumpPara[i] = para;
}

/******************************************************************************************************/

////提示1页面提示内容
//void tips1Show(TipsSource_TypeDef tipsSource, char *str)
//{
//	SetScreen(TIPS1PAGE_INDEX);
//	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, str);
//	pProjectMan->tipsSource = tipsSource;
//}

//灌注管道
void fillTube(void)
{
	uint16 i = 0;
	uint8_t relCoord;

	//提示是否灌注管道
	SetScreen(TIPS2PAGE_INDEX);//跳转到提示2页面
	SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, "Whether fill the tube?");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	if(pProjectMan->tipsButton == TIPS_CANCEL)
	{
		pProjectMan->tipsButton = TIPS_NONE;
		SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
		return;
	}
	pProjectMan->tipsButton = TIPS_NONE;

	//状态显示灌注管道中
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In filling tube……");

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

	while(pProjectMan->tipsButton == TIPS_NONE) //等待泵选择完成
		beepAlarm(1);
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
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the WASTE TANK in the Plate1");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
	
	//灌注要灌注的管道
	for(i=0;i<8;i++)
	{
		if(pProjectMan->pumpSelPumpSel & (0x01<<i))
		{
			//将废液槽转到对应的泵位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
			pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]为废液槽

			pDCMotor->SetSpeed(i, 10);
			pDCMotor->SetCMD(i, ENABLE);				
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);	
			pDCMotor->SetCMD(i, DISABLE);

			while(1)
			{
				//提示是否继续灌注管道
				SetScreen(TIPS2PAGE_INDEX);//跳转到提示2页面
				SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, "Whether continue fill the tube?");
				while(pProjectMan->tipsButton == TIPS_NONE)
					beepAlarm(1);
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
		}
	}
	
	//吸取废液
	wasteFluidAbsorb();

	//将废液槽转到手动点位置
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]为废液槽

	//提示移动废液槽
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please take away the Waste Tank");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(1);
	SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
	pProjectMan->tipsButton = TIPS_NONE;

	//清状态显示
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "");

	//SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
}

//放置板卡
void placePlate(void)
{
	uint8_t startPlate, endPlate;
	uint8_t relCoord;
	uint8_t str[50];
	uint16_t i;
	startPlate = pProjectMan->startTank/TANK_PER_PLATE+1;
	endPlate = pProjectMan->endTank/TANK_PER_PLATE+1;
	
	for(i=startPlate;i<=endPlate;i++)
	{
		//将对应槽板转到手动点
		relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);	
		pStepMotor->RelativePosition(relCoord, TANK_PER_PLATE*(i-1)+3);

		//提示放置槽板
		SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
		sprintf(str, "Please place the PLATE in the Plate%d", i);
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, str);
		while(pProjectMan->tipsButton == TIPS_NONE)
			beepAlarm(1);
		SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面
		pProjectMan->tipsButton = TIPS_NONE;		
	}
	//SetScreen(RUNNINGPAGE_INDEX);//跳转到运行页面	
}

//吸液
void imbibition(void)
{
	uint16_t i;

	if(pProjectMan->pCurRunningAction->imbiAmount)//吸液量大于0
	{
		//状态显示吸液中
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In imbibing……");

		//启动真空泵
		pDCMotor->SetCMD(PUMP_VACUUM, ENABLE);
		
		for(i=pProjectMan->startTank;i<=pProjectMan->endTank;i++)
		{
			//更新吸液编辑框内容为当前槽
			SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_IMBITANK_EDIT, i);			

			//将对应槽转到吸液口位置	
			pStepMotor->RelativePosition(AbsCoordinate[8], i);
			
			//放下废液口
			pDCMotor->WastePump_SetPos(DOWN);
			
			//延时对应时间
			os_wait(K_TMO, 600*pProjectMan->pCurRunningAction->imbiAmount, 0);
			
			//提起废液口
			pDCMotor->WastePump_SetPos(UP);	
		}
		
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
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In hinting……");
		
		for(i=pProjectMan->startTank;i<=pProjectMan->endTank;i++)
		{	
			//将对应槽转到手动点位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);	
			pStepMotor->RelativePosition(relCoord, i);
			
			//提示放置样品或膜条
			SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
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
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In adding……");

		//更新泵辑框内容为当前选择的泵编号
		SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_PUMP_EDIT, pProjectMan->pCurRunningAction->pump);		

		for(i=pProjectMan->startTank;i<=pProjectMan->endTank;i++)
		{
			//更新加注编辑框内容为当前槽
			SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_ADDTANK_EDIT, i);			

			//将对应槽转到吸液口位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[pProjectMan->pCurRunningAction->pump]);	
			pStepMotor->RelativePosition(relCoord, i);
			
			//蠕动泵加注
			pDCMotor->SetCMD(pProjectMan->pCurRunningAction->pump, ENABLE);
			time = pProjectMan->pCurRunningAction->addAmount*pProjectMan->pCaliPumpPara[pProjectMan->pCurRunningAction->pump];
			os_wait(K_TMO, 150*time, 0);
			pDCMotor->SetCMD(pProjectMan->pCurRunningAction->pump, ENABLE);
			
		}	
	}

	//更新加注编辑框内容为0
	SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_ADDTANK_EDIT, 0);

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
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "In incubating……");

		//启动RTC
		StartTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC);

	   	//设置摇动速度
		switch(pProjectMan->pCurRunningAction->shakeSpeed)
		{
			case 0:	 //慢
				pStepMotor->SetSpeed(5);
			break;
			case 1:	 //中
				pStepMotor->SetSpeed(7);
			break;
			case 2:	 //快
				pStepMotor->SetSpeed(8);
			break;
			default: //默认
				pStepMotor->SetSpeed(5);
			break;
		}

		//转动转盘并等待孵育时间到
		pStepMotor->SetCMD(ENABLE);
		while(!pProjectMan->RTCTimeout); //等待时间到
		pProjectMan->RTCTimeout = 0;
		
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

	//灌注管道
	fillTube();

	//放置板卡
	placePlate();

	//执行动作
	for(i=0;i<ACTION_COUNT_PER_PROJECT;i++)
	{
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
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "Finish!");
	
	//提示动作完成
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Project has Finish!");
	while(pProjectMan->tipsButton == TIPS_NONE)
		beepAlarm(pProjectMan->pCurRunningAction->voice+1);
	pProjectMan->tipsButton = TIPS_NONE;

	//跳到项目页面
	SetScreen(PROJECTPAGE_INDEX);	
}

/****************************************************************************************************/

//吸取废液
void wasteFluidAbsorb(void)
{
	uint8_t relCoord;

	//将废液槽转到废液口位置
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[8]); //废液口位置
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //将废液槽转到废液口

	//启动真空泵
	pDCMotor->SetCMD(PUMP_VACUUM, ENABLE);

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

	//提起废液口
	pDCMotor->WastePump_SetPos(UP);

	//停止真空泵
	pDCMotor->SetCMD(PUMP_VACUUM, DISABLE);
}

//清洗程序
void purgeProgram(void)
{
	uint8_t i;
	uint8_t relCoord;

	if(pProjectMan->purgePumpSel == 0x00)
	{
		SetButtonValue(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0x00); //设置按键为弹起状态
		return;
	}

	//提示将要清洗的管道置于蒸馏水中
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the tube that to be washed in the distilled water");
	while(pProjectMan->tipsButton == TIPS_NONE);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	//回原点
	//pStepMotor->Home();

	//将手动点回原位
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]);
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[9]为手动点

	//提示将废液槽置于槽板号1的位置
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示1页面
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the Waste Tank to the plate2");
	while(pProjectMan->tipsButton == TIPS_NONE);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面
	
	//清洗要清洗的管道
	for(i=0;i<8;i++)
	{
		if(pProjectMan->purgePumpSel & (0x01<<i))
		{
			//将废液槽转到对应的泵位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
			pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[8]为废液槽

			pDCMotor->SetSpeed(i, 10);
			pDCMotor->SetCMD(i, ENABLE);				
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);	
			pDCMotor->SetCMD(i, DISABLE);
		}
	}
	
	//吸取废液
	wasteFluidAbsorb();

	//提示将要清洗的管道置于清洁干燥处
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示2页面
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the tube that to be washed in the clean and dry place");
	while(pProjectMan->tipsButton == TIPS_NONE);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	//抽干要清洗的管道
	for(i=0;i<8;i++)
	{
		if(pProjectMan->purgePumpSel & (0x01<<i))
		{
			//将废液槽转到对应的泵位置
			relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
			pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[8]为3号槽（即废液槽）

			pDCMotor->SetSpeed(i, 10);
			pDCMotor->SetCMD(i, ENABLE);			
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);
			os_wait(K_TMO, 6000, 0);
			pDCMotor->SetCMD(i, DISABLE);
		}
	}

	//吸取废液
	wasteFluidAbsorb();

	//将废液槽转到手动点
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]); //手动点位置
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //将废液槽转到手动点

	//提示取走废液槽
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示2页面
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please take away the Waste Tank");
	while(pProjectMan->tipsButton == TIPS_NONE);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	//提示清洗完成
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示2页面
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Wash finish");
	while(pProjectMan->tipsButton == TIPS_NONE);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	SetButtonValue(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0x00); //设置按键为弹起状态	
}

#ifdef __cplusplus
}
#endif
