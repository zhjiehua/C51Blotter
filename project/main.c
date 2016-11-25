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
	//---------------初始化------------------------------------
	//IO_Init();
	Uart_Init();//串口初始化
//	Timer0_Init(); //定时器初始化

//	Timer_Create(0, 200, 1);

	//WDT_CONTR = 0x3C;//开看门狗，溢出时间1.1377s

    //EA = 1;//开启全局中断

//   	if(IapReadByte(IAP_ADDRESS) != 0xA5)
//	{
//		IapEraseSector(IAP_ADDRESS);    //Erase current sector
//		IapProgramByte(IAP_ADDRESS, 0xA5);
//		Uart_SendString("eeprom erase!\r\n");
//	}

	//Uart_SendString("hello");

	P4M1 = 0x71;    //双
	P4M0 = 0x1F;
	P4 = 0xF1;    //1
	
	P5M1 = 0x20;    //双  双  双      双 双     双  双  双  双
	P5M0 = 0x23;
	P5 = 0xFC;    //---,---,A_DIR,A_RST,---,---,---,---

	EA = 1;//开启全局中断

	Uart_SendString("hello"); 

    while (1)                       //loop
	{
		//uint8_t i;

		//喂狗
	//	WDT_CONTR = 0x3C;

		//解析串口数据
		//Uart_DataParse();

		//AUX_LED = ~AUX_LED;
//		Timer_Start(0);
//		while(!Timer_IsTimeOut(0));
//		Timer_Stop(0);

	}
}
#else

//起始任务
void startupTask(void) _task_ 0  
{
	//---------------初始化-------------------------------
	IO_Init();
	Uart_Init();//串口初始化

	WDT_CONTR = 0x3F;//开看门狗，溢出时间1.1377s

    EA = 1;//开启全局中断

//	Uart2_SendString("Uart2_SendString");
	cDebug("hello 51 MCU\n");

	init();//初始化所有

	//---------------创建任务-----------------------------
	os_create_task(1); 	//创建任务1
	os_create_task(2);	//创建任务2

	//---------------删除该任务-----------------------------
 	os_delete_task(0);	//删除自己(task0),使task0退出任务链表
}  

//UI任务
void UITask(void) _task_ 1  
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

			//P40 = 0; 
			ProcessMessage((PCTRL_MSG)cmd_buffer, size);//处理指令
		}

		tipsPageProcess();

		//P40 = 0;              
		os_wait(K_TMO, 100, 0);  //等待100个时钟滴答(ticks),即1s
					 //配置文件ConfTny.A51中INT_CLOCK EQU 10000; default is 10000 cycles
					 //意思是时钟滴答为10000个机器周期。即10000*1uS=10ms
		//P40 = 1;
		//os_wait(K_TMO, 100, 0);
		
    }  
}
 
//工作任务
void workTask(void) _task_ 2  
{  
	while(1)
	{	
		P55 = 0;              
		os_wait(K_TMO, 50, 0);  //等待100个时钟滴答(ticks),即1s
					 //配置文件ConfTny.A51中INT_CLOCK EQU 10000; default i s 10000 cycles
					 //意思是时钟滴答为10000个机器周期。即10000*1uS=10ms 
		P55 = 1;
		os_wait(K_TMO, 50, 0);
    }  
}  

#endif
