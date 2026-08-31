/*
 * sxcontrol.h
 *
 *  Created on: 2023Äê7ÔÂ27ÈÕ
 *      Author: liyuan
 */

#ifndef SRC_UCAS_SERVICE_USR_USRPDSXCONTROL_H_
#define SRC_UCAS_SERVICE_USR_USRPDSXCONTROL_H_

#include "utypes.h"


#define PMOS_PD_ADDR		XPAR_GPIO_0_BASEADDR
#define PMOS_PD_DEV			XPAR_AXI_GPIO_CTRL_AXI_GPIO_28V_IN_DEVICE_ID

#define PMOS_ADDR			XPAR_GPIO_1_BASEADDR
#define PMOS_DEV			XPAR_AXI_GPIO_CTRL_AXI_GPIO_SX_PMOS_DEVICE_ID

#define RELAY_ADDR			XPAR_GPIO_2_BASEADDR
#define RELAY_DEV			XPAR_AXI_GPIO_CTRL_AXI_GPIO_SX_RELAY_DEVICE_ID


void PD_SX_Init(void);

int GetPD_Status(void);
int GetSX_Status(void);
void SetPD_Enable(DWORD ch, DWORD value);
void SetSX_Enable(DWORD ch, DWORD value);

void SetPD_Status(DWORD ch, DWORD value);
void SetSX_Status(DWORD ch, DWORD value);





#endif /* SRC_UCAS_SERVICE_USR_USRPDSXCONTROL_H_ */








