I'am ported te lvgl 8.0 to STM32F407G + ILI9341 + XPT2064

* Cube HAL an Low Level Driver (LL Library to drive SPI for optimization) 
* ILI9341 Connected via SPI + DMA , it works in my opinion very well and stable! :) 

https://youtu.be/eH6FKjgsQfg   <- My Video prestation how to port the LVGL libarary on STM32 device or other in Polish league, it's rly simple!

External Links and sources:

https://www.youtube.com/c/LVGL-GUI/featured

LVGL official Site:
  https://lvgl.io/
DataSheet of LVGL
https://docs.lvgl.io/master/index.html

The ILI9341 Driver is Based on:
https://github.com/ScarsFun/lvgl_STM32F103_ILI9341_XPT2046
I modified a large part of this driver

XPT2064 Driver  is taken directly from:
https://kursstm32.pl/

One of examples from the Lib:

![Working](https://github.com/trteodor/LVGL_Ported_TO_ILI9341_STM32F407/blob/master/IMG/DzialajaceKlawiatura.PNG)

Connections:

![Connections](https://github.com/trteodor/LVGL_Ported_TO_ILI9341_STM32F407/blob/master/IMG/Untitled%20Diagram.jpg)
![Visualisation](https://github.com/trteodor/LVGL_Ported_TO_ILI9341_STM32F407/blob/master/IMG/Opis%20Po%C5%82%C4%85cze%C5%84.PNG)


