#include "pageCommon.h"
#include "managerment.h"
#include "CPrintf.h"

#ifdef __cplusplus
extern "C" {
#endif

void projectPageButtonProcess(uint16 control_id, uint8  state)
{
	switch(control_id)
	{
		case PRO_POSTPROJECT_BUTTON:
		{
			if(pProjectMan->pCurProject+1 > &project[PROJECT_COUNT-1])
				pProjectMan->pCurProject = &project[0];
			else
				pProjectMan->pCurProject += 1;

			SetTextValue(PROJECTPAGE_INDEX, PRO_PROJECTNAME_EDIT, pProjectMan->pCurProject->name);
		}
		break;
		case PRO_PREPROJECT_BUTTON:
		{
			if(pProjectMan->pCurProject-1 < &project[0])
				pProjectMan->pCurProject = &project[PROJECT_COUNT-1];
			else
				pProjectMan->pCurProject -= 1;

			SetTextValue(PROJECTPAGE_INDEX, PRO_PROJECTNAME_EDIT, pProjectMan->pCurProject->name);
		}
		break;
		case PRO_STARTPROJECT_BUTTON://开始项目，要初始化运行页面相应控件初始状态
		{
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, "runing");
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_PROJECT_EDIT, pProjectMan->pCurProject->name);
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_ACTION_EDIT, pProjectMan->pCurProject->action[0].name);
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_PUMP_EDIT, "0");
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_ADDTANK_EDIT, "0");
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_IMBITANK_EDIT, "0");
			SetTextValue(RUNNINGPAGE_INDEX, RUNNING_LOOPTIME_EDIT, "1");
			SetTextValueInt32(RUNNINGPAGE_INDEX, RUNNING_TOTALLOOPTIME_EDIT, pProjectMan->pCurProject->action[0].loopTime);

			if(pProjectMan->pCurProject->action[0].shakeTime.hour > 0
				|| pProjectMan->pCurProject->action[0].shakeTime.minute > 0)
			{
				uint32 rtcTime = pProjectMan->pCurProject->action[0].shakeTime.hour*3600
					+ pProjectMan->pCurProject->action[0].shakeTime.minute*60;
				SeTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC, rtcTime);
				//StartTimer(RUNNINGPAGE_INDEX, RUNNING_TIME_RTC);
			}

			pProjectMan->pCurRunningProject = pProjectMan->pCurProject;
			pProjectMan->pCurRunningAction = &pProjectMan->pCurRunningProject->action[0];
			pProjectMan->runningType = RUNNING_PROJECT; //同步工作线程

//			SetScreen(TIPS2PAGE_INDEX);//跳转到提示2页面
//			SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, FILLTUBE_TIPS);
//			pProjectMan->tipsSource = TIPSSOURCE_FILLTIPS;
//			
//			pProjectMan->proStatus = PROJECTSTATUS_WAITING;
//			pProjectMan->preProStatus = pProjectMan->proStatus;

			cDebug("========projectPage start to run the PROJECT program\n");
		}
		break;
		case PRO_BACK_BUTTON:
			break;
		default:
			cDebug("projectPage BUTTON error!\n");
		break;
	}
}

void projectPageEditProcess(uint16 control_id, uint8 *str)
{
	switch(control_id)
	{
		case PRO_STARTTANK_EDIT:
			pProjectMan->startTank = (uint8)StringToInt32(str);
			if(pProjectMan->startTank > pProjectMan->endTank)
				SetTextValue(PROJECTPAGE_INDEX, PRO_ENDTANK_EDIT, str);
		break;
		case PRO_ENDTANK_EDIT:
			pProjectMan->endTank = (uint8)StringToInt32(str);
			if(pProjectMan->startTank > pProjectMan->endTank)
				SetTextValue(PROJECTPAGE_INDEX, PRO_STARTTANK_EDIT, str);
		break;
		default:
			cDebug("projectPage EDIT error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
