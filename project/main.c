#include "./Peripheral/common.h"
#include "./Peripheral/uart.h"
#include "./Peripheral/io.h"
#include "./Peripheral/sensor.h"
#include "./Peripheral/dcmotor.h"
#include "./Peripheral/stepmotor.h"
#include "./Peripheral/24cxx.h"

#include "./HMI/cmd_queue.h"
#include "./HMI/hmi_driver.h"
#include "./HMI/cmd_process.h"

#include "./UILogic/managerment.h"

#include "CPrintf.h"
#include "stdio.h"

//起始任务
void startupTask(void) _task_ TASK_STARTUP  
{
	uint8_t i;
	char n[10];

	i = sizeof(Action_TypeDef);
	i = sizeof(n);

	//---------------初始化-------------------------------
	IO_Init();
	Uart_Init();//串口初始化
	AT24CXX_Init(); //EEPROM初始化

	initUI();

	Sensor_Init();
	DCMotor_Init();
	StepMotor_Init();

	//从EEPROM取数据

	WDT_CONTR = 0x3F;//开看门狗，溢出时间1.1377s

    EA = 1;//开启全局中断

	//Uart2_SendString("Uart2_SendString");
	//cDebug("hello 51 MCU\n");
	Uart_SendData(0xA5);

	DRV_DELAY = 1;

	//---------------创建任务-----------------------------
	os_create_task(TASK_UI); 	//创建任务1
	//os_create_task(TASK_HOME);	//创建任务2   TASK_HOME

	//---------------删除该任务-----------------------------
 	os_delete_task(TASK_STARTUP);	//删除自己(task0),使task0退出任务链表
}  

//UI任务
void UITask(void) _task_ TASK_UI  
{  
	while(1)
	{
		int16 size; //一定要int类型，如果char类型，会出现内存对齐问题，在调用sprintf时会出现（size = 1时，转成字符串时为256）
		uint16 i;

		//喂狗
		WDT_CONTR = 0x3F;
	
		//处理UI数据
		size = queue_find_cmd(cmd_buffer, CMD_MAX_SIZE); //从缓冲区获取一条指令
		if(size > 0)//接收到指令
		{
			//cDebug("cmd_buffer size = %d", size);
			//for(i=0;i<size;i++)
			//	cDebug("%#x", cmd_buffer[i]);

			ProcessMessage((PCTRL_MSG)cmd_buffer, size);//处理指令
		}

//		P40 = 0;              
//		os_wait(K_TMO, 100, 0);  //等待100个时钟滴答(ticks),即1s
//					 //配置文件ConfTny.A51中INT_CLOCK EQU 10000; default is 10000 cycles
//					 //意思是时钟滴答为10000个机器周期。即10000*1uS=10ms
//		P40 = 1;
//		os_wait(K_TMO, 100, 0);
    }  
}
 
////工作任务
//void teskTask(void) _task_ TASK_TEST  
//{  
//	while(1)
//	{	
//		LED2 = 1;              
//		os_wait(K_TMO, 200, 0);  //等待100个时钟滴答(ticks),即1s
//					 //配置文件ConfTny.A51中INT_CLOCK EQU 10000; default i s 10000 cycles
//					 //意思是时钟滴答为10000个机器周期。即10000*1uS=10ms			 
//		LED2 = 0;
//		os_wait(K_TMO, 200, 0);
//
//		//步进电机驱动测试
//		StepMotor_Test();
//
//		//直流电机驱动测试
//		DCMotor_Test();
//
//		//loopForever();
//    }  
//}  

//工程任务
void projectTask(void) _task_ TASK_PROJECT//TASK_PROJECT
{
	projectProgram();
	os_delete_task(TASK_PROJECT);	//删除自己	
}

//清洗任务
void purgeTask(void) _task_ TASK_PURGE
{
	purgeProgram();
	os_delete_task(TASK_PURGE);	//删除自己	
}

//回原点任务
void homeTask(void) _task_ TASK_HOME   //TASK_HOME
{
	homeProgram();
	os_delete_task(TASK_HOME);	//删除自己
}

//校准任务
void calibraTask(void) _task_ TASK_CALIBRA
{
	//cDebug("createTask : calibraProgram\n");
	calibraProgram();
	os_delete_task(TASK_CALIBRA);	//删除自己
}

void createTask(uint8_t task)
{
	//cDebug("createTask %d\n", (uint16_t)task);
	os_create_task(task);
}