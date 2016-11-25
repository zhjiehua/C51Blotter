#include "pageCommon.h"
#include "managerment.h"
#include "CPrintf.h"

#ifdef __cplusplus
extern "C" {
#endif

void actionPageButtonProcess(uint16 control_id, uint8  state)
{
	switch(control_id)
	{
		case ACTEDIT_SAVE_BUTTON:
		{
			//保存校准参数
			pProjectMan->pCurEditAction->pump = pProjectMan->pump;
			pProjectMan->pCurEditAction->tips = pProjectMan->tips;
			pProjectMan->pCurEditAction->voice = pProjectMan->voice;
			pProjectMan->pCurEditAction->addAmount = pProjectMan->addAmount;
			pProjectMan->pCurEditAction->imbiAmout = pProjectMan->imbiAmout;
			pProjectMan->pCurEditAction->shakeSpeed = pProjectMan->shakeSpeed;
			pProjectMan->pCurEditAction->shakeTime.hour = pProjectMan->shakeTime.hour;
			pProjectMan->pCurEditAction->shakeTime.minute = pProjectMan->shakeTime.minute;
			pProjectMan->pCurEditAction->loopTime = pProjectMan->loopTime;
			cDebug("========actionPage Save the ACTION data!\n");
		}
		break;
		case ACTEDIT_PUMPSEL_BUTTON:
		break;
		case ACTEDIT_TIPSSEL_BUTTON:
			break;
		case ACTEDIT_VOICESEL_BUTTON:
			break;
		case ACTEDIT_SPEEDSEL_BUTTON:
			break;
		case ACTEDIT_BACK_BUTTON:
		break;
		default:
			cDebug("actionPage BUTTON error!\n");
		break;
	}
}

void actionPageEditProcess(uint16 control_id, uint8 *str)
{
	switch(control_id)
	{
		case ACTEDIT_ADDAMOUNT_EDIT:
			pProjectMan->addAmount = StringToFloat(str);
		break;
		case ACTEDIT_IMBIAMOUNT_EDIT:
			pProjectMan->imbiAmout = StringToInt32(str);
			break;
		case ACTEDIT_TIMEHOUR_EDIT:
			pProjectMan->shakeTime.hour = StringToInt32(str);
			break;
		case ACTEDIT_TIMEMIN_EDIT:
			pProjectMan->shakeTime.minute = StringToInt32(str);
			break;
		case ACTEDIT_LOOPTIME_EDIT:
			pProjectMan->loopTime = StringToInt32(str);
			break;
		default:
			cDebug("actionPage EDIT error!\n");
		break;
	}
}

void actionPageMenuProcess(uint16 control_id, uint8 item)
{
	switch(control_id)
	{
	case ACTEDIT_PUMPSEL_MENU:
		{
			pProjectMan->pump = item;
		}
		break;
	case ACTEDIT_TIPSSEL_MENU:
		{
			pProjectMan->tips = item;
		}
		break;
	case ACTEDIT_VOICESEL_MENU:
		{
			pProjectMan->voice = item;
		}
		break;
	case ACTEDIT_SPEEDSEL_MENU:
		{
			pProjectMan->shakeSpeed = item;
		}
		break;
	default:
		cDebug("actionPage MENU error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
