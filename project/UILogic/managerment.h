#ifndef _MANAGERMENT_H_
#define _MANAGERMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pageCommon.h"
#include "project.h"
#include "../HMI/cmd_queue.h"
#include "../HMI/cmd_process.h"
#include "../HMI/hmi_user_uart.h"

typedef enum
{
	TIPSSOURCE_NONE,

	//tips2页面
	TIPSSOURCE_FILLTIPS,
	TIPSSOURCE_RUNNINGPAUSE,
	TIPSSOURCE_RUNNINGSTOP,

	//tips1页面
	TIPSSOURCE_PUTWASTETANK,
	TIPSSOURCE_GETWASTETANK,
	TIPSSOURCE_PUTTANK,
	TIPSSOURCE_GETTANK,
	TIPSSOURCE_SAMPLE,
	TIPSSOURCE_MEMBRANCE,
}TipsSource_TypeDef;

typedef enum
{
	TIPS_NONE,
	TIPS_CANCEL,
	TIPS_OK,
}TipsButton_TypeDef;

typedef enum
{
	RUNNING_NONE,
	RUNNING_PROJECT,
	RUNNING_BACKFLOW,
	RUNNING_PURGE,
	RUNNING_CALIBRATION,
}RunningType_TypeDef;

typedef enum
{
	PROJECTSTATUS_IDLE,
	PROJECTSTATUS_SELECTPUMP,
	PROJECTSTATUS_FILLING,
	PROJECTSTATUS_PLACEPLATE,
	PROJECTSTATUS_IMBIBING,
	PROJECTSTATUS_TIPS,
	PROJECTSTATUS_ADDING,
	PROJECTSTATUS_INCUBATION,
	PROJECTSTATUS_TIMEOUT,

	//PROJECTSTATUS_PAUSETIPS,
	PROJECTSTATUS_WAITING,
	PROJECTSTATUS_PAUSE,
}ProjectStatus_TypeDef;

/************************************************************************/
/* 项目管理结构体                                                       */
/************************************************************************/
typedef struct
{
	Project_TypeDef *pCurRunningProject;  //运行页面当前正在运行的项目
	Action_TypeDef *pCurRunningAction;  //运行页面当前正在运行的动作
	uint8 curTank;  //运行页面当前动作的槽
	uint8 curLoopTime;  //运行页面当前第几次循环
	uint8 RTCTimeout;  //运行页面RTC定时时间到标志，0:未到；>0:定时器溢出

	Project_TypeDef *pCurProject;  //项目页面当前选择的项目
	uint8 startTank;  //项目页面起始槽
	uint8 endTank;  //项目页面终止槽

	Project_TypeDef *pCurEditProject;  //项目编辑页面当前编辑的项目
	Action_TypeDef *pCurEditAction;  //项目编辑页面当前编辑的动作

	Action_TypeDef *pCurJumptoAction;  //暂停页面的跳转选择动作

	uint8 backflowPumpSel;  //回流页面的泵选择按钮状态
	uint8 purgePumpSel;  //清洗页面的泵选择按钮状态
	uint8 pumpSelPumpSel;  //泵选择页面的泵选择按钮状态

	uint8 caliPumpSel;  //校准页面的泵选择按钮状态
	float *pCaliPumpPara;  //实际的校准泵参数列表
	float caliAmount;  //校准页面实际加注量的输入临时参数

	//动作编辑页面的输入临时参数
	Pump_TypeDef pump;//泵编号
	Tips_TypeDef tips;//提示
	Voice_TypeDef voice;//声音
	float addAmount;//加注量
	uint8 imbiAmout;//吸液量
	ShakeSpeed_TypeDef shakeSpeed;//摇动速度
	ShakeTime_TypeDef shakeTime;//摇动时间
	uint8 loopTime;//循环次数

	TipsSource_TypeDef tipsSource;  //跳出提示页面的源头
	TipsButton_TypeDef tipsButton;  //提示页面的按钮状态
	RunningType_TypeDef runningType;  //正在运行的类型

	ProjectStatus_TypeDef proStatus;  //项目的动作流程
	ProjectStatus_TypeDef preProStatus;  //项目的动作流程
}ProjectMan_TypeDef;

/************************************************************************/
/*                                                                      */
/************************************************************************/

extern ProjectMan_TypeDef *pProjectMan;
extern uint8 cmd_buffer[CMD_MAX_SIZE];

void initProjectMan(ProjectMan_TypeDef *pm);
void init(void);

//void uartInterrupt(uint8 data);
//void loopForever(void);

void tipsPageProcess(void);

#ifdef __cplusplus
}
#endif

#endif
