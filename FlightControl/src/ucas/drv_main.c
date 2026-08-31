/*
 * drv_main.c
 *
 *  Created on: 2024年6月17日
 *      Author: wanghaobo
 */
#include "drv_main.h"
#include "Basic.h"
#include "drvuartpl.h"
#include "qflash.h"
#include "usrcan.h"
#include "usrled.h"
#include "interface.h"
#include "usrpdsxcontrol.h"
#include "Commulink.h"
#include "Sensors.h"
#include "adc_read.h"
#include "WP40Motor.h"
#include "ServoControl.h"

void init_drv_Main(void)
{
	PD_SX_Init();
	init_adc();
	UartPS0Init();
	uartpl_init();
//	UsrCanInit();
//	LedInit();

//	for(int i = 0;i < 8;i++)
//	{
//		PWMConfig(i, g_sys_state.Pwm_frequency[i]);
//	}

	QspiFlash_Init();
	//WatchDogInit();			//看门狗初始化

//	init_Commulink();
	init_drv_Sensors();

//	wp40_Init();
//	Servo_init();
}
