/*
 * XPT2064.h
 *
 *  Created on: Nov 13, 2020
 *      Author: Mateusz Salamon
 */

#ifndef INC_XPT2064_H_
#define INC_XPT2064_H_

#define XPT2046_USE_CS 1
#define XPT2046_SPI_TIMEOUT 1000

#define TOUCH_ROTATION	ILI9341_ROTATION
#define DISPLAY_HEIGHT	ILI9341_TFTHEIGHT
#define DISPLAY_WIDTH	ILI9341_TFTWIDTH

void XPT2046_Init(SPI_HandleTypeDef *hspi, IRQn_Type TouchIRQn);

void XPT2046_Task(void);
void XPT2046_GetTouchPoint(uint16_t *X, uint16_t *Y);
uint8_t XPT2046_IsTouched(void);
void XPT2046_IRQ(void);

void DoCalibration(void);

#endif /* INC_XPT2064_H_ */
