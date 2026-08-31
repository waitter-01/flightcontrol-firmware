/*
 * flight_control.c
 *
 *  Created on: 2024年5月8日
 *      Author: wanghaobo
 */
#include "flight_control.h"
#include <stdbool.h>
#include "Sensors.h"
#include "Basic.h"

void flight_control_task(void *pvParameters)
{
	GF404_Sensor g_Sensor;
	TickType_t xLastWakeTime;//准确周期延时

	while( getInitializationCompleted() == false )
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	xLastWakeTime = xTaskGetTickCount();
	while(true)
	{
		if(get_sensors_data(&g_Sensor,portMAX_DELAY))
		{

		}

		vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(1) );
	}
}


void init_ControlSystem(void)
{

	//飞控算法
	xTaskCreate((TaskFunction_t )flight_control_task, //任务函数
				(const char* )"flight_control", //任务名称
				(uint16_t )flight_control_STK_SIZE, //任务堆栈大小
				(void* )NULL, //传递给任务函数的参数
				(UBaseType_t )flight_control_TASK_PRIO, //任务优先级
				(TaskHandle_t* )&flight_controlTask_Handler); //任务句柄
}
