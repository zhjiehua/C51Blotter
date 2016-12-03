#include "pageCommon.h"
#include "managerment.h"
#include "CPrintf.h"
#include "../Peripheral/dcmotor.h"
#include "../Peripheral/stepmotor.h"

#ifdef __cplusplus
extern "C" {
#endif

void purgePageButtonProcess(uint16 control_id, uint8  state)
{
	switch(control_id)
	{
		case PURGE_PUMP1_BUTTON:
		{
			state ? (pProjectMan->purgePumpSel |= PUMP1_MASK) : (pProjectMan->purgePumpSel &= ~PUMP1_MASK);
		}
		break;
		case PURGE_PUMP2_BUTTON:
			{
				state ? (pProjectMan->purgePumpSel |= PUMP2_MASK) : (pProjectMan->purgePumpSel &= ~PUMP2_MASK);
			}
			break;
		case PURGE_PUMP3_BUTTON:
			{
				state ? (pProjectMan->purgePumpSel |= PUMP3_MASK) : (pProjectMan->purgePumpSel &= ~PUMP3_MASK);
			}
			break;
		case PURGE_PUMP4_BUTTON:
			{
				state ? (pProjectMan->purgePumpSel |= PUMP4_MASK) : (pProjectMan->purgePumpSel &= ~PUMP4_MASK);
			}
			break;
		case PURGE_PUMP5_BUTTON:
			{
				state ? (pProjectMan->purgePumpSel |= PUMP5_MASK) : (pProjectMan->purgePumpSel &= ~PUMP5_MASK);
			}
			break;
		case PURGE_PUMP6_BUTTON:
			{
				state ? (pProjectMan->purgePumpSel |= PUMP6_MASK) : (pProjectMan->purgePumpSel &= ~PUMP6_MASK);
			}
			break;
		case PURGE_PUMP7_BUTTON:
			{
				state ? (pProjectMan->purgePumpSel |= PUMP7_MASK) : (pProjectMan->purgePumpSel &= ~PUMP7_MASK);
			}
			break;
		case PURGE_PUMP8_BUTTON:
			{
				state ? (pProjectMan->purgePumpSel |= PUMP8_MASK) : (pProjectMan->purgePumpSel &= ~PUMP8_MASK);
			}
			break;
		case PURGE_START_BUTTON:
			{
				cDebug("========purgePage start to run the PURGE program!\n");
				pProjectMan->runningType = RUNNING_PURGE;
			}
			break;
		case PURGE_BACK_BUTTON:
			break;
		default:
			cDebug("purgePage BUTTON error!\n");
		break;
	}
}

void purgeProgram(void)
{
	uint8_t i;
	uint8_t relCoord;

	if(pProjectMan->purgePumpSel)
	{
		//提示将要清洗的管道置于蒸馏水中
		SetScreen(TIPS1PAGE_INDEX);//跳转到提示2页面
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the tube that to be washed in the distilled water");
		while(pProjectMan->tipsButton == TIPS_NONE);
		pProjectMan->tipsButton = TIPS_NONE;
		SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

		//回原点
		pStepMotor->Home();

		//提示将废液槽置于槽板号1的位置
		SetScreen(TIPS1PAGE_INDEX);//跳转到提示2页面
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the Waste Tank to the plate2");
		while(pProjectMan->tipsButton == TIPS_NONE);
		pProjectMan->tipsButton = TIPS_NONE;
		SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面
		
		//清洗要清洗的管道
		for(i=0;i<8;i++)
		{
			if(pProjectMan->purgePumpSel & (0x01<<i))
			{
				//将废液槽转到对应的泵位置
				relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
				pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[8]为3号槽（即废液槽）

				pDCMotor->SetSpeed(i, 10);
				pDCMotor->SetCMD(i, ENABLE);
				
				os_wait(K_TMO, 6000, 0);
				os_wait(K_TMO, 6000, 0);
				os_wait(K_TMO, 6000, 0);
	
				pDCMotor->SetCMD(i, DISABLE);
			}
		}
	}
	
	//将废液槽转到对应的泵位置
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[8]); //废液口位置
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //将废液槽转到废液口

	//启动真空泵

	//放下废液口

	//延时一小段时间

	//提起废液口

	//将废液槽转到收动点
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]); //手动点位置
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //将废液槽转到收动点

	//提示取走废液槽
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示2页面
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please take away the Waste Tank");
	while(pProjectMan->tipsButton == TIPS_NONE);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	//提示清洗完成
	SetScreen(TIPS1PAGE_INDEX);//跳转到提示2页面
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Wash finish");
	while(pProjectMan->tipsButton == TIPS_NONE);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//跳转到清洗页面

	SetButtonValue(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0x00); //设置按键为弹起状态
	pProjectMan->runningType = RUNNING_NONE;  //程序执行完成要清除标志	
}

#ifdef __cplusplus
}
#endif
