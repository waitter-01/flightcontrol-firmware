/*
 * WP40Motor.h
 *
 *  Created on: 2024年6月24日
 *      Author: wanghaobo
 */

#ifndef SRC_WP40MOTOR_H_
#define SRC_WP40MOTOR_H_
#include "main.h"

#define Wp40_num 4

//BASE0 = 0x0100 +(ECU通信地址 - 1)*0x0050
//ECU通信地址可设置范围1-20，默认初始值为1。
#define can_dir_Return_id_base 0x00000100
#define can_dir_Control_id_base 0x00000120
//#define can_id_addr 1
//#define can_id (can_dir_id_base+((can_id_addr - 1)*0x0050))

#define Can_Wp40 			0

#define Wp40_monitor_id 		0
#define Wp40_state_id 		    1
#define Wp40_Fuel_id 		    2
#define Wp40_info_id 		    3
#define Wp40_upstate_id 		4
#define Wp40_sysinfo_id 		5
#define Wp40_check_id 		    6
#define Wp40_power_id 		    8
#define Wp40_back_id 		    11
#define Wp40_parameter_id 	    12


#define can_cmd_TASK_PRIO SysPriority_SafeTask //任务优先级
#define can_cmd_STK_SIZE 4096 //任务堆栈大小
TaskHandle_t can_cmdTask_Handler; //任务句柄
void WP40_Return_data(void *pvParameters);//任务函数

void wp40_Init(void);
void ControlWp40Out(u8 ch,u16 speed);
void Wp40EmergencyStopAll(void);
void Wp40StopAll(void);
void Wp40StartProcessAll(void);
#endif /* SRC_WP40MOTOR_H_ */
