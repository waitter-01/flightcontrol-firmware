/*
 * MavlinkCMDProcess.h
 *
 *  Created on: 2024年6月21日
 *      Author: wanghaobo
 */

#ifndef SRC_MAVLINKCMDPROCESS_H_
#define SRC_MAVLINKCMDPROCESS_H_
#include "mavlink.h"

//Mavlink命令处理函数表
extern void (*const Mavlink_CMD_Process[])( uint8_t port_index , const mavlink_message_t* msg_rd );
//Mavlink命令处理函数个数
extern const uint16_t Mavlink_CMD_Process_Count;

#endif /* SRC_MAVLINKCMDPROCESS_H_ */
