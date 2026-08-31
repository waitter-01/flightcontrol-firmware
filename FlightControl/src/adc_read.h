/*
 * adc_read.h
 *
 *  Created on: 2024年7月8日
 *      Author: wanghaobo
 */

#ifndef SRC_ADC_READ_H_
#define SRC_ADC_READ_H_

#include "main.h"

#define adc_read_TASK_PRIO 1 //任务优先级
#define adc_read_STK_SIZE 128 //任务堆栈大小
TaskHandle_t adc_readTask_Handler; //任务句柄
void adc_read_task(void *pvParameters);//任务函数

void init_adc(void);
#endif /* SRC_ADC_READ_H_ */
