/*
 * usrtemp.h
 *
 *  Created on: 2020年10月28日
 *      Author: liyuan
 */
#include "xadcps.h"


#ifndef SRC_UCAS_SERVICE_USR_USRTEMP_H_
#define SRC_UCAS_SERVICE_USR_USRTEMP_H_

#define XADC_DEVICE_ID      XPAR_XADCPS_0_DEVICE_ID        //PS XADC 器件 ID
int temp_get_init();
float System_GetCoreTemperature(void);
void CoreTempTest(void);
#endif /* SRC_UCAS_SERVICE_USR_USRTEMP_H_ */
