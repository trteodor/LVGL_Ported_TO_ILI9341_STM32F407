#include "core.h"
#include "lv_driver.h"
#include "stdlib.h"
//#include <stdio.h>

static lv_disp_drv_t *LastDriver;
uint16_t tbuf[100];

void ILI9341_SetAddrWindowForLv(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
			//I Create else 1 function to SetAddrWindow because LvGL calling the function
					//with other parametrs. Similar function is implemented in ../ILI9341/core.c
{
	uint8_t DataToTransfer[4];
	// Calculate end ranges
	// Fulfill X's buffer
	DataToTransfer[0] = x1 >> 8;
	DataToTransfer[1] = x1 & 0xFF;
	DataToTransfer[2] = x2 >> 8;
	DataToTransfer[3] = x2 & 0xFF;
	// Push X's buffer
	ILI9341_SendCommandAndData(ILI9341_CASET, DataToTransfer, 4);
	// Fulfill Y's buffer
	DataToTransfer[0] = y1 >> 8;
	DataToTransfer[1] = y1 & 0xFF;
	DataToTransfer[2] = y2 >> 8;
	DataToTransfer[3] = y2 & 0xFF;
	// Push Y's buffer
	ILI9341_SendCommandAndData(ILI9341_PASET, DataToTransfer, 4);
}

// void ILI9341_flush(struct _disp_drv_t * drv, const lv_area_t * area,  lv_color_t * color_map)
void ILI9341_flush(lv_disp_drv_t * drv, const lv_area_t * area,  lv_color_t * color_map)
{
	LastDriver=drv;
	/*ILI9341_SetAddrWindow((uint16_t)area->x1, (uint16_t)area->y1,
							(uint16_t)area->x2-(uint16_t)area->x1 +1, (uint16_t)area->y2-(uint16_t)area->y1 +1);*/
					//Bc my SetAddrWindow function accepts argument x,y,w,h so i fitted into it
												//Buy at the expense of performance
														//Use It to safe some flash memory
	ILI9341_SetAddrWindowForLv((uint16_t)area->x1, (uint16_t)area->y1, (uint16_t)area->x2, (uint16_t)area->y2);
	ILI9341_SendCommand(ILI9341_RAMWR);
	uint32_t size = (area->x2 - area->x1 +1) * (area->y2 - area->y1 +1);
	Send_DMA_Data16( (uint16_t *)color_map, size);
}
void DMA_ILI9341_SPI_TransmitComplete_Callback()
{
	lv_disp_flush_ready(LastDriver);
}






