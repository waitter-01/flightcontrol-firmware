/*
 * usrpdsxtest.c
 *
 *  Created on: 2023Äê12ÔÂ14ÈÕ
 *      Author: liyuan
 */

#include "usrpdsxtest.h"
#include "sleep.h"
#include <stdio.h>
#include "usrpdsxcontrol.h"

#define   CHANNEL_ON 	1
#define   CHANNEL_OFF 	0

int PD_SX_Test()
{
	char c;
	DWORD PDState = 0, SXState = 0;
	DWORD PDStatus = 0, SXStatus = 0;
	DWORD PDChannel = 0, SXChannel = 0;

	while(1)
	{
		printf("----------- Shi Xu Pei Dian  Test -------------\n");
		printf("1. PD Single Channel Test.\n");
		printf("2. PD All Channel Turn ON.\n");
		printf("3. PD All Channel Turn OFF.\n");
		printf("4. SX Single Channel Test.\n");
		printf("5. SX All Channel Turn ON.\n");
		printf("6. SX All Channel Turn OFF.\n");

		printf("q. Quit.\n");
		printf("Please enter:\n");

		PD_SX_Init();

//		SetSX_Enable(0, 1);

		scanf(" %c",&c);
//		c = '1';

		if(c == '1')
		{
			printf("Please choice PD channel number(0-6):\n");
			scanf("%d", &PDChannel);
			printf("PD channel number is %d.\r\n",PDChannel);
			printf("Please choice PDState ON:1 OFF:0:\n");
			scanf("%d", &PDState);
			printf("PDState is %d.\r\n",PDState);
			SetPD_Enable(PDChannel, 1);
			SetPD_Status(PDChannel, PDState);

			usleep(10000);
			PDStatus = GetPD_Status();
			printf("PD current status is: %x\r\n", PDStatus);

		}

		if(c == '2')
		{
			for(int i = 0;i < 7;i++)
			{
				SetPD_Enable(i, CHANNEL_ON);
				SetPD_Status(i, CHANNEL_ON);
			}
			usleep(10000);
			PDStatus = GetPD_Status();
			printf("PD current status is: %x\r\n", PDStatus);
		}

		if(c == '3')
		{
			for(int i = 0;i < 7;i++)
			{
				SetPD_Status(i, CHANNEL_OFF);
			}
			usleep(10000);
			PDStatus = GetPD_Status();
			printf("PD current status is: %x\r\n", PDStatus);

		}

		if(c == '4')
		{
			printf("Please choice SX channel number(0-14):\n");
			scanf("%d", &SXChannel);
			printf("SX channel number is %d.\r\n",SXChannel);
			printf("Please choice SXState ON:1 OFF:0:\n");
			scanf("%d", &SXState);
			printf("SXState is %d.\r\n",SXState);

			SetSX_Enable(SXChannel, 1);
			SetSX_Status(SXChannel, SXState);

			usleep(10000);
			SXStatus = GetSX_Status();
			printf("SX current status is: %x\r\n", SXStatus);

		}

		if(c == '5')
		{
			for(int i = 0;i < 15;i++)
			{
				SetSX_Enable(i, CHANNEL_ON);
				SetSX_Status(i, CHANNEL_ON);
			}
			usleep(10000);
			SXStatus = GetSX_Status();
			printf("SX current status is: %x\r\n", SXStatus);
		}

		if(c == '6')
		{
			for(int i = 0;i < 15;i++)
			{
				SetSX_Enable(i, CHANNEL_ON);
				SetSX_Status(i, CHANNEL_OFF);
			}
			usleep(10000);
			SXStatus = GetSX_Status();
			printf("SX current status is: %x\r\n", SXStatus);

		}

		if(c == 'q')
		{
			break;
		}

	}

	return 0;
}


