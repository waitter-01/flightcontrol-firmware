/*
 * ServoControl.h
 *
 *  Created on: 2024年7月8日
 *      Author: wanghaobo
 */

#ifndef SRC_SERVOCONTROL_H_
#define SRC_SERVOCONTROL_H_

#include "main.h"


#define Servo_TASK_PRIO 1 //任务优先级
#define Servo_STK_SIZE 4096 //任务堆栈大小
TaskHandle_t ServoTask_Handler; //任务句柄
void ServoControl(void *pvParameters);//任务函数

extern QueueHandle_t Servo_Control_Queue;

void Servo_init(void);

#endif /* SRC_SERVOCONTROL_H_ */
