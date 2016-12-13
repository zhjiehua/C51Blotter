#include "pageCommon.h"
#include "managerment.h"
#include "CPrintf.h"
#include "../Peripheral/24cxx.h"
#include "../Peripheral/dcmotor.h"
#include "../Peripheral/common.h"

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

			createTask(TASK_CALIBRA);
		}
		break;
		case CALI_OK_BUTTON:
		{
			//保存校准参数
			uint16_t addrOffset;
			pProjectMan->pCaliPumpPara[pProjectMan->caliPumpSel] = pProjectMan->caliAmount;
			addrOffset = pProjectMan->caliPumpSel*sizeof(float);
			AT24CXX_Write(CALIBPARA_BASEADDR+addrOffset, (uint8_t*)&pProjectMan->caliAmount, sizeof(float));  //保存参数
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
			cDebug("pProjectMan->caliAmount = %f\n", pProjectMan->caliAmount);
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
