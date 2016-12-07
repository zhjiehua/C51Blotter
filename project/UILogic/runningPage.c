#include "pageCommon.h"
#include "managerment.h"
#include "CPrintf.h"

#ifdef __cplusplus
extern "C" {
#endif

void runningPageButtonProcess(uint16 control_id, uint8  state)
{
	switch(control_id)
	{
	case RUNNING_PAUSE_BUTTON:
		SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, PAUSE_TIPS);
		SetScreen(TIPS2PAGE_INDEX);
		//pProjectMan->exception = EXCEPTION_PAUSE;
		pProjectMan->exceptionButtonFlag = EXCEPTION_PAUSE;
		break;
	case RUNNING_STOP_BUTTON:
		SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, STOP_TIPS);
		SetScreen(TIPS2PAGE_INDEX);
		//pProjectMan->exception = EXCEPTION_STOP;
		pProjectMan->exceptionButtonFlag = EXCEPTION_STOP;
		break;
		default:
			cDebug("runningPage BUTTON error!\n");
		break;
	}
}

void runningPageRTCTimeoutProcess(uint16 control_id)
{
	pProjectMan->RTCTimeout = 1;
	cDebug("========runningPage RTC timeout!\n");
}

#ifdef __cplusplus
}
#endif
