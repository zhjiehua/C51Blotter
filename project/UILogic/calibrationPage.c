#include "pageCommon.h"
#include "managerment.h"
#include "CPrintf.h"

#ifdef __cplusplus
extern "C" {
#endif

void caliPageButtonProcess(uint16 control_id, uint8  state)
{
	switch(control_id)
	{
		case CALI_START_BUTTON:
		{
			cDebug("========caliPage start to run the PUMP program!\n");
		}
		break;
		case CALI_OK_BUTTON:
		{
			//保存校准参数
			pProjectMan->pCaliPumpPara[pProjectMan->pumpSelPumpSel] = pProjectMan->caliAmount;
			cDebug("========caliPage Save the calibration data!\n");
		}
		break;
		case CALI_PUMPSELECT_BUTTON:
		break;
		case CALI_BACK_BUTTON:
		break;
		default:
			cDebug("caliPage BUTTON error!\n");
		break;
	}
}

void caliPageEditProcess(uint16 control_id, uint8 *str)
{
	switch(control_id)
	{
		case CALI_ACTUALAMOUNT_EDIT:
			pProjectMan->caliAmount = StringToFloat(str);
		break;

		default:
			cDebug("caliPage EDIT error!\n");
		break;
	}
}

void caliPageMenuProcess(uint16 control_id, uint8 item)
{
	switch(control_id)
	{
	case CALI_PUMPSELECT_MENU:
		{
			pProjectMan->caliPumpSel = item;
			SetTextValueFloat(CALIBRATIONPAGE_INDEX, CALI_ACTUALAMOUNT_EDIT, pProjectMan->pCaliPumpPara[pProjectMan->caliPumpSel]);
		}
		break;
	default:
		cDebug("caliPage MENU error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
