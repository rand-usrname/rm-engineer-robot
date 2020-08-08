/************************************** Copyright ****************************** 
  *                 (C) Copyright 2020,mqy,China,HITwh.
  *                            All Rights Reserved
  *
  *                     HITwh Excellent Robot Organization
  *                     https://github.com/HERO-ECG
  *                     https://gitee.com/HIT-718LC
  *    
  * FileName   : drv_rgblight.h
  * Version    : v1.0
  * Author     : mqy
  * Date       : 2020-02-14
  * Description:
********************************************************************************/

#ifndef __DRV_RGBLIGHT_H__
#define __DRV_RGBLIGHT_H__

#include "rtthread.h"
#include "drv_common.h"

#define CORE_WHITE		255,255,255
#define CORE_BLACK		0,0,0
#define CORE_RED		255,0,0
#define CORE_BLUE		0,0,255
#define CORE_GREEN		0,255,0
#define CORE_YELLOW		255,255,0
#define CORE_PURPLE		128,0,128

extern void set_RGB(rt_uint8_t red,rt_uint8_t green,rt_uint8_t blue);

#endif
