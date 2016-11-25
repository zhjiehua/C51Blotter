#include "pageCommon.h"
#include "managerment.h"
#include "CPrintf.h"

#ifdef __cplusplus
extern "C" {
#endif

void pausePageButtonProcess(uint16 control_id, uint8  state)
{
	switch(control_id)
	{
	case PAUSE_PREACTION_BUTTON:
		{
			if(pProjectMan->pCurJumptoAction-1 < &pProjectMan->pCurRunningProject->action[0])
				pProjectMan->pCurJumptoAction = &pProjectMan->pCurRunningProject->action[ACTION_COUNT_PER_PROJECT-1];
			else
				pProjectMan->pCurJumptoAction -= 1;

			SetTextValue(PAUSEPAGE_INDEX, PAUSE_ACTIONNAME_EDIT, pProjectMan->pCurJumptoAction->name);
		}
		break;
	case PAUSE_POSTACTION_BUTTON:
		{
			if(pProjectMan->pCurJumptoAction+1 > &pProjectMan->pCurRunningProject->action[ACTION_COUNT_PER_PROJECT-1])
				pProjectMan->pCurJumptoAction = &pProjectMan->pCurRunningProject->action[0];
			else
				pProjectMan->pCurJumptoAction += 1;

			SetTextValue(PAUSEPAGE_INDEX, PAUSE_ACTIONNAME_EDIT, pProjectMan->pCurJumptoAction->name);
		}
		break;
	case PAUSE_JUMPTO_BUTTON:
		pProjectMan->pCurRunningAction = pProjectMan->pCurJumptoAction;
		pProjectMan->proStatus = PROJECTSTATUS_IMBIBING;
		SetScreen(RUNNINGPAGE_INDEX);
		cDebug("========pausePage JUMPTO program!\n");
		break;
	case PAUSE_ROTATE_BUTTON:
		cDebug("========pausePage ROTATE program!\n");
		break;
	case PAUSE_RESUME_BUTTON:
		pProjectMan->proStatus = pProjectMan->preProStatus;
		SetScreen(RUNNINGPAGE_INDEX);
		cDebug("========pausePage RESUME program!\n");
		break;
		default:
			cDebug("pausePage BUTTON error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
