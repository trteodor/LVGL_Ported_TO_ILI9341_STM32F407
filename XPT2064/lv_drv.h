/*
 * lv_drv.h
 *
 *  Created on: Jun 15, 2021
 *      Author: tktof
 */

#ifndef LV_DRV_H_
#define LV_DRV_H_

#include "../lvgl/lvgl.h"

#include "XPT2064.h"

void lvXPT2064_Read(lv_indev_drv_t * drv, lv_indev_data_t*data);

#endif /* LV_DRV_H_ */
