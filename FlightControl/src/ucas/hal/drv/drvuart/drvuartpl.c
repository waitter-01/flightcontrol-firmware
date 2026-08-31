#include "drvuartpl.h"
#include "uart.h"
#include "utypes.h"
#include "xuartps_hw.h"
#include "AXI_4Ch_UART.h"
#include "xil_cache.h"
#include "string.h"
//#include "cache.h"
#include "xtime_l.h"




#if 0
/**********************************串口模式**************************************/

BOOL DrvUartPLInit(void *arg)
{
	u32 baud;
    DWORD dwRet;

	PUART_PARAM pParam = (PUART_PARAM)arg;
	DWORD dwBaseAddress = pParam->dwBaseAddr;
	BOOL Parity = pParam->bNoParity;
	BYTE ParityBits = pParam->byParity;
        
        int reg = 0;

//根据奇偶校验、波特率配置寄存器
#if 1
//	根据地址判断串口时钟频率
	if(dwBaseAddress < 0x80000000)
	{
		baud = (100000000/pParam->dwBaud)-1;
	}
	else
	{
		baud = (100000000/pParam->dwBaud)-1;
	}

//	判断串口是否有检验
	if(Parity == TRUE)
	{
		if(ParityBits == 0)        //如果是偶校验
		{
			AXI_4CH_UART_mWriteReg(dwBaseAddress, 0, 0xC0000000|baud);
		}
		else                      //奇校验
		{
			AXI_4CH_UART_mWriteReg(dwBaseAddress, 0, 0xE0000000|baud);
		}

	}
	else
	{
		AXI_4CH_UART_mWriteReg(dwBaseAddress, 0, 0x80000000|baud);
	}

//        printf("reg[0] = %x\r\n", reg)
        
#endif

//	发送控制寄存器：清空缓存，使能阈值，设置阈值4096
	AXI_4CH_UART_mWriteReg(dwBaseAddress, 0x04, 0x80000000);
        dwRet = AXI_4CH_UART_mReadReg(dwBaseAddress, 0x8);
        printf("dwret = %x\r\n",dwRet);


//	接收控制寄存器：清空缓存，使能阈值，设置阈值4096
	AXI_4CH_UART_mWriteReg(dwBaseAddress, 0x08, 0x80000000);
        
        dwRet = AXI_4CH_UART_mReadReg(dwBaseAddress, 0x8);
        printf("dwret = %x\r\n",dwRet);

//	中断控制寄存器：
	AXI_4CH_UART_mWriteReg(dwBaseAddress, 0x50, 0x000FFFFF);
        
        AXI_4CH_UART_mWriteReg(dwBaseAddress, 0x108, 0xF);			//模式配置,0：dma模式；1：串口模式

	return TRUE;

}



DWORD DrvUartPLRead(void *arg, BYTE *pbyData, DWORD dwSize)
{
	PUART_PARAM pParam = (PUART_PARAM)arg;

	DWORD dwBaseAddress = pParam->dwBaseAddr;

	DWORD dwRet, dwIndex = 0;

	dwRet = AXI_4CH_UART_mReadReg(dwBaseAddress, 0x8);
	dwRet = dwRet & 0x00000fff;

	if(dwRet < dwSize)
//		return 0;
		dwSize = dwRet;

	for(dwIndex = 0; dwIndex < dwSize; dwIndex++)
	{
		pbyData[dwIndex] = AXI_4CH_UART_mReadReg(dwBaseAddress, 0x10);
                
                printf("phydata[%d] = %x\r\n", dwIndex, pbyData[dwIndex]);
	}

	return dwSize;

}


DWORD DrvUartPLWrite(void *arg, BYTE *pbyData, DWORD dwSize)
{
	PUART_PARAM pParam = (PUART_PARAM)arg;

	DWORD dwBaseAddress = pParam->dwBaseAddr;

	DWORD dwRet, dwN, dwIndex;
        
        BYTE testdata = 0;

	dwRet = AXI_4CH_UART_mReadReg(dwBaseAddress, 0x4);
	dwRet = dwRet & 0x01fff;

	dwN = 4096 - dwRet;

	if (dwN < dwSize)
		dwSize = dwN;

	for(dwIndex= 0; dwIndex < dwSize; dwIndex++)
	{
		AXI_4CH_UART_mWriteReg(dwBaseAddress, 0x0C, pbyData[dwIndex]);
               testdata = AXI_4CH_UART_mReadReg(dwBaseAddress, 0x0C);
               printf("dwret = %x\r\n", testdata);
//                
                
	}

	return dwIndex;

}

#endif




#if 1
/*******************************DMA模式*********************************/

BOOL DrvUartPLInit(void *arg)
{
	u32 baud;

	PUART_PARAM pParam = (PUART_PARAM)arg;
	DWORD dwBaseAddress = pParam->dwBaseAddr;
	DWORD dwDDRAddress = pParam->dwDDRAddr;				//ddr基地址
	BOOL Parity = pParam->bNoParity;
	BYTE ParityBits = pParam->byParity;

//根据奇偶校验、波特率配置寄存器
#if 1
//	根据地址判断串口时钟频率
	if(dwBaseAddress < 0x80000000)
	{
		baud = (100000000/pParam->dwBaud)-1;
	}
	else
	{
		baud = (100000000/pParam->dwBaud)-1;
	}

//	判断串口是否有检验
	if(Parity == TRUE)
	{
		if(ParityBits == 0)        //如果是偶校验
		{
			AXI_4CH_UART_mWriteReg(dwBaseAddress, 0, 0xC0000000|baud);
		}
		else                      //奇校验
		{
			AXI_4CH_UART_mWriteReg(dwBaseAddress, 0, 0xE0000000|baud);
		}
	}
	else
	{
		AXI_4CH_UART_mWriteReg(dwBaseAddress, 0, 0x80000000|baud);
	}

#endif
	if(dwBaseAddress == 0x83C00000)
	{
		Xil_Out32(dwBaseAddress+0x28,20000);			//gnss超时：200us;  100M工作时钟，5ms就是100*1000*5
	}
	else
	{
		Xil_Out32(dwBaseAddress+0x28,11000);			//串口超时：110us;  100M工作时钟，5ms就是100*1000*5
	}
	Xil_Out32(dwBaseAddress+0x14, dwDDRAddress);			            //写发送ddr基地址
	Xil_Out32(dwBaseAddress+0x1C,dwDDRAddress + 0x1000);				//写接收ddr基地址

//	AXI_4CH_UART_mWriteReg(dwBaseAddress, 0x108, 0xF);			//模式配置,0：dma模式；1：串口模式
	return TRUE;

}



DWORD DrvUartPLRead(void *arg, BYTE *pbyData, DWORD dwSize)
{
	PUART_PARAM pParam = (PUART_PARAM)arg;
	DWORD dwBaseAddress = pParam->dwBaseAddr;			//串口基地址
	DWORD dwDDRAddress = pParam->dwDDRAddr + 0x1000;	//接收ddr地址在基地址偏移0x1000

	int recv_len;
	BYTE status;
	int RecvData[1024] = {0};

	status = Xil_In32(dwBaseAddress+0x24);			//读一帧标志位

	if((status & 0x2) == 0x2)
	{
		recv_len = Xil_In32(dwBaseAddress + 0x20);       	//读接收的一帧数据长度

		Xil_Out32(dwBaseAddress + 0x24, 2);			//标志位写1清零

		//Xil_DCacheFlushRange(dwDDRAddress, recv_len);		 //刷新 Data Cache
		//flush_dcache_range(dwDDRAddress, dwDDRAddress + recv_len);
		Xil_DCacheInvalidateRange(dwDDRAddress, recv_len);
		if((recv_len%4) > 0)								//根据获得字节数读ddr
		{
			for(int i = 0; i < (recv_len/4 + 1); i++)
			{
				RecvData[i] = Xil_In32(dwDDRAddress + i*4);
//				printf("the address at  %x data is : %x \n\r" , dwDDRAddress+i*4, RecvData[i]);
			}
		}
		else
		{
			for(int i = 0; i < recv_len/4; i++)
			{
				RecvData[i] = Xil_In32(dwDDRAddress + i*4);
//				printf("the address at  %x data is : %x \n\r" , dwDDRAddress+i*4, RecvData[i]);
			}
		}
		//Xil_DCacheFlushRange(dwDDRAddress, recv_len);		 //刷新 Data Cache
		//flush_dcache_range(dwDDRAddress, dwDDRAddress + recv_len);
		memcpy(pbyData, RecvData, recv_len);

		return recv_len;
	}

	return FALSE;

}


DWORD DrvUartPLWrite(void *arg, const BYTE *pbyData, DWORD dwSize)
{
	PUART_PARAM pParam = (PUART_PARAM)arg;
	DWORD dwBaseAddress = pParam->dwBaseAddr;			//串口基地址
	DWORD dwDDRAddress = pParam->dwDDRAddr;				//ddr基地址

	DWORD status;
	int count = 6;
	int SendData[1024] = {0};

	memcpy(SendData, pbyData, dwSize);

    for(int i=0; i < (dwSize/4 + 1); i++)
    {
        Xil_Out32(dwDDRAddress+i*4, SendData[i]);			//数据写入ddr
        
//        printf("sendData[%x] = %x\r\n", i, Xil_In32(dwDDRAddress+i*4));
        
    }
    
    Xil_DCacheFlushRange(dwDDRAddress, dwSize); 			//刷新 Data Cache
//        flush_dcache_range(dwDDRAddress, dwDDRAddress + dwSize);
	Xil_Out32(dwBaseAddress+0x18, dwSize);					//写发送数据长度
//        printf("sendData[] = %x\r\n",  Xil_In32(dwBaseAddress+ 0x18));
        
	//Xil_DCacheFlushRange(dwDDRAddress, dwSize); 			//刷新 Data Cache
//        flush_dcache_range(dwDDRAddress, dwDDRAddress + dwSize);

	while(count--)
	{
		status = Xil_In32(dwBaseAddress+0x24);					//读取标志位
		if((status & 1) == 1)					    			//判断发送完成标志位
		{
			Xil_Out32(dwBaseAddress+0x24, 1);					//标志位写1清零
			return dwSize;
		}
	}

	return 0;

}

#endif

DWORD DrvUartPLIoctl(void *arg)
{
	PUART_PARAM pParam = (PUART_PARAM)arg;

	DWORD dwRet, dwN;
//	for(dwIndex = 0; dwIndex < 17; dwIndex++)
//	{
	DWORD dwBaseAddress = pParam->dwBaseAddr;
//	pUart->Param.dwBaseAddr = 0x43C00000+(dwIndex/4)*0x10000+(dwIndex%4)*0x14;

	dwRet = AXI_4CH_UART_mReadReg(dwBaseAddress, 0x4);
//	}
	dwRet = dwRet & 0x0fff;

	dwN = 4096 - dwRet;

	return dwN;
}


BOOL DrvUartPLOpen(void *arg)
{
	PUART_PARAM pParam = (PUART_PARAM)arg;

	return TRUE;
}	

BOOL DrvUartPLClose(void *arg)
{
	PUART_PARAM pParam = (PUART_PARAM)arg;

	return TRUE;
}
