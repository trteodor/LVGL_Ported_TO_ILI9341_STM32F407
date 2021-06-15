#include "core.h"
#include "lv_driver.h"
#include "stdlib.h"
//#include <stdio.h>

static lv_disp_drv_t *LastDriver;
uint16_t tbuf[100];

// void ILI9341_flush(struct _disp_drv_t * drv, const lv_area_t * area,  lv_color_t * color_map)
void ILI9341_flush(lv_disp_drv_t * drv, const lv_area_t * area,  lv_color_t * color_map)
{
	LastDriver=drv;
	ILI9341_SetAddrWindow((uint16_t)area->x1, (uint16_t)area->y1, (uint16_t)area->x2 +1 , (uint16_t)area->y2 +1);
	ILI9341_SendCommand(ILI9341_RAMWR);  //Ta komenda przed zapisem!!!
	uint32_t size = (area->x2 - area->x1 +1) * (area->y2 - area->y1 +1);
	Send_DMA_Data16( (uint16_t *)color_map, size);
}
void DMA_ILI9341_SPI_TransmitComplete_Callback()
{
	lv_disp_flush_ready(LastDriver);
}






