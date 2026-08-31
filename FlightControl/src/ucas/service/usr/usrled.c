/*
 * usrled.c
 *
 *  Created on: 2023年7月28日
 *      Author: liyuan
 */
#include "usrled.h"
#include "xgpio.h"


XGpio Gpio_LED; /* The Instance of the GPIO Driver */

//led gpio 初始化
int LedInit()
{
	int Status;

	/* Initialize the XPAR_AXI_GPIO_LED_DEVICE_ID Driver */
	Status = XGpio_Initialize(&Gpio_LED, XPAR_AXI_GPIO_LED_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("AXI_GPIO_0 Initialization Failed\r\n");
	}

	return 0;
}

// led 控制：4个led，通道1，低4位有效
//开：0 关：1
void LedWrite(u32 value)
{
	unsigned char Channel = 1;
	//	把通道设为输出模式
		XGpio_SetDataDirection(&Gpio_LED, Channel, 0x0);

	//	配置led状态
		XGpio_DiscreteWrite(&Gpio_LED, Channel, value);

}




