/*
 * adc_read.c
 *
 *  Created on: 2024Äê7ÔÂ8ÈÕ
 *      Author: wanghaobo
 */
#include "adc_read.h"
#include "voltsensor.h"
#include "tempsensor.h"
#include "usrtemp.h"
#include "interface.h"
#include <stdbool.h>
#include "Basic.h"

typedef struct __sys_state
{
	float Coretemp;
	float PDTemp[2];
	float FKTemp[2];
	float adc_Voltage[16];
	float Voltage_28V;
	float Current_28V;
	uint32_t Pwm_frequency[8];
	float Pwm_Duty[8];

	bool Ext28VIn[3];
	bool ExtNoSrcIn[3];
	bool NoScrOut[3];

	bool pd_28VStatus[6];
	uint32_t pd_28Venable[6];

	uint32_t SX_enable[15];
	bool SX_Status[15];
}sys_state,*pSys_state;

sys_state g_sys_state;

void set_sysioState(int State)
{
	g_sys_state.Ext28VIn[0] = (State>>5) & 0x01;
	g_sys_state.Ext28VIn[1] = (State>>4) & 0x01;
	g_sys_state.Ext28VIn[2] = (State>>3) & 0x01;

	g_sys_state.ExtNoSrcIn[0] = (State>>2) & 0x01;
	g_sys_state.ExtNoSrcIn[1] = (State>>1) & 0x01;
	g_sys_state.ExtNoSrcIn[2] = State & 0x01;
}

void adc_read_task(void *pvParameters)
{
	float pbyData[2];

	while( getInitializationCompleted() == false )
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	while(true)
	{
		g_sys_state.Coretemp = System_GetCoreTemperature();
		memcpy(&g_sys_state.PDTemp,GetPDTemp(),8);
		memcpy(&g_sys_state.FKTemp,GetFKTemp(),8);
		ADC_GetVoltage(g_sys_state.adc_Voltage);
		AnalogVolCur(pbyData);
		g_sys_state.Voltage_28V = pbyData[0];
		g_sys_state.Current_28V = pbyData[1];

		set_sysioState(Get_IOState(GPIO_CHANNEL1));

		//SetPD_Enable(PDChannel, g_Sys_State.pd_enable[i]);
		//SetPD_Status(PDChannel, PDState);

		//GetPD_Status();

		//SetSX_Enable(SXChannel, 1);
		//SetSX_Status(SXChannel, SXState);

		//GetSX_Status();

		vTaskDelay(500);
	}
}

void init_adc(void)
{
	GpioChannel_Init();
	VoltCurrSensorInit();
	Tmp75_Init();
	temp_get_init();

	xTaskCreate((TaskFunction_t )adc_read_task,
				(const char* )"adc_read_task",
				(uint16_t )adc_read_STK_SIZE,
				(void* )NULL,
				(UBaseType_t )adc_read_TASK_PRIO,
				(TaskHandle_t* )&adc_readTask_Handler);

}
