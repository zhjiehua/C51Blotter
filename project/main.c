#include "./Peripheral/common.h"
#include "./Peripheral/uart.h"
#include "./Peripheral/io.h"
#include "./HMI/cmd_queue.h"
#include "./HMI/hmi_driver.h"
#include "./HMI/cmd_process.h"
#include "./UILogic/managerment.h"
#include "CPrintf.h"
#include "stdio.h"

//-----------------------------------------------

#if 0
/* main program */
void main()
{	
	//---------------��ʼ��------------------------------------
	//IO_Init();
	Uart_Init();//���ڳ�ʼ��
//	Timer0_Init(); //��ʱ����ʼ��

//	Timer_Create(0, 200, 1);

	//WDT_CONTR = 0x3C;//�����Ź������ʱ��1.1377s

    //EA = 1;//����ȫ���ж�

//   	if(IapReadByte(IAP_ADDRESS) != 0xA5)
//	{
//		IapEraseSector(IAP_ADDRESS);    //Erase current sector
//		IapProgramByte(IAP_ADDRESS, 0xA5);
//		Uart_SendString("eeprom erase!\r\n");
//	}

	//Uart_SendString("hello");

	P4M1 = 0x71;    //˫
	P4M0 = 0x1F;
	P4 = 0xF1;    //1
	
	P5M1 = 0x20;    //˫  ˫  ˫      ˫ ˫     ˫  ˫  ˫  ˫
	P5M0 = 0x23;
	P5 = 0xFC;    //---,---,A_DIR,A_RST,---,---,---,---

	EA = 1;//����ȫ���ж�

	Uart_SendString("hello"); 

    while (1)                       //loop
	{
		//uint8_t i;

		//ι��
	//	WDT_CONTR = 0x3C;

		//������������
		//Uart_DataParse();

		//AUX_LED = ~AUX_LED;
//		Timer_Start(0);
//		while(!Timer_IsTimeOut(0));
//		Timer_Stop(0);

	}
}
#else

//��ʼ����
void startupTask(void) _task_ 0  
{
	//---------------��ʼ��-------------------------------
	IO_Init();
	Uart_Init();//���ڳ�ʼ��

	WDT_CONTR = 0x3F;//�����Ź������ʱ��1.1377s

    EA = 1;//����ȫ���ж�

//	Uart2_SendString("Uart2_SendString");
	cDebug("hello 51 MCU\n");

	init();//��ʼ������

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
		
		//����UI����
		size = queue_find_cmd(cmd_buffer, CMD_MAX_SIZE); //�ӻ�������ȡһ��ָ��
		if(size > 0)//���յ�ָ��
		{
			//cDebug("cmd_buffer size = %d", size);
			//for(i=0;i<size;i++)
			//	cDebug("%#x", cmd_buffer[i]);

			//P40 = 0; 
			ProcessMessage((PCTRL_MSG)cmd_buffer, size);//����ָ��
		}

		tipsPageProcess();

		//P40 = 0;              
		os_wait(K_TMO, 100, 0);  //�ȴ�100��ʱ�ӵδ�(ticks),��1s
					 //�����ļ�ConfTny.A51��INT_CLOCK EQU 10000; default is 10000 cycles
					 //��˼��ʱ�ӵδ�Ϊ10000���������ڡ���10000*1uS=10ms
		//P40 = 1;
		//os_wait(K_TMO, 100, 0);
		
    }  
}
 
//��������
void workTask(void) _task_ 2  
{  
	while(1)
	{	
		P55 = 0;              
		os_wait(K_TMO, 50, 0);  //�ȴ�100��ʱ�ӵδ�(ticks),��1s
					 //�����ļ�ConfTny.A51��INT_CLOCK EQU 10000; default i s 10000 cycles
					 //��˼��ʱ�ӵδ�Ϊ10000���������ڡ���10000*1uS=10ms 
		P55 = 1;
		os_wait(K_TMO, 50, 0);
    }  
}  

#endif
