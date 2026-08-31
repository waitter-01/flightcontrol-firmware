/*
 * drvcanip.h
 *
 *  Created on: 2022年1月26日
 *      Author: liyuan
 */
#include "utypes.h"

#ifndef SRC_UCAS_HAL_DRV_DRVCAN_DRVCANIP_H_
#define SRC_UCAS_HAL_DRV_DRVCAN_DRVCANIP_H_


#define FRAME_DATA_LENGTH 		8 			//帧数据宽度

#define MESSAGE_ID			2000		//帧ID

#define CAN_DEV_MAX_NUM		2
//#define CAN_DEVICE_ID	XPAR_XCANPS_0_DEVICE_ID		//设备ID



BOOL DrvCanInit(void *arg);
BOOL DrvCanOpen(void *arg);
BOOL DrvCanClose(void *arg);
BOOL DrvCanIoctl(void *arg);
DWORD DrvCanRead(void *arg, u32 *pbyData);
DWORD DrvCanWrite(void *arg, u8 cmd,u32 *pbyData,u8 len);
BOOL Get_Can_Read_Flag(u32 DeviceID);
int Can_Read(u8 dir,u32 *id,u8 *data_len,u8 *buff);
int Can_Write(u8 dir,u8 cmdid,u8 *buff,u8 len);
#endif /* SRC_UCAS_HAL_DRV_DRVCAN_DRVCANIP_H_ */
