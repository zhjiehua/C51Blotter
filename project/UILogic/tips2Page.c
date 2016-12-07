#include "pageCommon.h"
#include "managerment.h"
#include "CPrintf.h"

#ifdef __cplusplus
extern "C" {
#endif

void tips2PageButtonProcess(uint16 control_id, uint8  state)
{
	switch(control_id)
	{
	case TIPS2_OK_BUTTON:
		if(pProjectMan->exceptionButtonFlag == EXCEPTION_PAUSE)
		{
			pProjectMan->exception = EXCEPTION_PAUSE;
			pProjectMan->exceptionButtonFlag = EXCEPTION_NONE;

			//更新动作名称
			pProjectMan->pCurJumptoAction = pProjectMan->pCurRunningAction;
			SetTextValue(PAUSEPAGE_INDEX, PAUSE_ACTIONNAME_EDIT, pProjectMan->pCurJumptoAction->name);
			SetScreen(PAUSEPAGE_INDEX);
		}
		else if(pProjectMan->exceptionButtonFlag == EXCEPTION_STOP)
		{
			pProjectMan->exception = EXCEPTION_STOP;
			pProjectMan->exceptionButtonFlag = EXCEPTION_NONE;
			SetScreen(PROJECTPAGE_INDEX);
		}
		else	
			pProjectMan->tipsButton = TIPS_OK;
		break;
	case TIPS2_CANCEL_BUTTON:
		if(pProjectMan->exceptionButtonFlag != EXCEPTION_NONE)
			pProjectMan->exceptionButtonFlag = EXCEPTION_NONE;
		else
			pProjectMan->tipsButton = TIPS_CANCEL;
		break;
		default:
			cDebug("tips2Page BUTTON error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
