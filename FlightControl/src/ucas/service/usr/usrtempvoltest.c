/*
 * usrtmp75test.c
 *
 *  Created on: 2022年6月27日
 *      Author: liyuan
 */
#include <stdio.h>
#include "usrtempvoltest.h"
#include "tempsensor.h"
#include "voltsensor.h"
#include "usrtemp.h"

/**********提取温度并打印**********/
void TempTest(void)
{
	float *FKtemp, *PDtemp, Coretemp;
	PDtemp = GetPDTemp();
	FKtemp = GetFKTemp();
	Coretemp = System_GetCoreTemperature();

	printf("FK Sensor1 temp = %.4f\n", *FKtemp);
	printf("FK Sensor2 temp = %.4f\n", *(FKtemp+1));
	printf("PD Sensor1 temp = %.4f\n", *PDtemp);
	printf("PD Sensor2 temp = %.4f\n", *(PDtemp+1));
	printf("CoreTemperature = %.4f\n", Coretemp);

}

void VolCurTest(void)
{
	float fvolcur[2] = {0};
	AnalogVolCur(fvolcur);

	printf("Voltage Analog Data = %.4f\r\n", fvolcur[0]);
	printf("Current Analog Data = %.4f\r\n", fvolcur[1]);

}





