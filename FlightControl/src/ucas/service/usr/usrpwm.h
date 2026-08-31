/*
 * usrpwm.h
 *
 *  Created on: 2023Äê7ÔÂ27ÈÕ
 *      Author: liyuan
 */

#ifndef SRC_UCAS_SERVICE_USR_USRPWM_H_
#define SRC_UCAS_SERVICE_USR_USRPWM_H_

#include "xil_types.h"
#include "xparameters.h"


#define PWM_BASEADDR  0x43C20000


int PWMConfig(u8 ch, u32 frequency);
void PWMControl(u8 ch, float Duty, u8 state);
void PWMAllStop();

#endif /* SRC_UCAS_SERVICE_USR_USRPWM_H_ */
