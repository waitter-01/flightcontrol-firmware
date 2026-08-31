/*
 * MavlinkRCProcess.h
 *
 *  Created on: 2024年6月21日
 *      Author: wanghaobo
 */

#ifndef SRC_MAVLINKRCPROCESS_H_
#define SRC_MAVLINKRCPROCESS_H_

#include "mavlink.h"

//Mavlink消息处理函数表
extern void (*const Mavlink_RC_Process[])( uint8_t Port_index , const mavlink_message_t* msg_sd );
//Mavlink消息处理函数个数
extern const uint16_t Mavlink_RC_Process_Count;

#endif /* SRC_MAVLINKRCPROCESS_H_ */
