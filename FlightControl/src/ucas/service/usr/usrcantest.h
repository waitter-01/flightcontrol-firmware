/*
 * usrcantest2.h
 *
 *  Created on: 2022Äê1ÔÂ28ÈÕ
 *      Author: liyuan
 */

#ifndef SRC_UCAS_SERVICE_USR_USRCANTEST_H_
#define SRC_UCAS_SERVICE_USR_USRCANTEST_H_

#include "xcanps.h"

#define XCANPS_MAX_FRAME_SIZE_IN_WORDS ((XCANPS_MAX_FRAME_SIZE) / (sizeof(u32)))

int CanTest();

#endif /* SRC_UCAS_SERVICE_USR_USRCANTEST_H_ */
