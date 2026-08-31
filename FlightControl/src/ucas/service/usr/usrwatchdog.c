/*
 * usrwatchdog.c
 *
 *  Created on: 2023年12月12日
 *      Author: liyuan
 */

#include "usrwatchdog.h"
#include "xgpiops.h"
#include "xscugic.h"
#include "xscutimer.h"
#include "xtime_l.h"
#include "sleep.h"
#include <stdio.h>


XGpioPs wdgGpio; // GPIO 设备的驱动程序实例

XScuTimer TimerInstance;	/* Cortex A9 Scu Private Timer Instance */
XScuGic IntcInstance;		/* Interrupt Controller Instance */



// 定时器中断句柄
void TimerIntrHandler(void *CallBackRef)
{

	static u8 status = 0;
	XGpioPs_WritePin(&wdgGpio, MIOFEEDDOG, status); //向指定引脚写入数据： 0 或 1
	status = ~status & 0x1;

//	printf("status = %d\r\n", status);

	XScuTimer_ClearInterruptStatus(&TimerInstance);
//	XScuTimer_DisableAutoReload(&TimerInstance);
//	TimerDisableIntr(&IntcInstance, TIMER_IRPT_INTR);

}


int WatchDogIOInit()
{
	int Status;
	XGpioPs_Config *ConfigPtr;



	ConfigPtr = XGpioPs_LookupConfig(WDGDEVICE);
	Status = XGpioPs_CfgInitialize(&wdgGpio, ConfigPtr,
				ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS){
		return XST_FAILURE;
	}
//设置指定引脚的方向： 0 输入， 1 输出
	XGpioPs_SetDirectionPin(&wdgGpio, MIOFEEDDOG, 1);

//使能指定引脚输出： 0 禁止输出使能， 1 使能输出
	XGpioPs_SetOutputEnablePin(&wdgGpio, MIOFEEDDOG, 1);


	return 0;
}



// 定时器设备初始化
int TimerDevInit()
{
	int Status;
	XScuTimer_Config *ConfigPtr;
	int TimerDeviceId = 0;

	/*
	 * Initialize the Scu Private Timer driver.
	 */
	ConfigPtr = XScuTimer_LookupConfig(TimerDeviceId);

	/*
	 * This is where the virtual address would be used, this example
	 * uses physical address.
	 */
	Status = XScuTimer_CfgInitialize(&TimerInstance, ConfigPtr,
					ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XScuTimer_SelfTest(&TimerInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return 0;
}



// 中断控制器设备 初始化
int INTC_DevInit()
{
	int Status;
	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&IntcInstance, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	Xil_ExceptionInit();

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				(Xil_ExceptionHandler)XScuGic_InterruptHandler,
				&IntcInstance);
	Xil_ExceptionEnable();

	return 0;
}


// 定时器中断初始化
int TimerIntrInit()
{
	int Status;
	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(&IntcInstance, TIMER_IRPT_INTR,
				(Xil_ExceptionHandler)TimerIntrHandler,
				(void *)&TimerInstance);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable the interrupt for the device.
	 */
	XScuGic_Enable(&IntcInstance, TIMER_IRPT_INTR);

	/*
	 * Enable the timer interrupts for timer mode.
	 */
	XScuTimer_EnableInterrupt(&TimerInstance);

	//配置中断优先级和中断方式
	XScuGic_SetPriorityTriggerType(&IntcInstance, TIMER_IRPT_INTR, 0x18, 0x1);

	/*
	 * Enable Auto reload mode.
	 */
	XScuTimer_EnableAutoReload(&TimerInstance);

	/*
	 * Load the timer counter register.
	 */
	XScuTimer_LoadTimer(&TimerInstance, TIMER_LOAD_VALUE);

	/*
	 * Start the timer counter and then wait for it
	 * to timeout a number of times.
	 */
	XScuTimer_Start(&TimerInstance);


	return 0;
}

/*****************************************************************************/
/**
*
* This function disables the interrupts that occur for the device.
*
* @param	IntcInstancePtr is the pointer to the instance of XScuGic
*		driver.
* @param	TimerIntrId is the Interrupt Id for the device.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void TimerDisableIntr(XScuGic *IntcInstancePtr, u16 TimerIntrId)
{
	/*
	 * Disconnect and disable the interrupt for the Timer.
	 */
	XScuGic_Disconnect(IntcInstancePtr, TimerIntrId);
}

int WatchDogInit()
{
	WatchDogIOInit();

	INTC_DevInit();

	TimerDevInit();

	TimerIntrInit();

//	static u8 status = 0;
//	while(1){
//		XGpioPs_WritePin(&wdgGpio, MIOFEEDDOG, status); //向指定引脚写入数据： 0 或 1
//		status = ~status & 0x1;
//		sleep(1);
//		printf("1\r\n");
//	}

	return 0;
}
