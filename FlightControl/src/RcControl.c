/*
 * RcControl.c
 *
 *  Created on: 2024年7月25日
 *      Author: wanghaobo
 */

#include "RcControl.h"
#include "Basic.h"
#include "xuartps.h"
#include "usrpdsxcontrol.h"

static XUartPs UartPs0;

void ReceiverUpdate(void *pvParameters)
{
	TickType_t xLastWakeTime;//准确周期延时

	uint8_t rdata;


	while( getInitializationCompleted() == false )
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	XUartPs *uart = (XUartPs *)pvParameters;
	//xLastWakeTime = xTaskGetTickCount();
	while(true)
	{
		if(xSemaphoreTake(UartPsRxMessagesMutex,portMAX_DELAY) == pdTRUE)
		{
			while(XUartPs_IsReceiveData(uart->Config.BaseAddress))
			{
				rdata = XUartPs_RecvByte(uart->Config.BaseAddress);
				Elrs_RC_In(rdata);
			}
		}

		//vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(5) );
	}

}

void Elrs_RC_In(uint8_t rdata)
{
//	for( int a=0; a<uartpl_len; a++ )
//	{
//	    if( mavlink_parse_char(MAVLINK_COMM_1, uartpl_buff[a], &msg, &status ) )
//	    {
//			if( msg.msgid < Mavlink_RC_Process_Count )
//			{
//				if( Mavlink_RC_Process[ msg.msgid ] != 0 )
//					Mavlink_RC_Process[ msg.msgid ]( MAVLINK_COMM_1 , &msg );
//				//printf("Received message with ID %d, sequence: %d from component %d of system %d \r\n", msg.msgid, msg.seq, msg.compid, msg.sysid);
//			}
//	    }
//	}
}


void SBUS_RC_In(u8 rdata)
{
	static uint8_t SBUS_RC_State;
	static uint8_t current_rc_channel;
	static uint8_t rc_bit_count;
	static uint16_t current_rc;
	static float rc_buf[18];
	static bool failsafe;

	if(SBUS_RC_State == 0)
	{
		if(rdata == 0x0f)
		{
			++SBUS_RC_State;
		}
	}
	else if( SBUS_RC_State <= 22 )
	{
		rc_bit_count += 8;
		if(rc_bit_count >= 11)
		{
			rc_bit_count -= 11;
			uint8_t l_byte_count = 8 - rc_bit_count;
			rc_buf[ current_rc_channel++ ] = 0.04885197850512945774303859306302f *
						(float)( current_rc | ( ((uint8_t)( rdata << rc_bit_count ) ) << 3 ) );
			current_rc = (uint8_t)( rdata >> l_byte_count );
		}
		else
		{
			current_rc |= rdata << (rc_bit_count - 8);
		}
		++SBUS_RC_State;
	}
	else if(SBUS_RC_State == 23)
	{
		//判断是否失控
		failsafe = rdata & (1<<4);
		rc_buf[ current_rc_channel++ ] = rdata & (1<<7);
		rc_buf[ current_rc_channel++ ] = rdata & (1<<6);
		++SBUS_RC_State;
	}
	else
	{
		if( rdata == 0 )
		{
			SBUS_RC_State = 0;
			current_rc_channel = 0;
			rc_bit_count = 0;
			current_rc = 0;
		}
	}
}

void XUartPs0_Send(u8 *data,u32 len)
{
	for(int i=0;i<len;i++)
		XUartPs_SendByte(UartPs0.Config.BaseAddress,*(data++));
}



void RcControl_init(void *PsUart)
{
	SetPD_Enable(3,1);
	SetPD_Status(3,1);
	vTaskDelay(pdMS_TO_TICKS(1000));

	UartPs0 = *(XUartPs*)PsUart;

	xTaskCreate((TaskFunction_t )ReceiverUpdate,
				(const char* )"Servo_Control",
				(uint16_t )RcControl_STK_SIZE,
				(void* )PsUart,
				(UBaseType_t )RcControl_TASK_PRIO,
				(TaskHandle_t* )&RcControlTask_Handler);
}


