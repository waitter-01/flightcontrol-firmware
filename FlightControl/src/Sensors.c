/*
 * Sensors.c
 *
 *  Created on: 2024年6月21日
 *      Author: wanghaobo
 */
#include "Sensors.h"
#include "Basic.h"
#include "main.h"
#include "usrpdsxcontrol.h"

static SemaphoreHandle_t SensorsMessagesMutex;
static QueueHandle_t Sensors_Queue;
static EventGroupHandle_t Sensors_Event;

GF404_Sensor g_GF404_Sensor;

static void Sensors_Server(void* pvParameters)
{
	TickType_t xLastWakeTime;//准确周期延时
	uint8_t uartpl_buff[4096];
	uint16_t uartpl_len;
	uint8_t Sensor_buff[160];
	uart_State CurState = HEAD_H;  //当前状态
	uint32_t readlen = 0;
	uint32_t datalen = 0;

	while( getInitializationCompleted() == false )
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	xLastWakeTime = xTaskGetTickCount();
	while(true)
	{
		if((UartPl_Get_Recvflag(SensorsUart) & 0x02) == 0x02)
		{
			uartpl_len = UartPl_RecvBuff(SensorsUart,uartpl_buff);
			for(int i=0;i<uartpl_len;i++)
			{
				switch(CurState)
				{
					case HEAD_H:
						if(uartpl_buff[i] == 0x55)
						{
							CurState = HEAD_L;
							readlen = 0;
							Sensor_buff[readlen++] = uartpl_buff[i];
						}
						break;
					case HEAD_L:
						if(uartpl_buff[i] == 0xAA)
						{
							CurState = LEN;
							Sensor_buff[readlen++] = uartpl_buff[i];
						}
						else
						{
							CurState = HEAD_H;
						}
						break;
					case LEN:
						datalen = uartpl_buff[i]-2;
						if(datalen > 158)
						{
							CurState = HEAD_H;
						}
						else
						{
							CurState = ID;
							Sensor_buff[readlen++] = uartpl_buff[i];
						}
						break;
					case ID:
						if(uartpl_buff[i] == 0xA1)
						{
							CurState = DATA;
							Sensor_buff[readlen++] = uartpl_buff[i];
						}
						else
						{
							CurState = HEAD_H;
						}
						break;
					case DATA:
						if(readlen == datalen)
						{
							CurState = CHECK;
							Sensor_buff[readlen++] = uartpl_buff[i];
						}
						else
						{
							Sensor_buff[readlen++] = uartpl_buff[i];
						}
						break;
					case CHECK:
						if( uartpl_buff[i] == xor_checksum(Sensor_buff,readlen) )
						{
							Sensor_buff[readlen++] = uartpl_buff[i];
							memcpy( &g_GF404_Sensor,Sensor_buff + 4,readlen-8 );
							xEventGroupWaitBits(Sensors_Event,0x01,pdTRUE,pdTRUE,portMAX_DELAY);

							taskENTER_CRITICAL(); //进入临界区
							xQueueOverwrite(Sensors_Queue,&g_GF404_Sensor);
							set_sensors_Event(true);
							taskEXIT_CRITICAL(); //退出临界区

							CurState = HEAD_H;
						}
						else
						{
							CurState = HEAD_H;
						}

						break;

					default:
						break;
				}
			}
		}

		vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(5) );
	}
}

void set_sensors_Event(bool event)
{
	if(event)
		xEventGroupSetBits(Sensors_Event,0x01);
	else
		xEventGroupClearBits(Sensors_Event,0x01);
}

bool get_sensors_Event(uint32_t TIMEOUT)
{
	EventBits_t Event_value;
	Event_value = xEventGroupWaitBits(Sensors_Event,0x01,pdTRUE,pdTRUE,pdMS_TO_TICKS(TIMEOUT));
	//Event_value = xEventGroupGetBits(Sensors_Event);

	if(Event_value == 0x01)
		return true;
	else
		return false;
}

BaseType_t get_sensors_data(GF404_Sensor *Sensor,uint32_t TIMEOUT)
{
	return xQueuePeek(Sensors_Queue,Sensor,pdMS_TO_TICKS(TIMEOUT) );
}

void init_drv_Sensors(void)
{
	SensorsMessagesMutex = xSemaphoreCreateMutex();
	Sensors_Queue = xQueueCreate(1,sizeof(GF404_Sensor) );
	Sensors_Event = xEventGroupCreate();
	set_sensors_Event(true);
	UART_PL_Setting(SensorsUart, 460800, 'n');//惯组

	SetPD_Enable(0,1);
	SetPD_Status(0,1);
	vTaskDelay(pdMS_TO_TICKS(1000) );

	xTaskCreate((TaskFunction_t )Sensors_Server,
				(const char* 	)"Sensors_task",
				(uint16_t 		)Sensors_STK_SIZE,
				(void* 			)NULL,
				(UBaseType_t 	)SysPriority_MeasurementSystem,
				(TaskHandle_t* 	)&SensorsTask_Handler);
}
