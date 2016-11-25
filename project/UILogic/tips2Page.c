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
		pProjectMan->tipsButton = TIPS_OK;
		break;
	case TIPS2_CANCEL_BUTTON:
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
