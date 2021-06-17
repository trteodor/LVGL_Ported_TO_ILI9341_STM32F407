I'am ported te lvgl 8.0 to STM32F407G + ILI9341 + XPT2064

https://youtu.be/eH6FKjgsQfg   <- Video prestation how to port the libarary on STM32 device or other, it's rly simple!



/*----------------------------------------------------------------------------
 *      Discovery Board - STM32F407G Demo, using littleVgl:                  *
 *                                       									 *
 *       LittleVgl V8.0                       							     *
 *       LL (STM32 Low Level Drivers).         							     *
 *       ILI9341 display over SPI with DMA,                                  *
 *       XPT2046 resistive touch panel    									 *
 *																			 *
 *			POWER:															 *
 *           VCC - VDD                                            			 *
 *           GND - GND                                  					 *
 *           LED - VDD     													 *
 *																			 *	
 *																			 *
 *       ILI9341 SPI1 CONNECTIONS:                                           *
 *       -------------------------------                                     *
 *       TFT_RESET                   PD8                                     *
 *       TFT_DC                      PD9                                     *
 *       TFT_CS                      PB12                                    *
 *       TFT_SCK                     PB13                                    *
 *       TFT_MISO                    PB14                                    *
 *       TFT_MOSI                    PB15                                    *
 *                                                                           *
 *       XPT2046 SPI2 CONNECTIONS:                                           *
 *       --------------------------------                                    *
 *       TOUCH_CLK                   PA5                                     *
 *       TOUCH_DIN (MOSI)            PA7                                     *
 *       TOUCH_DO (MISO)             PA6                                     *
 *       TOUCH_CS                    PC4                                     *
 *       TOUCH_IRQ                   PC5                                     *
 *                                                                           *