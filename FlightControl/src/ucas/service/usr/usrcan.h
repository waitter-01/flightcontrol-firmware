
#include "can.h"

#ifndef SRC_UCAS_SERVICE_USR_USRCAN_2_H_
#define SRC_UCAS_SERVICE_USR_USRCAN_2_H_


#define USR_CAN_MAX			2

extern CAN_PARAM g_UsrCan[USR_CAN_MAX];
extern CAN_PARAM  can_param[];

BOOL UsrCanInit(void);
void CanBaudConfig(BYTE Dev, BYTE baud);

DWORD CanSendFrame(int Dev, u32 *pbyData);
DWORD CanRecvFrame(int Dev, u32 *pbyData);
void CanIDConfig(DWORD dwCanId, BYTE Dev);

#endif /* SRC_UCAS_SERVICE_USR_USRCAN_2_H_ */
