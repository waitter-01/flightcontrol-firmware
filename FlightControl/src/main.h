/*
 * main.h
 *
 *  Created on: 2024年5月8日
 *      Author: wanghaobo
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_cache.h"
#include "xil_assert.h"

#include <stdio.h>

//操作系统优先级定义
#define SysPriority_MeasurementSystem_Integral 6
#define SysPriority_MeasurementSystem 5
#define SysPriority_ControlSystem 4
#define SysPriority_ExtSensor 3
#define SysPriority_SafeTask 2
#define SysPriority_UserTask 1

extern XScuGic xInterruptController;

#define START_TASK_PRIO 1 //任务优先级
#define START_STK_SIZE 256 //任务堆栈大小
TaskHandle_t StartTask_Handler; //任务句柄
void start_task(void *pvParameters); //任务函数

void Xil_Assert_Callback(const char8 *File, s32 Line);

#endif /* SRC_MAIN_H_ */
