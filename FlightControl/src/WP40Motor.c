/*
 * WP40Motor.c
 *
 *  Created on: 2024年6月24日
 *      Author: wanghaobo
 */

#include "WP40Motor.h"
#include "drvcanip.h"
#include "usrcan.h"
#include <stdbool.h>
#include "Basic.h"
#include "semphr.h"

typedef struct __monitor
{
	u16 Set_speed;
	u16 True_speed;
	s16 Exhaust_gas_temperature;
	u8 status;
	u8 pump_voltage;

}motor_ecu_monitor,*pmotor_ecu_monitor;

typedef struct __state
{
	u8 battery_voltage;
	u8 Total_current;
	u8 Reserve[2];
	u8 device_status;
	u8 Reserve_byte;
	u8 Warning_status;
	u8 error_code;

}motor_ecu_state,*pmotor_ecu_state;

typedef struct __Fuel
{
	u16 Fuel_flow;
	u16 Fuel_consumption;
	s8 PCB_temperature;
	u8 Env_pressure;
	u8 Env_temperature;

}motor_ecu_Fuel,*pmotor_ecu_Fuel;

typedef struct __info
{
	u16 successful_Number;
	u8 end_Number;
	u8 Warning_Number;
	u8 error_Number;
	u8 Total_run_time[3];

}motor_ecu_info,*pmotor_ecu_info;

typedef struct __upstate
{
	u16 Last_run_time;
	u8 Reserve[5];
	u8 Last_error_code;

}motor_ecu_upstate,*pmotor_ecu_upstate;

typedef struct __sysinfo
{
	u16 ECU_number;
	u8 Code_version[2];
	u16 motor_number;
	u8 control_mod;
	u8 control_model;

}motor_ecu_sysinfo,*pmotor_ecu_sysinfo;

typedef struct __check
{
	u8 Starter_motor;
	u8 Main_oil_valve;
	u8 Starting_oil_valve;
	u8 Speed_sensor;
	u8 Pump;
	u8 Reserve[3];

}motor_ecu_check,*pmotor_ecu_check;

typedef struct __power
{
	u8 Reserve[6];
	u16 motor_thrust;

}motor_ecu_power,*pmotor_ecu_power;

typedef struct __back
{
	u8 Reserve[2];
	u8 Command_speed[2];
	u8 flag;
	u8 airspeed;
	s8 Temperature;
	u8 Air_pressure;

}motor_ecu_back,*pmotor_ecu_back;

typedef struct __parameter
{
	u8 Maximum_speed;
	u8 Minimum_speed;
	u8 Warning_temp;
	u8 Warning_speed;
	u8 acceleration_time;
	u8 Cooling_speed;
	u8 Control_interval;
	u8 Operation_Mode;

}motor_ecu_parameter,*pmotor_ecu_parameter;

typedef struct __motor
{
	u32 motor_ecu_Return_ID;
	u32 motor_ecu_Control_ID;
	u32 Control_time;
	u32 Control_MAXcycle;
	u16 Control_speed;
	motor_ecu_monitor g_motor_ecu_monitor;
	motor_ecu_state g_motor_ecu_state;
	motor_ecu_Fuel g_motor_ecu_Fuel;
	motor_ecu_info g_motor_ecu_info;
	motor_ecu_upstate g_motor_ecu_upstate;
	motor_ecu_sysinfo g_motor_ecu_sysinfo;
	motor_ecu_check g_motor_ecu_check;
	motor_ecu_power g_motor_ecu_power;
	motor_ecu_back g_motor_ecu_back;
	motor_ecu_parameter g_motor_ecu_parameter;

}motor_info,*pmotor_info;

motor_info g_motor_info[Wp40_num];

static SemaphoreHandle_t StartProcessBinarySemaphore;


void WP40_Return_data(void *pvParameters)
{
	u32 id;
	u8 data_len;
	u8 data[8];
	int Status;
	u8 can_ch;
	TickType_t xLastWakeTime;//准确周期延时

	while( getInitializationCompleted() == false )
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	pmotor_info p_info = (motor_info*)pvParameters;
	xLastWakeTime = xTaskGetTickCount();
	while(true)
	{
		if(!Get_Can_Read_Flag(Can_Wp40) )
		{
			Status = Can_Read(Can_Wp40,&id,&data_len,data);
			if(Status == XST_SUCCESS)
			{
				if(id == p_info[0].motor_ecu_Return_ID)
				{
					can_ch = 0;
					id -= p_info[can_ch].motor_ecu_Return_ID;
				}
				else if(id == p_info[1].motor_ecu_Return_ID)
				{
					can_ch = 1;
					id -= p_info[can_ch].motor_ecu_Return_ID;
				}
				else if(id == p_info[2].motor_ecu_Return_ID)
				{
					can_ch = 2;
					id -= p_info[can_ch].motor_ecu_Return_ID;
				}
				else if(id == p_info[3].motor_ecu_Return_ID)
				{
					can_ch = 3;
					id -= p_info[can_ch].motor_ecu_Return_ID;
				}
				else
				{
					continue;
				}
				portTICK_TYPE_ENTER_CRITICAL();
				switch(id)
				{
					case Wp40_monitor_id:
						memcpy(&p_info[can_ch].g_motor_ecu_monitor,data,8);
						break;
					case Wp40_state_id:
						memcpy(&p_info[can_ch].g_motor_ecu_state,data,8);
						break;
					case Wp40_Fuel_id:
						memcpy(&p_info[can_ch].g_motor_ecu_Fuel,data,8);
						break;
					case Wp40_info_id:
						memcpy(&p_info[can_ch].g_motor_ecu_info,data,8);
						break;
					case Wp40_upstate_id:
						memcpy(&p_info[can_ch].g_motor_ecu_upstate,data,8);
						break;
					case Wp40_sysinfo_id:
						memcpy(&p_info[can_ch].g_motor_ecu_sysinfo,data,8);
						break;
					case Wp40_check_id:
						memcpy(&p_info[can_ch].g_motor_ecu_info,data,8);
						break;
					case Wp40_power_id:
						memcpy(&p_info[can_ch].g_motor_ecu_power,data,8);
						break;
					case Wp40_back_id:
						memcpy(&p_info[can_ch].g_motor_ecu_back,data,8);
						break;
					case Wp40_parameter_id:
						memcpy(&p_info[can_ch].g_motor_ecu_parameter,data,8);
						break;
					default:
						break;
				}
				portTICK_TYPE_EXIT_CRITICAL();
			}
		}
		vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(5) );
	}
}




void SetWp40CanId(u8 ch,u8 id,u32 time,motor_info *g_info)
{
	pmotor_info p_info = g_info;

	p_info[ch].Control_MAXcycle = time;
	p_info[ch].motor_ecu_Return_ID = (can_dir_Return_id_base+((id - 1)*0x0050));
	p_info[ch].motor_ecu_Control_ID = (can_dir_Return_id_base+((id - 1)*0x0050));
}

void SetWp40start(u8 ch_id)
{
	u8 data[8];
	u8 len = 0;

	data[len++] = 1;

	Can_Write(Can_Wp40,ch_id,data,len);
}

void SetWp40stop(u8 ch_id)
{
	u8 data[8];
	u8 len = 0;

	data[len++] = 0;

	Can_Write(Can_Wp40,ch_id,data,len);
}

void SetWp40EmergencyStop(u8 ch_id)
{
	u8 data[8];
	u8 len = 0;

	data[len++] = 0;
	data[len++] = 15;

	Can_Write(Can_Wp40,ch_id,data,len);
}

void SetWp40Rotatingspeed(u8 ch_id,u16 speed)
{
	u8 data[8];
	u8 len = 0;

	data[len++] = (u8)(speed>>8);
	data[len++] = (u8)speed;

	Can_Write(Can_Wp40,ch_id+2,data,len);
}

void SetWp40test(u8 ch,u8 OilPump,u8 MainOil,u8 startOil,u8 ElectricIgnition,u8 startmotor)
{
	u8 data[8];
	u8 len = 0;

	data[len++] = OilPump;
	data[len++] = 0;
	data[len++] = MainOil;
	data[len++] = startOil;
	data[len++] = ElectricIgnition;
	data[len++] = startmotor;

	while(!Target_is_time(g_motor_info[ch].Control_time,g_motor_info[ch].Control_MAXcycle))
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	g_motor_info[ch].Control_time = xTaskGetTickCount();
	Can_Write(Can_Wp40,g_motor_info[ch].motor_ecu_Control_ID+4,data,len);
}

void SetWp40SelfTest(u8 ch)
{
	u8 data[8];
	u8 len = 0;

	data[len++] = 1;
	while(!Target_is_time(g_motor_info[ch].Control_time,g_motor_info[ch].Control_MAXcycle))
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}
	g_motor_info[ch].Control_time = xTaskGetTickCount();
	Can_Write(Can_Wp40,g_motor_info[ch].motor_ecu_Control_ID+6,data,len);
}

void SetWp40simulation(u8 ch)
{
	u8 data[8];
	u8 len = 0;

	data[len++] = 1;

	while(!Target_is_time(g_motor_info[ch].Control_time,g_motor_info[ch].Control_MAXcycle))
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}
	g_motor_info[ch].Control_time = xTaskGetTickCount();
	Can_Write(Can_Wp40,g_motor_info[ch].motor_ecu_Control_ID+9,data,len);
}


void ControlWp40Out(u8 ch,u16 speed)
{
	if(ch > Wp40_num)
		return;

	while(!Target_is_time(g_motor_info[ch].Control_time,g_motor_info[ch].Control_MAXcycle))
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	g_motor_info[ch].Control_speed = speed;
	g_motor_info[ch].Control_time = xTaskGetTickCount();
	SetWp40Rotatingspeed(g_motor_info[ch].motor_ecu_Control_ID,g_motor_info[ch].Control_speed);
}

void WP40_StartProcess(void *pvParameters)
{
	TickType_t xLastWakeTime;//准确周期延时

	u8 channel = *(u8*)pvParameters;

	while( getInitializationCompleted() == false )
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}
	if((g_motor_info[channel].g_motor_ecu_monitor.status & 0x0f) == 0)
	{
		SetWp40start(g_motor_info[channel].motor_ecu_Control_ID);

		xLastWakeTime = xTaskGetTickCount();
		while(((g_motor_info[channel].g_motor_ecu_monitor.status & 0x0f) != 1) || (!Target_is_time(xLastWakeTime,25)))
		{
			vTaskDelay(pdMS_TO_TICKS(1));
		}

		if((g_motor_info[channel].g_motor_ecu_monitor.status & 0x0f) == 1)
		{
			SetWp40start(g_motor_info[channel].motor_ecu_Control_ID);
			while(((g_motor_info[channel].g_motor_ecu_monitor.status & 0x0f) != 3) && (!Target_is_time(xLastWakeTime,10000)))
			{
				vTaskDelay(pdMS_TO_TICKS(1));
			}
		}
	}

	xSemaphoreGive(StartProcessBinarySemaphore);

	vTaskDelete(NULL);
}


bool Wp40StartProcess(u8 ch)
{
	BaseType_t err = pdFALSE;

	StartProcessBinarySemaphore = xSemaphoreCreateBinary();

	xTaskCreate((TaskFunction_t )WP40_StartProcess,
				(const char* )"StartProcess",
				(uint16_t )1024,
				(void* )&ch,
				(UBaseType_t )SysPriority_SafeTask,
				(TaskHandle_t* )NULL);

	err = xSemaphoreTake(StartProcessBinarySemaphore,portMAX_DELAY);
	if(err == pdTRUE)
	{
		if((g_motor_info[ch].g_motor_ecu_monitor.status&0x0f) == 3)
		{
			return true;
		}
	}

	return false;
}

void Wp40StartProcessAll(void)
{
	for(u8 id=0; id<Wp40_num; id++)
		Wp40StartProcess(id);
}

void Wp40StopAll(void)
{
	for(u8 id=0;id<Wp40_num;id++)
	{
		SetWp40stop(g_motor_info[id].motor_ecu_Control_ID);
	}
}

void Wp40EmergencyStopAll(void)
{
	for(u8 id=0;id<Wp40_num;id++)
	{
		SetWp40EmergencyStop(g_motor_info[id].motor_ecu_Control_ID);
	}
}

void wp40_Init(void)
{
	SetWp40CanId(0,1,100,g_motor_info);
	SetWp40CanId(1,2,100,g_motor_info);
	SetWp40CanId(2,3,100,g_motor_info);
	SetWp40CanId(3,4,100,g_motor_info);

	xTaskCreate((TaskFunction_t )WP40_Return_data,
				(const char* 	)"wp40_Motor",
				(uint16_t 		)can_cmd_STK_SIZE,
				(void* 			)g_motor_info,
				(UBaseType_t 	)can_cmd_TASK_PRIO,
				(TaskHandle_t* 	)&can_cmdTask_Handler);
}
