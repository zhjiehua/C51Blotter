#include "pageCommon.h"
#include "managerment.h"
#include "CPrintf.h"

#ifdef __cplusplus
extern "C" {
#endif

void projectEditPageButtonProcess(uint16 control_id, uint8  state)
{
	switch(control_id)
	{
		case PROEDIT_POSTPROJECT_BUTTON:
		{
			if(pProjectMan->pCurEditProject+1 > &project[PROJECT_COUNT-1])
				pProjectMan->pCurEditProject = &project[0];
			else
				pProjectMan->pCurEditProject += 1;
			pProjectMan->pCurEditAction = &pProjectMan->pCurEditProject->action[0];

			SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_PROJECTNAME_EDIT, pProjectMan->pCurEditProject->name);
			SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_ACTIONNAME_EDIT, pProjectMan->pCurEditAction->name);
		}
		break;
		case PROEDIT_PREPROJECT_BUTTON:
		{
			if(pProjectMan->pCurEditProject-1 < &project[0])
				pProjectMan->pCurEditProject = &project[PROJECT_COUNT-1];
			else
				pProjectMan->pCurEditProject -= 1;
			pProjectMan->pCurEditAction = &pProjectMan->pCurEditProject->action[0];

			SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_PROJECTNAME_EDIT, pProjectMan->pCurEditProject->name);
			SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_ACTIONNAME_EDIT, pProjectMan->pCurEditAction->name);
		}
		break;
		case PROEDIT_POSTACTION_BUTTON:
			{
				if(pProjectMan->pCurEditAction+1 > &pProjectMan->pCurEditProject->action[ACTION_COUNT_PER_PROJECT-1])
					pProjectMan->pCurEditAction = &pProjectMan->pCurEditProject->action[0];
				else
					pProjectMan->pCurEditAction += 1;

				SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_ACTIONNAME_EDIT, pProjectMan->pCurEditAction->name);
			}
		break;
		case PROEDIT_PREACTION_BUTTON:
			{
				if(pProjectMan->pCurEditAction-1 < &pProjectMan->pCurEditProject->action[0])
					pProjectMan->pCurEditAction = &pProjectMan->pCurEditProject->action[ACTION_COUNT_PER_PROJECT-1];
				else
					pProjectMan->pCurEditAction -= 1;

				SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_ACTIONNAME_EDIT, pProjectMan->pCurEditAction->name);
			}
		break;
		case PROEDIT_EDITACTION_BUTTON:  //跳到动作页面，更新动作页面控件信息
			{
				SetTextValue(ACTIONPAGE_INDEX, ACTEDIT_PUMPSEL_EDIT, actionPumpMenuText[pProjectMan->pCurEditAction->pump]);
				SetTextValue(ACTIONPAGE_INDEX, ACTEDIT_TIPSSEL_EDIT, actionTipsMenuText[pProjectMan->pCurEditAction->tips]);
				SetTextValue(ACTIONPAGE_INDEX, ACTEDIT_VOICESEL_EDIT, actionVoiceMenuText[pProjectMan->pCurEditAction->voice]);
				SetTextValueFloat(ACTIONPAGE_INDEX, ACTEDIT_ADDAMOUNT_EDIT, pProjectMan->pCurEditAction->addAmount);
				SetTextValueInt32(ACTIONPAGE_INDEX, ACTEDIT_IMBIAMOUNT_EDIT, pProjectMan->pCurEditAction->imbiAmout);
				SetTextValue(ACTIONPAGE_INDEX, ACTEDIT_SPEEDSEL_EDIT, actionSpeedMenuText[pProjectMan->pCurEditAction->shakeSpeed]);
				SetTextValueInt32(ACTIONPAGE_INDEX, ACTEDIT_TIMEHOUR_EDIT, pProjectMan->pCurEditAction->shakeTime.hour);
				SetTextValueInt32(ACTIONPAGE_INDEX, ACTEDIT_TIMEMIN_EDIT, pProjectMan->pCurEditAction->shakeTime.minute);
				SetTextValueInt32(ACTIONPAGE_INDEX, ACTEDIT_LOOPTIME_EDIT, pProjectMan->pCurEditAction->loopTime);

				pProjectMan->pump = pProjectMan->pCurEditAction->pump;
				pProjectMan->tips = pProjectMan->pCurEditAction->tips;
				pProjectMan->voice = pProjectMan->pCurEditAction->voice;
				pProjectMan->addAmount = pProjectMan->pCurEditAction->addAmount;
				pProjectMan->imbiAmout = pProjectMan->pCurEditAction->imbiAmout;
				pProjectMan->shakeSpeed = pProjectMan->pCurEditAction->shakeSpeed;
				pProjectMan->shakeTime.hour = pProjectMan->pCurEditAction->shakeTime.hour;
				pProjectMan->shakeTime.minute = pProjectMan->pCurEditAction->shakeTime.minute;
				pProjectMan->loopTime = pProjectMan->pCurEditAction->loopTime;

			}
		break;
		case PROEDIT_BACK_BUTTON:
			break;
		default:
			cDebug("projectEditPage BUTTON error!\n");
		break;
	}
}

void projectEditPageEditProcess(uint16 control_id, uint8 *str)
{
	switch(control_id)
	{
		case PROEDIT_PROJECTNAME_EDIT:
			memcpy(pProjectMan->pCurEditProject->name, str, NAME_SIZE);
		break;
		case PROEDIT_ACTIONNAME_EDIT:
			memcpy(pProjectMan->pCurEditAction->name, str, NAME_SIZE);
		break;
		default:
			cDebug("projectEditPage EDIT error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
