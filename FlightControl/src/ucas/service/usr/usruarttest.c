/*
 * usruarttest.c
 *description：串口单路，多路测试函数
 *  Created on: 2022年1月24日
 *      Author: liyuan
 */

#include "usruarttest.h"
#include "usruart.h"
#include "usranalyzer.h"
#include "libcrc.h"
#include "interface.h"
#include "sleep.h"
#include "xtime_l.h"
#include  "sleep.h"

void UsrUartTest(void)
{
	BYTE c;
	DWORD dwIndexSend;
	DWORD dwIndexRecv;
//	DWORD dwBaud;
//	BYTE byParity;


	while(1)
	{
		printf("----------- Serial Test -------------\n");
		printf("1. Single Serial test.\n");
		printf("2. Multiple Serial test.\n");
		printf("q. Quit.\n");
		printf("Please enter:\n");

		scanf(" %c",&c);
//		c = '1';

		if(c == '1')
		{
			/////////////////////////////////////////////////
			printf("Please choice PL send number(0-%d):\n",USR_UART_PL_MAX-1);
			scanf("%d", &dwIndexSend);

//			dwIndexSend = 0;
			printf("Send number is %d.\n",dwIndexSend);
			if(dwIndexSend >= USR_UART_PL_MAX)
			{
				printf("Send number error!\n");
				continue;
			}
			/////////////////////////////////////////////////
			printf("Please choice PL recv number(0-%d):\n",USR_UART_PL_MAX-1);
			scanf("%d", &dwIndexRecv);
//			dwIndexRecv = 0;

			printf("Recv number is %d.\n",dwIndexRecv);
			if(dwIndexRecv >= USR_UART_PL_MAX)
			{
				printf("Recv number error!\n");
				continue;
			}

			UART_Setting(dwIndexSend, 921600, 'o');
			UART_Setting(dwIndexRecv, 921600, 'o');

			{
				DWORD dwSendCount = 0;
//				DWORD dwSendBytes = 0;
				DWORD dwRecvBytes = 0;
//				DWORD dwAnalyzerCount = 0;
//				DWORD dwIndex = 0;
				DWORD dwRet;
				DWORD dwLen = g_UsrAnalyzerDemo.dwPackSizeMax;
				BYTE bySendData[2048] = {
									0x55, 0xAA, 0x08, 0x22, 0x33, 0x44,
									0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
									0xff, 0xff
								};
				BYTE byRecvData[2048];
//				BYTE byPackData[2048];
//				DWORD dwPackSize;
				DWORD SendBytes = 260 * 4;

				WORD wCrc = CalCRC16(bySendData+2, dwLen-4);				//crc校验
				bySendData[258] = (wCrc >> 8)&0xff;
				bySendData[259] = (wCrc >> 0)&0xff;

				dwSendCount = 100;
				int count = 0;

				while(1)
				{
					if(dwSendCount > 0)
					{
						count++;
						dwRet = UART_PutBuff(dwIndexSend, bySendData, SendBytes);		//dwLen
//						printf("SendRet = %d, count = %d\n", dwRet,count);
						dwSendCount--;
					}

					usleep(15000);				//延时等待发送完成
					dwRet = UART_GetFrame(dwIndexRecv, byRecvData, 300);		//接收数据

					dwRecvBytes += dwRet;		//接收计数累加
					dwRet = 0;

					if(dwSendCount == 0 )
					{
						printf("Port[%d] Recieve Bytes = %d\n", dwIndexRecv, dwRecvBytes);
						break;
					}
				}
			}

		}
		if(c == '2')
		{
			UartMultiTest();
		}
		if(c == 'q')
			break;
	}
}



//8路串口收发测试
void UartMultiTest(void)
{
	int i = 0;
	DWORD dwIndexSend;
	DWORD dwIndexRecv;
	DWORD dwRet;
	DWORD dwPackSize = 500;
	BYTE byRecvData[2048] = {0};
//	BYTE byTransData[1024] = {0};
//	BYTE g_byPackData[1024] = {0};
	DWORD dw_datacount[14] = {0};

//	DWORD dwAnalyzerCount[7] = {0};
	DWORD dwPacketcount = 100;
	DWORD dwPacketcount2 = 100;
	BYTE SerialPortNum[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8};		//

	DWORD dwLen = g_UsrAnalyzerUARTPL[0].dwPackSizeMax;
	BYTE bySendData[2048] = {
								0x55, 0xAA, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
								0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC
							};

//	for(i = 13;i >= 0; i--)
	for(i = 0;i < 8; i++)
	{
		UART_Setting(i, 921600, 'o');
	}

//	crc校验
	for(i = 0;i <100;i++)
	{
		bySendData[3]++;

		WORD wCrc = CalCRC16(bySendData+2, dwLen-4);
		bySendData[258] = (wCrc >> 8)&0xff;
		bySendData[259] = (wCrc >> 0)&0xff;

//		for(dwIndexSend = 0;dwIndexSend < 3;dwIndexSend++)
//		{
//			dwIndexSend = 2*dwIndexSend;
//			AnalyzerWrite(&g_UsrAnalyzerUARTPL[dwIndexSend], bySendData, 16);
//
//		}
		for(int j = 0;j < 7;j++)
		{
			AnalyzerWrite(&g_UsrAnalyzerUARTPL[j*2], bySendData, 16);
		}
	}


	while(1)
	{
		if(dwPacketcount > 0)
		{
			for(i = 0; i < 4;i++)
			{
				dwIndexSend = SerialPortNum[2*i];

				UART_PutBuff(dwIndexSend, bySendData, dwPackSize);
			}
//			延时等待发送完成
			usleep(10000);
			for(i = 0; i < 4;i++)
			{
				dwIndexRecv = SerialPortNum[2*i + 1];

				dwRet = UART_GetFrame(dwIndexRecv, byRecvData, 100);
				dw_datacount[dwIndexRecv] += dwRet;

			}

			dwPacketcount--;
		}

		if(dwPacketcount2 > 0)
		{
			for(i = 0; i < 4;i++)
			{
				dwIndexSend = SerialPortNum[2*i + 1];

				UART_PutBuff(dwIndexSend, bySendData, dwPackSize);				//发送数据

			}
//			延时等待发送完成
			usleep(10000);
			for(i = 0; i < 4;i++)
			{
				dwIndexRecv = SerialPortNum[2*i];

				dwRet = UART_GetFrame(dwIndexRecv, byRecvData, 100);	//接收数据
				dw_datacount[dwIndexRecv] += dwRet;
			}

			dwPacketcount2--;
		}

//		打印接收数据字节数
		if((dwPacketcount == 0)&&(dwPacketcount2 == 0))
		{
//			printf("port [%2d] recive data = %d. \n", SerialPortNum[10], dw_datacount[SerialPortNum[10]]);
			for(int j = 0;j < 8;j++)
			{
				printf("Port[%d] Receive Bytes = %d. \n", j, dw_datacount[j]);
			}

			break;
		}

	}

}


