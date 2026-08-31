/*
 * usrwatchdog.h
 *
 *  Created on: 2023年12月12日
 *      Author: liyuan
 */

#ifndef SRC_UCAS_SERVICE_USR_USRWATCHDOG_H_
#define SRC_UCAS_SERVICE_USR_USRWATCHDOG_H_

#include "xparameters.h"
#include "xscugic.h"
#include "xscutimer.h"



#define MIOFEEDDOG  	    9				//watchdog IO
#define WDGDEVICE     	 XPAR_XGPIOPS_0_DEVICE_ID


#define TIMER_DEVICE_ID		XPAR_XSCUTIMER_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR		XPAR_SCUTIMER_INTR


//5ms定时计数:0x18CBA8  计算公式(ms)：TIMER_LOAD_VALUE*1000/(650000000/2)
#define TIMER_LOAD_VALUE	0x18CBA8 * 100

void TimerIntrHandler(void *CallBackRef);
int WatchDogIOInit();
int TimerDevInit();

int INTC_DevInit();
int TimerIntrInit();
void TimerDisableIntr(XScuGic *IntcInstancePtr, u16 TimerIntrId);

int WatchDogInit();

#endif /* SRC_UCAS_SERVICE_USR_USRWATCHDOG_H_ */
