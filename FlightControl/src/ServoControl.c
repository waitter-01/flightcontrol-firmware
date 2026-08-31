/*
 * ServoControl.c
 *
 *  Created on: 2024年7月8日
 *      Author: wanghaobo
 */
#include "ServoControl.h"
#include "Basic.h"

typedef struct __Servo_Control
{
	u32 Servo_num_back;
	short Servo_Control_Back[4];
	short Servo_Back[4];

	u32 Servo_num;
	short Servo_Control[4];

}Servo_Control,*pServo_Control;

Servo_Control g_Servo_Control;
QueueHandle_t Servo_Control_Queue;

void ServoControl(void *pvParameters)
{
	short Servo_Control[4];
	u8 Servo_Control_buff[32];
	u8 Servo_len = 0;
	BaseType_t err = pdFALSE;
	u16 add = 0;
	TickType_t xLastWakeTime;//准确周期延时
	u8 uartpl_buff[4096];
	u8 Sensor_buff[160];
	u16 uartpl_len;
	uart_State CurState = HEAD_H;  //当前状态
	int datalen = 0;
	int readlen = 0;
	bool read_HL = true;
	u16 addcheck = 0;
	u16 check = 0;

	while( getInitializationCompleted() == false )
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	while(true)
	{
		xLastWakeTime = xTaskGetTickCount();
		err = xQueueReceive(Servo_Control_Queue,Servo_Control,0);
		if(err == pdTRUE)
		{
			Servo_Control_buff[Servo_len++] = 0xAA;
			Servo_Control_buff[Servo_len++] = 0x55;
			Servo_Control_buff[Servo_len++] = 0x08;
			Servo_Control_buff[Servo_len++] = 0x00;

			Servo_Control_buff[Servo_len++] = (u8)g_Servo_Control.Servo_num;
			Servo_Control_buff[Servo_len++] = (u8)g_Servo_Control.Servo_num>>8;
			Servo_Control_buff[Servo_len++] = (u8)g_Servo_Control.Servo_num>>16;
			Servo_Control_buff[Servo_len++] = (u8)g_Servo_Control.Servo_num>>24;

			memcpy(Servo_Control_buff + Servo_len,Servo_Control,sizeof(short)*4);
			Servo_len += sizeof(short)*4;

			for(u8 i=2;i<Servo_len;i++)
			{
				add += Servo_Control_buff[i];
			}

			Servo_Control_buff[Servo_len++] = add;
			Servo_Control_buff[Servo_len++] = add>>8;

			UartPl_SendBuff(ServoUart,Servo_Control_buff,Servo_len);

			g_Servo_Control.Servo_num++;
		}

		if((UartPl_Get_Recvflag(ServoUart) & 0x02) == 0x02)
		{
			uartpl_len = UartPl_RecvBuff(ServoUart,uartpl_buff);

			for(int i=0;i<uartpl_len;i++)
			{
				switch(CurState)
				{
					case HEAD_H:
						if(uartpl_buff[i] == 0xAA)
						{
							CurState = HEAD_L;
							readlen = 0;
							addcheck = 0;
						}
						break;
					case HEAD_L:
						if(uartpl_buff[i] == 0x5F)
						{
							CurState = LEN;
						}
						else
						{
							CurState = HEAD_H;
						}
						break;
					case LEN:
						if(read_HL)
						{
							datalen = uartpl_buff[i];
							addcheck += uartpl_buff[i];
							read_HL = false;
						}
						else
						{
							datalen |= uartpl_buff[i]<<8;
							addcheck += uartpl_buff[i];
							if(datalen > 16)
							{
								CurState = HEAD_H;
							}
							else
							{
								CurState = DATA;
							}
							read_HL = true;
						}
						break;
					case DATA:
						if(readlen == (datalen+3))
						{
							CurState = CHECK;
							Sensor_buff[readlen++] = uartpl_buff[i];
							addcheck += uartpl_buff[i];
						}
						else
						{
							Sensor_buff[readlen++] = uartpl_buff[i];
							addcheck += uartpl_buff[i];
						}
						break;
					case CHECK:
						if(read_HL)
						{
							check = uartpl_buff[i];
							read_HL = false;
						}
						else
						{
							check |= uartpl_buff[i]<<8;
							if(check == addcheck)
							{
								portTICK_TYPE_ENTER_CRITICAL();
								memcpy(&g_Servo_Control,Sensor_buff,readlen);
								portTICK_TYPE_EXIT_CRITICAL();
								CurState = HEAD_H;
							}
							else
							{
								CurState = HEAD_H;
							}
							read_HL = true;
						}
						break;
					default:
						break;
				}
			}
		}
		Servo_len = 0;
		add = 0;

		vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(5) );
		//PWMControl(PWMChannel, fDuty, state);
		//PWMAllStop();
		//vTaskDelay(5);
	}
}


void Servo_init(void)
{
	UART_PL_Setting(ServoUart, 460800, 'n');//舵机

	Servo_Control_Queue = xQueueCreate(10,sizeof(short)*4);

	SetPD_Enable(2,1);
	SetPD_Status(2,1);
	vTaskDelay(pdMS_TO_TICKS(1000));

	xTaskCreate((TaskFunction_t )ServoControl,
				(const char* )"Servo_Control",
				(uint16_t )Servo_STK_SIZE,
				(void* )NULL,
				(UBaseType_t )Servo_TASK_PRIO,
				(TaskHandle_t* )&ServoTask_Handler);


}
