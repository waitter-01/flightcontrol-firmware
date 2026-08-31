/*
 * usrgpiotest.c
 *
 *  Created on: 2020年9月25日
 *      Author: liyuan
 */

#include "usrgpiotest.h"
#include "interface.h"
#include "utypes.h"

void UsrGPIOTest()
{
	char c;
	int state = 0;

	while(1)
	{
		printf("----------- GPIO Test -------------\n");
		printf("1. Get IO State.\n");
		printf("2. Set IO State.\n");
		printf("q. Quit.\n");
		printf("Please enter:\n");

		scanf(" %c",&c);

		if(c == '1')
		{
			int value = 0;
			GpioChannel_Init();

			state = Get_IOState(GPIO_CHANNEL2);				//获取初始状态
			printf("io_state = %x\n",state);				//打印初始状态

			while(1)
			{
				state = Get_IOState(GPIO_CHANNEL2);

				if((state != value) && (state != 0x3f))					//换按键，自动打印当前状态
				{
					printf("IO State = %x\n",state);
					value = state;
				}
				if(state == 0x3c)					//如果同时按无源前两个(gpi0、gpi1)，退出循环
				{
					break;
				}
//				printf("io_state = %x\n",state);
			}


		}
		if(c == '2')
		{
			printf("Please enter the channel state(Hex data) :\n");
			scanf("%x", &state);
			state &= 0x07;              //状态数据低三位有效
			printf("State is :%x.\n",state);
			GpioChannel_Init();
			Set_IOState(GPIO_CHANNEL1, state);

		}
		if(c == 'q')
		{
			break;
		}
	}

}
