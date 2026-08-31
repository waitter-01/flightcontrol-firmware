/*
 * interface.c
 *
 *
 *
 *
 *
 *
 *  Created on: 2020年9月24日
 *      Author: liyuan
 */
#include "interface.h"
#include "utypes.h"
#include "usruart.h"

//#include "AXI_AD7606.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xgpio.h"
#include "xuartps.h"
#include "drvuartps.h"
#include "libcrc.h"
#include "sleep.h"
#include "usranalyzer.h"
#include "xtime_l.h"
#include "AXI_4Ch_UART.h"



//XTime tEnd, tCur;			//计时开始、结束
/******************************************UART ******************************************/

/*
	功能：设置串口参数
	入口：
	ch：串口号，值域0-7
	Baud：波特率
	Par：校验位，’n’无校验，’e’偶校验，’o’奇校验
	返回值：错误标志Errcode
*/
int UART_Setting(int ch, unsigned long Baud, char Par)
{
	if( Par == 'n' || Par == 'N')
	{
		g_UsrUartPL[ch].Param.bNoParity = FALSE;
	}
	else if(Par == 'e'|| Par == 'E')
	{
		g_UsrUartPL[ch].Param.bNoParity = TRUE;

		g_UsrUartPL[ch].Param.byParity = 0;
	}
	else
	{
		g_UsrUartPL[ch].Param.bNoParity = TRUE;
		g_UsrUartPL[ch].Param.byParity = 1;
	}

	g_UsrUartPL[ch].Param.dwBaud = Baud;

	UartInit(&g_UsrUartPL[ch]);

	return 0;
}

/*
	功能：从串口发送一个数据包
	入口：
	ch：串口号，值域0-7
	TxBuff：发送的数据包的数组首地址
	Length：发送的长度
	返回值：	成功返回写入数据长度

*/

int UART_PutBuff(int ch, unsigned char * TxBuff, unsigned short Length)
{
	int WriteLength;
	WriteLength = UartWrite(&g_UsrUartPL[ch], TxBuff, Length);

	return WriteLength;

}
/*
	功能：以超时方式检查串口是否收到一帧
	入口：
	ch：串口号，值域0-7
	返回值：
	若有帧结束事件，返回缓存数据长度
*/

int _UART_IsFrameReady(int ch)
{

	return 0;
}


/*
	功能：查询并接收一帧
	入口：
	ch：串口号，值域0-7
	RxBuff：用于存储接收数据帧的数组首地址
	MaxLength：接收数组的长度，即允许接收的最大长度
	返回值：
	若成功接收一帧，返回缓存数据长度(正值)
	若没有返回一帧，返回FALSE
 */

int UART_GetFrame(int ch, unsigned char * RxBuff, unsigned short MaxLength)
{
	int dwRet;

	dwRet = UartRead(&g_UsrUartPL[ch], RxBuff, MaxLength);

	return dwRet;

}



/******************************************ADC ******************************************/

/*
	 功能：模拟采样，ADC原始数据
	入口：
	ValueBuff：数组首地址
	返回值：
	成功返回0
 */

int ADC_GetValue(unsigned short *ValueBuff)
{
	u32 ADC_Value[8];

//	取四组32位数值，每组的高16位和低16位为ADC原始数值（分别对应该组数据的低通道，高通道）。
//	数据处理。每路基于基地址偏移+0x4
	ADC_Value[0] =AXI_4CH_UART_mReadReg(XPAR_AXI_AD7606_0_S00_AXI_BASEADDR, AD7606_REG0_OFFSET);
	ADC_Value[1] =AXI_4CH_UART_mReadReg(XPAR_AXI_AD7606_0_S00_AXI_BASEADDR, AD7606_REG1_OFFSET);
	ADC_Value[2] =AXI_4CH_UART_mReadReg(XPAR_AXI_AD7606_0_S00_AXI_BASEADDR, AD7606_REG2_OFFSET);
	ADC_Value[3] =AXI_4CH_UART_mReadReg(XPAR_AXI_AD7606_0_S00_AXI_BASEADDR, AD7606_REG3_OFFSET);

	ADC_Value[4] =AXI_4CH_UART_mReadReg(XPAR_AXI_AD7606_1_S00_AXI_BASEADDR, AD7606_REG0_OFFSET);
	ADC_Value[5] =AXI_4CH_UART_mReadReg(XPAR_AXI_AD7606_1_S00_AXI_BASEADDR, AD7606_REG1_OFFSET);
	ADC_Value[6] =AXI_4CH_UART_mReadReg(XPAR_AXI_AD7606_1_S00_AXI_BASEADDR, AD7606_REG2_OFFSET);
	ADC_Value[7] =AXI_4CH_UART_mReadReg(XPAR_AXI_AD7606_1_S00_AXI_BASEADDR, AD7606_REG3_OFFSET);

	ValueBuff[0] = (ADC_Value[0] >> 16) & 0x0000ffff;
	ValueBuff[1] = ADC_Value[0] & 0x0000ffff;

	ValueBuff[2] = (ADC_Value[1] >> 16) & 0x0000ffff;
	ValueBuff[3] = ADC_Value[1] & 0x0000ffff;

	ValueBuff[4] = (ADC_Value[2] >> 16) & 0x0000ffff;
	ValueBuff[5] = ADC_Value[2] & 0x0000ffff;

	ValueBuff[6] = (ADC_Value[3] >> 16) & 0x0000ffff;
	ValueBuff[7] = ADC_Value[3] & 0x0000ffff;

	ValueBuff[8] = (ADC_Value[4] >> 16) & 0x0000ffff;
	ValueBuff[9] = ADC_Value[4] & 0x0000ffff;

	ValueBuff[10] = (ADC_Value[5] >> 16) & 0x0000ffff;
	ValueBuff[11] = ADC_Value[5] & 0x0000ffff;

	ValueBuff[12] = (ADC_Value[6] >> 16) & 0x0000ffff;
	ValueBuff[13] = ADC_Value[6] & 0x0000ffff;

	ValueBuff[14] = (ADC_Value[7] >> 16) & 0x0000ffff;
	ValueBuff[15] = ADC_Value[7] & 0x0000ffff;

	return 0;
}

/*
	 功能：模拟采样，输入电压值
	入口：
	ValueBuff：数组首地址
	返回值：
	成功返回0

 */
int ADC_GetVoltage(float * ValueBuff)
{
	unsigned short ValueBuffer[16];   //8路ADC采集原始值
	int numsign[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};            //电压正负取值符号

	ADC_GetValue(ValueBuffer);

//	数据处理，如果是负值，取补码
	for(int i = 0; i < 16;i++)
	{
		if(ValueBuffer[i] >= 0x8000)
		{
			ValueBuffer[i]  = (~ValueBuffer[i]) + 1;
			numsign[i] = -1;
		}
	}

//	获取八路adc电压值，每个数据的高16位为低位数据，低16位为高位地址
	ValueBuff[0] = (float)(ValueBuffer[0] & 0x0000ffff) * 10/32768 * numsign[0];
	ValueBuff[1] = (float)(ValueBuffer[1] & 0x0000ffff) * 10/32768 * numsign[1];

	ValueBuff[2] = (float)(ValueBuffer[2] & 0x0000ffff) * 10/32768 * numsign[2];
	ValueBuff[3] = (float)(ValueBuffer[3] & 0x0000ffff) * 10/32768 * numsign[3];

	ValueBuff[4] = (float)(ValueBuffer[4] & 0x0000ffff) * 10/32768 * numsign[4];
	ValueBuff[5] = (float)(ValueBuffer[5] & 0x0000ffff) * 10/32768 * numsign[5];

	ValueBuff[6] = (float)(ValueBuffer[6] & 0x0000ffff) * 10/32768 * numsign[6];
	ValueBuff[7] = (float)(ValueBuffer[7] & 0x0000ffff) * 10/32768 * numsign[7];

	ValueBuff[8] = (float)(ValueBuffer[8] & 0x0000ffff) * 10/32768 * numsign[8];
	ValueBuff[9] = (float)(ValueBuffer[9] & 0x0000ffff) * 10/32768 * numsign[9];

	ValueBuff[10] = (float)(ValueBuffer[10] & 0x0000ffff) * 10/32768 * numsign[10];
	ValueBuff[11] = (float)(ValueBuffer[11] & 0x0000ffff) * 10/32768 * numsign[11];

	ValueBuff[12] = (float)(ValueBuffer[12] & 0x0000ffff) * 10/32768 * numsign[12];
	ValueBuff[13] = (float)(ValueBuffer[13] & 0x0000ffff) * 10/32768 * numsign[13];

	ValueBuff[14] = (float)(ValueBuffer[14] & 0x0000ffff) * 10/32768 * numsign[14];
	ValueBuff[15] = (float)(ValueBuffer[15] & 0x0000ffff) * 10/32768 * numsign[15];

	return 0;
}

/******************************************GPIO ******************************************/

/*
	读取和设置gpio数据状态
	gpio通道1为输入通道，读取6路io状态
	gpio通道2为输出通道，设置3路io状态
 */
XGpio Gpio;

int GpioChannel_Init()
{
	int Status;
	Status = XGpio_Initialize(&Gpio, GPIO_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}

//	把通道设为输入模式
	XGpio_SetDataDirection(&Gpio, GPIO_CHANNEL1, 1);
//	把通道设为输出模式
	XGpio_SetDataDirection(&Gpio, GPIO_CHANNEL2, 0);
	return 0;
}

/*
	功能：查询IO通道状态
	入口：IO通道号
	返回值：
	成功：IO状态
	错误标志Errcode
 */
int Get_IOState(int IO_ch)
{
	int state;


//	读通道状态
	state = XGpio_DiscreteRead(&Gpio, IO_ch);
//	printf("io_state = %x\n",state);
	return state;

}


/*
	功能：IO通道控制
	入口：IO通道号，state控制状态
	返回值：
	成功返回0
	错误标志Errcode
*/
int Set_IOState(int IO_ch, int state)
{


//	配置通道状态
	XGpio_DiscreteWrite(&Gpio, IO_ch, state);

	return 0;
}



