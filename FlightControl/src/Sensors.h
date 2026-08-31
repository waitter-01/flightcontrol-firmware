/*
 * Sensors.h
 *
 *  Created on: 2024年6月21日
 *      Author: wanghaobo
 */

#ifndef SRC_SENSORS_H_
#define SRC_SENSORS_H_
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include <stdbool.h>

typedef struct
{
	u32 Sensor_time;
	u32 Nav_time;
	u32 Sensor_state;
	u32 Sensor_error;

	int Ins_angle[3];
	int Ins_LonLatAlt[3];
	int Ins_ENU_Speed[3];

	float Gyro[3];
	float Acc[3];

	int Nav_angle[3];
	int Nav_LonLatAlt[3];
	int Nav_ENU_Speed[3];

	u32 PPS_time;
	u32 Satellite_time;
	int Satellite_LonLatAlt[3];
	int Satellite_ENU_X_Y_Speed[2];
	short Satellite_ENU_Z_Speed;

	u8 Positioning_status;
	u8 Satellite_num;

	short Elevation_anomaly;
	u16 GPS_week;
	u16 HDOP;
	u16 VDOP;

}GF404_Sensor,*pGF404_Sensor;

#define Sensors_TASK_PRIO SysPriority_MeasurementSystem //任务优先级
#define Sensors_STK_SIZE 4096 //任务堆栈大小
TaskHandle_t SensorsTask_Handler; //任务句柄

void init_drv_Sensors(void);
void set_sensors_Event(bool event);
bool get_sensors_Event(uint32_t TIMEOUT);
BaseType_t get_sensors_data(GF404_Sensor *Sensor,uint32_t TIMEOUT);
#endif /* SRC_SENSORS_H_ */
