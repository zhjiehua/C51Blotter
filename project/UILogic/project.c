#ifdef __cplusplus	  
extern "C" {
#endif

#include "project.h"
#include "managerment.h"
#include "CPrintf.h"
#include "stdio.h"

/************************************************************************/
/* У׼ҳ�������б��ַ���                                             */
/************************************************************************/
const char caliPumpMenuText[8][3] = {
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
};

/************************************************************************/
/* �����༭ҳ�������б��ַ���                                             */
/************************************************************************/
const char actionPumpMenuText[8][3] = {
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
};

const char actionTipsMenuText[3][9] = {
	"None",
	"Sample",
	"Membrane",
};

const char actionVoiceMenuText[4][7] = {
	"None",
	"Short",
	"Middle",
	"Long",
};

const char actionSpeedMenuText[3][7] = {
	"Slow",
	"Middle",
	"Fast",
};

/************************************************************************/
/* ������ʼ��Demo                                                       */
/************************************************************************/
const Action_TypeDef actionDemo = {
	PUMP1,
	SAMPLE_TIPS,
	SHORT_VOICE,
	1.0,
	0,
	SLOW_SPEED,
	{0, 1},
	1,
	"action0"
};

//����12����Ŀ
Project_TypeDef project[PROJECT_COUNT];

//�����䶯��У׼�����б�
float caliPumpPara[PUMP_COUNT];

//��ʼ����Ŀ�ṹ��
void initProjectStruct(Project_TypeDef* pro, char *name)
{
	uint16 i;
	char str[NAME_SIZE];

#if 0
	memset(pro->name, '\0', sizeof(pro->name));
	strcpy(pro->name, name);

	memset(pro->action[0].name, '\0', sizeof(pro->action[0].name));
	strcpy(pro->action[0].name, "action0");
	//pro->action[0].name = "action0";
	pro->action[0].pump = PUMP1;
	pro->action[0].tips = NO_TIPS;
	pro->action[0].voice = SHORT_VOICE;
	pro->action[0].addAmount = 10;
	pro->action[0].imbiAmout = 0;
	pro->action[0].shakeSpeed = FAST_SPEED;
	pro->action[0].shakeTime.hour = 0;
	pro->action[0].shakeTime.minute = 10;
	pro->action[0].loopTime = 1;

	memset(pro->action[1].name, '\0', sizeof(pro->action[1].name));
	strcpy(pro->action[1].name, "action1");
	//pro->action[1].name = "action1";
	pro->action[1].pump = PUMP2;
	pro->action[1].tips = NO_TIPS;
	pro->action[1].voice = SHORT_VOICE;
	pro->action[1].addAmount = 10;
	pro->action[1].imbiAmout = 0;
	pro->action[1].shakeSpeed = FAST_SPEED;
	pro->action[1].shakeTime.hour = 0;
	pro->action[1].shakeTime.minute = 10;
	pro->action[1].loopTime = 1;

	memset(pro->action[2].name, '\0', sizeof(pro->action[2].name));
	strcpy(pro->action[2].name, "action2");
	//pro->action[2].name = "action2";
	pro->action[2].pump = PUMP3;
	pro->action[2].tips = NO_TIPS;
	pro->action[2].voice = SHORT_VOICE;
	pro->action[2].addAmount = 10;
	pro->action[2].imbiAmout = 0;
	pro->action[2].shakeSpeed = FAST_SPEED;
	pro->action[2].shakeTime.hour = 0;
	pro->action[2].shakeTime.minute = 10;
	pro->action[2].loopTime = 1;
#else

	memset(pro->name, '\0', NAME_SIZE);
	memcpy(pro->name, name, NAME_SIZE);

	for(i=0;i<ACTION_COUNT_PER_PROJECT;i++)
	{
		memset(str, '\0', NAME_SIZE);
		memset(pro->action[i].name, '\0', NAME_SIZE);
		sprintf(str, "action%d", i);

		memcpy((void *)(&(pro->action[i])), (void *)(&actionDemo), sizeof(actionDemo));

		memcpy(pro->action[i].name, str, NAME_SIZE);
	}
#endif
}

//��ʼ��У׼����
void initCaliPumpPara(float para)
{
	uint16 i;
	for(i=0;i<PUMP_COUNT;i++)
		caliPumpPara[i] = para;
}

//��ʾ1ҳ����ʾ����
void tips1Show(TipsSource_TypeDef tipsSource, char *str)
{
	SetScreen(TIPS1PAGE_INDEX);
	SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, str);
	pProjectMan->tipsSource = tipsSource;
}

//��ע�ܵ�
void fillTube(void)
{
	uint16 i = 0;

	//��λ1�Ų۵��ֶ���
	cDebug("locate TANK1 to MANUAL POINT!\n", i);

	//��ʾҳ��1����ʾ��������Һ�۵�1�Ų۰塿
	tips1Show(TIPSSOURCE_PUTWASTETANK, "Please put the WASTE TANK to TANK1");

	//SetScreen(TIPS1PAGE_INDEX);
	//SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, "Please put the WASTE TANK to TANK1");
	//pProjectMan->tipsSource = TIPSSOURCE_PUTWASTETANK;

	for(i=0;i<PUMP_COUNT;i++)
	{
		if(pProjectMan->pumpSelPumpSel&(PUMP1_MASK<<i))
		{
			//��λ��i�ű�
			cDebug("locate to PUMP%d!\n", i);

			//������ע����
			cDebug("start to fill the tube%d!\n", i);
		}
	}

	//��λ1�Ų۵���Һ��
	cDebug("locate TANK1 to WASTE POINT\n");
	
	//������Һ����
	cDebug("start the WASTE program\n");
	
	//��ʾ���߷�Һ��
	tips1Show(TIPSSOURCE_GETWASTETANK, "Please remove the WASTE TANK");
}

//ִ�ж���
void execAction(Action_TypeDef act)
{
	 ;
}


#ifdef __cplusplus
}
#endif
