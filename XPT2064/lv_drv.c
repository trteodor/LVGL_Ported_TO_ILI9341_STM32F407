
#include "lv_drv.h"

uint16_t Xread, Yread;

void lvXPT2064_Read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
	if(TouchState==XPT2046_TOUCHED)
	{
		XPT2046_GetTouchPoint(&Xread,&Yread);
		 data->state = LV_INDEV_STATE_PRESSED;
		    data->point.x = Xread;
		    data->point.y = Yread;
	}
	else {
	    data->state = LV_INDEV_STATE_RELEASED;
	  }
}
