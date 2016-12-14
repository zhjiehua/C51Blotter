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

//��ʼ����
void startupTask(void) _task_ TASK_STARTUP  
{
	uint8_t i;
	char n[10];

	i = sizeof(Action_TypeDef);
	i = sizeof(n);

	//---------------��ʼ��-------------------------------
	IO_Init();
	Uart_Init();//���ڳ�ʼ��
	AT24CXX_Init(); //EEPROM��ʼ��

	initUI();

	Sensor_Init();
	DCMotor_Init();
	StepMotor_Init();

	//��EEPROMȡ����

	WDT_CONTR = 0x3F;//�����Ź������ʱ��1.1377s

    EA = 1;//����ȫ���ж�

	//Uart2_SendString("Uart2_SendString");
	//cDebug("hello 51 MCU\n");
	Uart_SendData(0xA5);

	DRV_DELAY = 1;

	//---------------��������-----------------------------
	os_create_task(TASK_UI); 	//��������1
	//os_create_task(TASK_HOME);	//��������2   TASK_HOME

	//---------------ɾ��������-----------------------------
 	os_delete_task(TASK_STARTUP);	//ɾ���Լ�(task0),ʹtask0�˳���������
}  

//UI����
void UITask(void) _task_ TASK_UI  
{  
	while(1)
	{
		int16 size; //һ��Ҫint���ͣ����char���ͣ�������ڴ�������⣬�ڵ���sprintfʱ����֣�size = 1ʱ��ת���ַ���ʱΪ256��
		uint16 i;

		//ι��
		WDT_CONTR = 0x3F;
	
		//����UI����
		size = queue_find_cmd(cmd_buffer, CMD_MAX_SIZE); //�ӻ�������ȡһ��ָ��
		if(size > 0)//���յ�ָ��
		{
			//cDebug("cmd_buffer size = %d", size);
			//for(i=0;i<size;i++)
			//	cDebug("%#x", cmd_buffer[i]);

			ProcessMessage((PCTRL_MSG)cmd_buffer, size);//����ָ��
		}

//		P40 = 0;              
//		os_wait(K_TMO, 100, 0);  //�ȴ�100��ʱ�ӵδ�(ticks),��1s
//					 //�����ļ�ConfTny.A51��INT_CLOCK EQU 10000; default is 10000 cycles
//					 //��˼��ʱ�ӵδ�Ϊ10000���������ڡ���10000*1uS=10ms
//		P40 = 1;
//		os_wait(K_TMO, 100, 0);
    }  
}
 
////��������
//void teskTask(void) _task_ TASK_TEST  
//{  
//	while(1)
//	{	
//		LED2 = 1;              
//		os_wait(K_TMO, 200, 0);  //�ȴ�100��ʱ�ӵδ�(ticks),��1s
//					 //�����ļ�ConfTny.A51��INT_CLOCK EQU 10000; default i s 10000 cycles
//					 //��˼��ʱ�ӵδ�Ϊ10000���������ڡ���10000*1uS=10ms			 
//		LED2 = 0;
//		os_wait(K_TMO, 200, 0);
//
//		//���������������
//		StepMotor_Test();
//
//		//ֱ�������������
//		DCMotor_Test();
//
//		//loopForever();
//    }  
//}  

//��������
void projectTask(void) _task_ TASK_PROJECT//TASK_PROJECT
{
	projectProgram();
	os_delete_task(TASK_PROJECT);	//ɾ���Լ�	
}

//��ϴ����
void purgeTask(void) _task_ TASK_PURGE
{
	purgeProgram();
	os_delete_task(TASK_PURGE);	//ɾ���Լ�	
}

//��ԭ������
void homeTask(void) _task_ TASK_HOME   //TASK_HOME
{
	homeProgram();
	os_delete_task(TASK_HOME);	//ɾ���Լ�
}

//У׼����
void calibraTask(void) _task_ TASK_CALIBRA
{
	//cDebug("createTask : calibraProgram\n");
	calibraProgram();
	os_delete_task(TASK_CALIBRA);	//ɾ���Լ�
}

void createTask(uint8_t task)
{
	//cDebug("createTask %d\n", (uint16_t)task);
	os_create_task(task);
}