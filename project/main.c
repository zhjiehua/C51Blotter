#include "./Peripheral/common.h"
#include "./Peripheral/uart.h"
#include "./Peripheral/io.h"
#include "./Peripheral/sensor.h"
#include "./Peripheral/dcmotor.h"
#include "./Peripheral/stepmotor.h"

#include "./HMI/cmd_queue.h"
#include "./HMI/hmi_driver.h"
#include "./HMI/cmd_process.h"

#include "./UILogic/managerment.h"

#include "CPrintf.h"
#include "stdio.h"

//��ʼ����
void startupTask(void) _task_ 0  
{
	//---------------��ʼ��-------------------------------
	IO_Init();
	Uart_Init();//���ڳ�ʼ��

	initUI();

	Sensor_Init();
	DCMotor_Init();
	StepMotor_Init();

	WDT_CONTR = 0x3F;//�����Ź������ʱ��1.1377s

    EA = 1;//����ȫ���ж�

	//Uart2_SendString("Uart2_SendString");
	//cDebug("hello 51 MCU\n");
	Uart_SendData(0xA5);

	//---------------��������-----------------------------
	os_create_task(1); 	//��������1
	os_create_task(2);	//��������2

	//---------------ɾ��������-----------------------------
 	os_delete_task(0);	//ɾ���Լ�(task0),ʹtask0�˳���������
}  

//UI����
void UITask(void) _task_ 1  
{  
	while(1)
	{
		int16 size; //һ��Ҫint���ͣ����char���ͣ�������ڴ�������⣬�ڵ���sprintfʱ����֣�size = 1ʱ��ת���ַ���ʱΪ256��
		uint16 i;

		//ι��
		WDT_CONTR = 0x3F;

#if 1		
		//����UI����
		size = queue_find_cmd(cmd_buffer, CMD_MAX_SIZE); //�ӻ�������ȡһ��ָ��
		if(size > 0)//���յ�ָ��
		{
			//cDebug("cmd_buffer size = %d", size);
			//for(i=0;i<size;i++)
			//	cDebug("%#x", cmd_buffer[i]);

			ProcessMessage((PCTRL_MSG)cmd_buffer, size);//����ָ��
		}

		tipsPageProcess();
#endif

//		P40 = 0;              
//		os_wait(K_TMO, 100, 0);  //�ȴ�100��ʱ�ӵδ�(ticks),��1s
//					 //�����ļ�ConfTny.A51��INT_CLOCK EQU 10000; default is 10000 cycles
//					 //��˼��ʱ�ӵδ�Ϊ10000���������ڡ���10000*1uS=10ms
//		P40 = 1;
//		os_wait(K_TMO, 100, 0);
    }  
}
 
//��������
void workTask(void) _task_ 2  
{  
	while(1)
	{	
		LED2 = 1;              
		os_wait(K_TMO, 200, 0);  //�ȴ�100��ʱ�ӵδ�(ticks),��1s
					 //�����ļ�ConfTny.A51��INT_CLOCK EQU 10000; default i s 10000 cycles
					 //��˼��ʱ�ӵδ�Ϊ10000���������ڡ���10000*1uS=10ms			 
		LED2 = 0;
		os_wait(K_TMO, 200, 0);

		//���������������
		//StepMotor_Test();

		//ֱ�������������
		//DCMotor_Test();
    }  
}  
