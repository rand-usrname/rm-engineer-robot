/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_sentrychas.h"
#include "drv_remote.h"
#include "drv_gyro.h"

int main(void)
{
	chassis_init();
	remote_uart_init();
	
	while(1)
	{
		if(Change_from_middle(S1)==middle_to_up)
		{
			int i=0;
		}
	}
	
}