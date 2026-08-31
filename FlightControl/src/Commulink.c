/*
 * Commulink.c
 *
 *  Created on: 2024年6月17日
 *      Author: wanghaobo
 */
#include "Commulink.h"
#include "Basic.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "mavlink.h"
#include <stdbool.h>
#include "mavlink_SendFuncs.h"
#include "MavlinkCMDProcess.h"
#include "MavlinkRCProcess.h"

typedef struct __mavlinkFun
{
	u16 mavlink_id;
	u32 tagtime;
	u32 time;
}mavlink_Funs;

static SemaphoreHandle_t MavlinkMessagesMutex;

bool SetMsgRate(mavlink_Funs *funs,uint16_t Msg_id, uint32_t Rate)
{
	if(funs == NULL)
		return false;

	for(u16 a=0;a<Mavlink_Send_Funcs_Count;a++)
	{
		if(Mavlink_Send_Funcs[Msg_id] != 0)
		{
			if(funs[a].mavlink_id == 0)
			{
				funs[a].mavlink_id = Msg_id;
				funs->tagtime = Rate;
			}
			return true;
		}
	}
	return false;
}

void MAVLINK_SEND_UART_BYTES(mavlink_channel_t chan,const uint8_t *buf,uint16_t len)
{
    if (chan == MAVLINK_COMM_0)
    {
    	UartPl_SendBuff(CommulinkUart,buf,len);
    }
    else if (chan == MAVLINK_COMM_1)
    {
    	XUartPs0_Send(buf,len);
    }
    else if (chan == MAVLINK_COMM_2)
    {
    	//uart1_transmit(ch);
    }
    else if (chan == MAVLINK_COMM_3)
    {
    	//uart1_transmit(ch);
    }
    else
    {


    }
}

static void Commulink_Server(void* pvParameters)
{
	mavlink_Funs mavlink_SendFlag[Mavlink_Send_Funcs_Count];
	uint8_t mavlink_buf[MAVLINK_MAX_PACKET_LEN];
	uint16_t mavlink_len;

	uint8_t uartpl_buff[4096];
	uint16_t uartpl_len;
	mavlink_status_t status;
	mavlink_message_t msg;//mavlink协议帧结构


	TickType_t xLastWakeTime;//准确周期延时
	SetMsgRate(mavlink_SendFlag,MAVLINK_MSG_ID_HEARTBEAT,Mavlink_heartbeat_time);
	SetMsgRate(mavlink_SendFlag,MAVLINK_MSG_ID_SYS_STATUS,Mavlink_SYS_STATUS_time);
	SetMsgRate(mavlink_SendFlag,MAVLINK_MSG_ID_ATTITUDE,Mavlink_ATTITUDE_time);
	SetMsgRate(mavlink_SendFlag,MAVLINK_MSG_ID_GPS_RAW_INT,Mavlink_GPS_RAW_time);
	SetMsgRate(mavlink_SendFlag,MAVLINK_MSG_ID_GLOBAL_POSITION_INT,Mavlink_global_position_time);
	SetMsgRate(mavlink_SendFlag,MAVLINK_MSG_ID_LOCAL_POSITION_NED,Mavlink_LOCAL_POSITION_NED);

	while( getInitializationCompleted() == false )
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	xLastWakeTime = xTaskGetTickCount();
	while(true)
	{
		for(u16 i=0;i<Mavlink_Send_Funcs_Count;i++)//发送遥测消息
		{
			if( Target_is_time(mavlink_SendFlag[i].time,mavlink_SendFlag[i].tagtime) )
			{
				if(xSemaphoreTake(MavlinkMessagesMutex,pdMS_TO_TICKS(2)) == pdTRUE)
				{
					Mavlink_Send_Funcs[mavlink_SendFlag[i].mavlink_id](MAVLINK_COMM_0,&msg);//根据不同的msg id对msg进行填充
					mavlink_msg_to_send_buffer(mavlink_buf, &msg);//打包mavlink包
					mavlink_len = mavlink_msg_get_send_buffer_length(&msg);
					MAVLINK_SEND_UART_BYTES(MAVLINK_COMM_0,mavlink_buf,mavlink_len);//通过串口发送
					MAVLINK_SEND_UART_BYTES(MAVLINK_COMM_1,mavlink_buf,mavlink_len);
					mavlink_SendFlag[i].time = xTaskGetTickCount();

					xSemaphoreGive(MavlinkMessagesMutex);
				}
			}
		}

		if((UartPl_Get_Recvflag(CommulinkUart) & 0x02) == 0x02)//接收MAVLINK消息并解析
		{
			uartpl_len = UartPl_RecvBuff(CommulinkUart,uartpl_buff);
			for( int a = 0; a < uartpl_len; a++ )
			{
			    if( mavlink_parse_char(MAVLINK_COMM_0, uartpl_buff[a], &msg, &status ) )
			    {
					if( msg.msgid < Mavlink_RC_Process_Count )
					{
						if( Mavlink_RC_Process[ msg.msgid ] != 0 )
							Mavlink_RC_Process[ msg.msgid ]( MAVLINK_COMM_0 , &msg );
						//printf("Received message with ID %d, sequence: %d from component %d of system %d \r\n", msg.msgid, msg.seq, msg.compid, msg.sysid);
					}
			    }
			}
		}

		vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(10) );
	}
}

void init_Commulink(void)
{
	MavlinkMessagesMutex = xSemaphoreCreateMutex();

	UART_PL_Setting(CommulinkUart, 115200, 'n');//数传

	xTaskCreate((TaskFunction_t )Commulink_Server,
				(const char* )"Commulink_task",
				(uint16_t )Commulink_STK_SIZE,
				(void* )NULL,
				(UBaseType_t )SysPriority_UserTask,
				(TaskHandle_t* )&CommulinkTask_Handler);
}
