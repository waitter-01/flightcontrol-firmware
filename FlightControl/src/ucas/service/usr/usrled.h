/*
 * usrled.h
 *
 *  Created on: 2023Äê7ÔÂ28ÈÕ
 *      Author: liyuan
 */

#ifndef SRC_UCAS_SERVICE_USR_USRLED_H_
#define SRC_UCAS_SERVICE_USR_USRLED_H_

#include "utypes.h"


#define GPIO_LED_DEV		XPAR_AXI_GPIO_LED_DEVICE_ID


int LedInit();

void LedWrite(u32 value);

#endif /* SRC_UCAS_SERVICE_USR_USRLED_H_ */


