/*
 * usrpwmtest.c
 *
 *  Created on: 2023Äê12ÔÂ20ÈÕ
 *      Author: liyuan
 */
#include "usrpwmtest.h"
#include "usrpwm.h"
#include <stdio.h>

void PWM_Test()
{
	char c;
	int PWMChannel = 0;
	int state = 0;
	int frequency = 0;
	int intDuty = 0;
	float fDuty = 0;


	while(1)
	{
		printf("------------- PWM Test -------------\r\n");
		printf("1. Single Channel Test.\n");
		printf("2. All Channel Output:(Freq:1hz Duty:0.5).\n");
		printf("3. All Channel Stop.\n");
		printf("q. Quit.\n");
		printf("Please enter:\n");

		scanf(" %c",&c);
		if(c == '1')
		{
			printf("please choise channel number(0-7)\r\n");
			scanf("%d", &PWMChannel);
			printf("PWM Channe Number is %d.\r\n",PWMChannel);

			printf("please choise output or not(0-1)\r\n");
			scanf("%d", &state);
			printf("Output state is %d\r\n", state);

			printf("please input  the frequency (e.g: 1:1s, 2:0.5s):\r\n");
			scanf("%d", &frequency);
			printf("The frequency is %d\r\n",frequency);

			printf("please input  the duty:\r\n");
			scanf("%d", &intDuty);
			fDuty = intDuty * 0.01;
			printf("The duty is %.2f\r\n",fDuty);

			PWMConfig(PWMChannel, frequency);
			PWMControl(PWMChannel, fDuty, state);
		}
		if(c == '2')
		{
			printf("All Channel Output!\r\n");
			for(int i = 0;i < 8;i++)
			{
				PWMConfig(i, 1);
				PWMControl(i, 0.5, 1);
			}

		}
		if(c == '3')
		{
			printf("All Channel Stop!\n");
			PWMAllStop();
		}
		if(c == 'q'){
			break;
		}
	}
}


