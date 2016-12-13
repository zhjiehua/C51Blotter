#include "pageCommon.h"
#include "managerment.h"
#include "CPrintf.h"
#include "./Peripheral/24cxx.h"
#include "project.h"

#ifdef __cplusplus
extern "C" {
#endif

void projectEditPageButtonProcess(uint16 control_id, uint8  state)
{
	switch(control_id)
	{
		case PROEDIT_POSTPROJECT_BUTTON:
		{
//			if(pProjectMan->pCurEditProject+1 > &project[PROJECT_COUNT-1])
//				pProjectMan->pCurEditProject = &project[0];
//			else
//				pProjectMan->pCurEditProject += 1;

			//从EEPROM读取项目数据
			uint16_t addrOffset;
			//cDebug("pProjectMan->pCurEditProject->index = %d\n", (uint16_t)pProjectMan->pCurEditProject->index);
			if(pProjectMan->pCurEditProject->index == PROJECT_COUNT-1)
				addrOffset = 0;  //读出第一个项目参数
			else
				addrOffset = pProjectMan->pCurEditProject->index+1;
			cDebug("addrOffset = %d\n", (uint16_t)addrOffset);
			AT24CXX_Read(PROJECT_BASEADDR+addrOffset*PROJECT_SIZE, (uint8_t*)project, PROJECT_SIZE);  //读出下一个项目参数			
			cDebug("project[0].name = %s\n", project[0].name);
			cDebug("project[0].index = %d\n", (uint16_t)(project[0].index));

			pProjectMan->pCurEditAction = &pProjectMan->pCurEditProject->action[0];

			SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_PROJECTNAME_EDIT, pProjectMan->pCurEditProject->name);
			SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_ACTIONNAME_EDIT, pProjectMan->pCurEditAction->name);
		}
		break;
		case PROEDIT_PREPROJECT_BUTTON:
		{
//			if(pProjectMan->pCurEditProject-1 < &project[0])
//				pProjectMan->pCurEditProject = &project[PROJECT_COUNT-1];
//			else
//				pProjectMan->pCurEditProject -= 1;

			//从EEPROM读取项目数据
			uint16_t addrOffset;
			if(pProjectMan->pCurEditProject->index == 0)
				addrOffset = PROJECT_COUNT-1;  //读出第一个项目参数
			else
				addrOffset = pProjectMan->pCurEditProject->index-1;
			AT24CXX_Read(PROJECT_BASEADDR+addrOffset*PROJECT_SIZE, (uint8_t*)project, PROJECT_SIZE);  //读出上一个项目参数

			pProjectMan->pCurEditAction = &pProjectMan->pCurEditProject->action[0];

			SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_PROJECTNAME_EDIT, pProjectMan->pCurEditProject->name);
			SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_ACTIONNAME_EDIT, pProjectMan->pCurEditAction->name);
		}
		break;
		case PROEDIT_POSTACTION_BUTTON:
			{
				if(pProjectMan->pCurEditAction+1 > &pProjectMan->pCurEditProject->action[ACTIONS_PER_PROJECT-1])
					pProjectMan->pCurEditAction = &pProjectMan->pCurEditProject->action[0];
				else
					pProjectMan->pCurEditAction += 1;

				SetTextValue(PROJECTEDITPAGE_INDEX, PROEDIT_ACTIONNAME_EDIT, pProjectMan->pCurEditAction->name);
			}
		break;
		case PROEDIT_PREACTION_BUTTON:
			{
				if(pProjectMan->pCurEditAction-1 < &pProjectMan->pCurEditProject->action[0])
					pProjectMan->pCurEditAction = &pProjectMan->pCurEditProject->action[ACTIONS_PER_PROJECT-1];
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
				SetTextValueInt32(ACTIONPAGE_INDEX, ACTEDIT_IMBIAMOUNT_EDIT, pProjectMan->pCurEditAction->imbiAmount);
				SetTextValue(ACTIONPAGE_INDEX, ACTEDIT_SPEEDSEL_EDIT, actionSpeedMenuText[pProjectMan->pCurEditAction->shakeSpeed]);
				SetTextValueInt32(ACTIONPAGE_INDEX, ACTEDIT_TIMEHOUR_EDIT, pProjectMan->pCurEditAction->shakeTime.hour);
				SetTextValueInt32(ACTIONPAGE_INDEX, ACTEDIT_TIMEMIN_EDIT, pProjectMan->pCurEditAction->shakeTime.minute);
				SetTextValueInt32(ACTIONPAGE_INDEX, ACTEDIT_LOOPTIME_EDIT, pProjectMan->pCurEditAction->loopTime);

				pProjectMan->pump = pProjectMan->pCurEditAction->pump;
				pProjectMan->tips = pProjectMan->pCurEditAction->tips;
				pProjectMan->voice = pProjectMan->pCurEditAction->voice;
				pProjectMan->addAmount = pProjectMan->pCurEditAction->addAmount;
				pProjectMan->imbiAmount = pProjectMan->pCurEditAction->imbiAmount;
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
	uint16_t addrOffset;
	switch(control_id)
	{
		case PROEDIT_PROJECTNAME_EDIT:
			memcpy(pProjectMan->pCurEditProject->name, str, NAME_SIZE);
			//cDebug("pProjectMan->pCurEditProject->name = %s\n", pProjectMan->pCurEditProject->name);
			addrOffset = (pProjectMan->pCurEditProject->index+1)*PROJECT_SIZE - NAME_SIZE;
			//cDebug("addrOffset = %d\n", addrOffset);
			AT24CXX_Write(PROJECT_BASEADDR+addrOffset, pProjectMan->pCurEditProject->name, NAME_SIZE);  //保存参数
			//cDebug("Save success\n");
		break;
		case PROEDIT_ACTIONNAME_EDIT:
			memcpy(pProjectMan->pCurEditAction->name, str, NAME_SIZE);
			addrOffset = pProjectMan->pCurEditProject->index*PROJECT_SIZE + (pProjectMan->pCurEditAction->index+1)*ACTION_SIZE - NAME_SIZE;
			AT24CXX_Write(PROJECT_BASEADDR+addrOffset, pProjectMan->pCurEditAction->name, NAME_SIZE);  //保存参数
		break;
		default:
			cDebug("projectEditPage EDIT error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
