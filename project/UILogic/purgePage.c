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
		//��ʾ��Ҫ��ϴ�Ĺܵ���������ˮ��
		SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ2ҳ��
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the tube that to be washed in the distilled water");
		while(pProjectMan->tipsButton == TIPS_NONE);
		pProjectMan->tipsButton = TIPS_NONE;
		SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

		//��ԭ��
		pStepMotor->Home();

		//��ʾ����Һ�����ڲ۰��1��λ��
		SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ2ҳ��
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please place the Waste Tank to the plate2");
		while(pProjectMan->tipsButton == TIPS_NONE);
		pProjectMan->tipsButton = TIPS_NONE;
		SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��
		
		//��ϴҪ��ϴ�Ĺܵ�
		for(i=0;i<8;i++)
		{
			if(pProjectMan->purgePumpSel & (0x01<<i))
			{
				//����Һ��ת����Ӧ�ı�λ��
				relCoord = pStepMotor->Abs2Rel(AbsCoordinate[i]);
				pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //AbsCoordinate[8]Ϊ3�Ųۣ�����Һ�ۣ�

				pDCMotor->SetSpeed(i, 10);
				pDCMotor->SetCMD(i, ENABLE);
				
				os_wait(K_TMO, 6000, 0);
				os_wait(K_TMO, 6000, 0);
				os_wait(K_TMO, 6000, 0);
	
				pDCMotor->SetCMD(i, DISABLE);
			}
		}
	}
	
	//����Һ��ת����Ӧ�ı�λ��
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[8]); //��Һ��λ��
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //����Һ��ת����Һ��

	//������ձ�

	//���·�Һ��

	//��ʱһС��ʱ��

	//�����Һ��

	//����Һ��ת���ն���
	relCoord = pStepMotor->Abs2Rel(AbsCoordinate[9]); //�ֶ���λ��
	pStepMotor->RelativePosition(relCoord, AbsCoordinate[9]); //����Һ��ת���ն���

	//��ʾȡ�߷�Һ��
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ2ҳ��
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please take away the Waste Tank");
	while(pProjectMan->tipsButton == TIPS_NONE);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	//��ʾ��ϴ���
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ2ҳ��
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Wash finish");
	while(pProjectMan->tipsButton == TIPS_NONE);
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	SetButtonValue(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0x00); //���ð���Ϊ����״̬
	pProjectMan->runningType = RUNNING_NONE;  //����ִ�����Ҫ�����־	
}

#ifdef __cplusplus
}
#endif
