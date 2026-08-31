/*
 * usrcantest2.c
 *
 *  Created on: 2022年1月28日
 *      Author: liyuan
 */

/*
 * usrcantest.c
 *
 *  Created on: 2020年11月5日
 *      Author: liyuan
 */
#include <stdio.h>
#include <usrcan.h>
#include "usrcantest.h"
#include "usranalyzer.h"
#include "sleep.h"

u32 SendData[XCANPS_MAX_FRAME_SIZE_IN_WORDS] = {0xEB, 0x90, 0xEB900011, 0x121355AA, 0x03, 0x04, 0x05,0x06};
u32 RecvData[XCANPS_MAX_FRAME_SIZE_IN_WORDS];

BYTE ConvertData[8] = {0};

DWORD stCanId[2] = {0x7DA, 0x1ff1f7DB};			//CAN ID 配置

int CanTest()
{
	BYTE c;
	BYTE byPackData[1024];
	DWORD dwPackSize;
	DWORD dwDeviceNum;

	while(1)
	{
		printf("------------- CAN Test -------------\n");
		printf("1. Single Device Test.\n");
		printf("2. Mutual Sending And Receiving Test.\n");
		printf("q. Quit.\n");
		printf("Please enter:\n");

		scanf(" %c",&c);
//		c = '1';

		if(c == '1')
		{
			printf("Please send data:\n");
			DWORD dwcount = 0;
			int baud;
			printf("Please Choose the baudrate.\n");
			printf("0:1000kbps  1:800kbps  2:500kbps  3:200kbps  4:100kbps  5:50kbps\n");
			printf("Please enter:\n");

			scanf("%d",&baud);
//			baud = 0;
			printf("Baud choice is %d.\n",baud);

			printf("Please choice device number(0-1)\n");
			scanf("%d",&dwDeviceNum);
			printf("The device number is: %d.\n",dwDeviceNum);

			printf("Please Send Data!\r\n");

			CanBaudConfig(dwDeviceNum,baud);		//设备波特率配置
			CanIDConfig(stCanId[0], 0);
			CanIDConfig(stCanId[1], 1);

			while(1)
			{
				CanRecvFrame(dwDeviceNum, RecvData);

				for(int i = 0;i < 8;i++)
				{
					ConvertData[i] = RecvData[i/4 + 2] >> ((i%4) * 8);			//数据转换给分析器
				}

				AnalyzerWrite(&g_UsrAnalyzerCAN, ConvertData, 8);				//写分析器fifo

				if(AnalyzerParse(&g_UsrAnalyzerCAN, byPackData, &dwPackSize))
				{
					byPackData [2] = 0x02;										//修改传输方向标志位
					for(int i = 0;i < 8;i++)									//数据转换并发送
					{
						SendData[2] = byPackData[0+i*8] | byPackData[1+i*8]<<8 | byPackData[2+i*8]<<16 | byPackData[3+i*8]<<24;
						SendData[3] = byPackData[4+i*8] | byPackData[5+i*8]<<8 | byPackData[6+i*8]<<16 | byPackData[7+i*8]<<24;

						CanSendFrame(dwDeviceNum, SendData);

					}
					dwcount += 1;
				}
				if(dwcount == 2)
					break;
			}
		}

		if(c == '2')
		{
			CanBaudConfig(0, 0);		//设备0波特率配置
			CanBaudConfig(1, 0);		//设备1波特率配置

			for(int j = 0;j < 5;j++)
			{
//				SendData[2] = 0xEB900111;
				CanSendFrame(1, SendData);
				usleep(1000);
				CanRecvFrame(0, RecvData);

				for(int i = 2;i < 4;i++)
				{
					xil_printf("CAN Dev[0] recv[%d] = %X", i-2, RecvData[i]);
					xil_printf("  ");
				}
				printf("\n");
				SendData[2] += 0x0100;
			}

			SendData[2] = 0xEB900011;
			for(int j = 0;j < 5;j++)
			{
				CanSendFrame(0, SendData);
				usleep(1000);
				CanRecvFrame(1, RecvData);
				for(int i = 2;i < 4;i++)
				{
					xil_printf("CAN Dev[1] recv[%d] = %X", i-2, RecvData[i]);
					xil_printf("  ");
				}
				printf("\n");
				SendData[2] += 0x0100;
			}
		}
		if(c == 'q')
			break;

	}
	return 0;
}

