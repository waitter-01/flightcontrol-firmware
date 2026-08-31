/*
 * mavlink_SendFuncs.h
 *
 *  Created on: 2024Äê6ÔÂ21ÈÕ
 *      Author: wanghaobo
 */

#ifndef SRC_MAVLINK_SENDFUNCS_H_
#define SRC_MAVLINK_SENDFUNCS_H_
#include "mavlink.h"

extern bool (*const Mavlink_Send_Funcs[])( uint8_t port , mavlink_message_t* msg_sd );
extern const uint16_t Mavlink_Send_Funcs_Count;

#endif /* SRC_MAVLINK_SENDFUNCS_H_ */
