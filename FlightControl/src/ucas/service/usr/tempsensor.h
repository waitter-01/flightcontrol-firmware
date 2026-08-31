/*
 * tmp75.h
 *
 *  Created on: 2022Äê6ÔÂ27ÈÕ
 *      Author: liyuan
 */
#include "xil_types.h"


#ifndef SRC_UCAS_SERVICE_USR_TMP75_H_
#define SRC_UCAS_SERVICE_USR_TMP75_H_


void XIicInit(void);
void Tmp75_Init(void);
u8 * Tmp75_RdTempsensor(UINTPTR BaseAddress, u8 Address);
u8 * RdTempsensor(void);

u8 * RdPDTempsensor(void);
u8 * RdFKTempsensor(void);

float *GetPDTemp();
float *GetFKTemp();

#endif /* SRC_UCAS_SERVICE_USR_TMP75_H_ */
