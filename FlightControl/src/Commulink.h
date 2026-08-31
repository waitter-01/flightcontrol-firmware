/*
 * Commulink.h
 *
 *  Created on: 2024年6月17日
 *      Author: wanghaobo
 */

#ifndef SRC_COMMULINK_H_
#define SRC_COMMULINK_H_
#include "main.h"

#define Mavlink_Sys_ID 1
#define Mavlink_heartbeat_time 1000
#define Mavlink_ATTITUDE_time 100
#define Mavlink_global_position_time 200
#define Mavlink_GPS_RAW_time 200
#define Mavlink_SYS_STATUS_time 500
#define Mavlink_LOCAL_POSITION_NED 100

#define Commulink_TASK_PRIO SysPriority_UserTask //任务优先级
#define Commulink_STK_SIZE 4096 //任务堆栈大小
TaskHandle_t CommulinkTask_Handler; //任务句柄

void init_Commulink(void);
#endif /* SRC_COMMULINK_H_ */
