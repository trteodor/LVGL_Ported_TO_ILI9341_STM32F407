/*
 * TFT_ILI9341.c
 *
 *  Created on: Oct 30, 2020
 *      Author: Mateusz Salamon
 */
#include "main.h"
#include "core.h"


volatile uint32_t flag_DMA_Stream_bsy;
//
// Rotation stuff
//
#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

static const uint8_t initcmd[] = {
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0x12, 0x81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
  ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
  ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
  ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
  ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
  ILI9341_PIXFMT  , 1, 0x55,
  ILI9341_FRMCTR1 , 2, 0x00, 0x18,
  ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
  0xF2, 1, 0x00,                         // 3Gamma Function Disable
  ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
  ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  ILI9341_SLPOUT  , 0x80,                // Exit Sleep
  ILI9341_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};

//
//  Basic function - write those for your MCU
//
static void ILI9341_Delay(uint32_t ms)
{
	HAL_Delay(ms);
}

void Send_DMA_Data8(uint16_t* buff, uint16_t dataSize)
{
	LL_SPI_Disable(SPI2);



	 LL_DMA_DisableStream(DMA1,LL_DMA_STREAM_4);
	 LL_DMA_ConfigTransfer(DMA1,
	                        LL_DMA_STREAM_4,
	                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_HIGH | LL_DMA_MODE_NORMAL |
	                        LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT |
							LL_DMA_PDATAALIGN_BYTE | LL_DMA_MDATAALIGN_BYTE);
	  LL_DMA_ConfigAddresses(DMA1,
	                         LL_DMA_STREAM_4,
	                         (uint32_t)buff, LL_SPI_DMA_GetRegAddr(SPI2),
	                         LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_4));

	  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_4, dataSize);

	  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_4);
	  LL_SPI_EnableDMAReq_TX(SPI2);
	  LL_SPI_Enable(SPI2);
	  LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_4);
	  flag_DMA_Stream_bsy=1;
}

void Send_DMA_Data16(uint16_t* buff, uint16_t dataSize)
{
	LL_SPI_Disable(SPI2);
	LL_SPI_SetDataWidth(SPI2, LL_SPI_DATAWIDTH_16BIT);

	  ILI9341_DC_HIGH;
	  ILI9341_CS_LOW;

	 LL_DMA_DisableStream(DMA1,LL_DMA_STREAM_4);
	 LL_DMA_ConfigTransfer(DMA1,
	                        LL_DMA_STREAM_4,
	                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_HIGH | LL_DMA_MODE_NORMAL |
	                        LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT |
							LL_DMA_PDATAALIGN_HALFWORD | LL_DMA_MDATAALIGN_HALFWORD);
	  LL_DMA_ConfigAddresses(DMA1,
	                         LL_DMA_STREAM_4,
	                         (uint32_t)buff, LL_SPI_DMA_GetRegAddr(SPI2),
	                         LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_4));

	  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_4, dataSize);

	  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_4);
	  LL_SPI_EnableDMAReq_TX(SPI2);
	  LL_SPI_Enable(SPI2);
	  LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_4);
	  flag_DMA_Stream_bsy=1;
}

void DMA1_Str4_TransmitComplete_Callback()
{

	DMA_ILI9341_SPI_TransmitComplete_Callback();  //file lv_driver.c

	 LL_SPI_SetDataWidth(SPI2, LL_SPI_DATAWIDTH_8BIT);
	ILI9341_CS_HIGH;
	LL_DMA_ClearFlag_TC4(DMA1);
	flag_DMA_Stream_bsy=0;
}


void Send_Data8(uint8_t data)
{
    while(!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
    LL_SPI_TransmitData8 (SPI2, data);
		//while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}
		(void) SPI2->DR; //fake Rx read;
    while (LL_SPI_IsActiveFlag_BSY(SPI2)){}
}

static void ILI9341_SendToTFT(uint8_t *Byte, uint32_t Length)
{
		ILI9341_CS_LOW;
	    for(uint16_t index =0; index<Length; index++)
	    {
	        Send_Data8( Byte[index]);
	    }
	    ILI9341_CS_HIGH;
}

void ILI9341_SendCommand(uint8_t Command)
{
	// CS Low
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_LOW;
#endif
	// DC to Command - DC to Low
	ILI9341_DC_LOW;

	// Send to TFT 1 byte
	ILI9341_SendToTFT(&Command, 1);
	// CS High
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_HIGH;
#endif
}

void ILI9341_SendCommandAndData(uint8_t Command, uint8_t *Data, uint32_t Length)
{
	// CS Low
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_LOW;
#endif
	// DC to Command - DC to Low
	ILI9341_DC_LOW;
	// Send to TFT 1 byte
	ILI9341_SendToTFT(&Command, 1);

	// DC to Data - DC to High
	ILI9341_DC_HIGH;
	// Send to TFT Length byte
	ILI9341_SendToTFT(Data, Length);

	// CS High
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_HIGH;
#endif
}
#if (ILI9341_OPTIMIZE_HAL_SP1 == 0)
void ILI9341_SendData16(uint16_t Data)
{
#if (ILI9341_USE_CS == 1)
	ILI9341_CS_LOW;
#endif

	uint8_t tmp[2];
	tmp[0] = (Data>>8);
	tmp[1] = Data & 0xFF;

	ILI9341_DC_HIGH;	// Data mode
	ILI9341_SendToTFT(tmp, 2);

#if (ILI9341_USE_CS == 1)
	ILI9341_CS_HIGH;
#endif
}
#endif

//
// TFT Functions
//
void ILI9341_SetRotation(uint8_t Rotation)
{
	if(Rotation > 3)
		return;

	//
	// Set appropriate bits for Rotation
	//
	switch(Rotation)
	{
	case 0:
		Rotation = (MADCTL_MX | MADCTL_BGR);
		break;
	case 1:
		Rotation = (MADCTL_MV | MADCTL_BGR);
		break;
	case 2:
		Rotation = (MADCTL_MY | MADCTL_BGR);
		break;
	case 3:
		Rotation = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		break;
	}

	// Write indo MAD Control register our Rotation data
	ILI9341_SendCommandAndData(ILI9341_MADCTL, &Rotation, 1);
}

void ILI9341_SetAddrWindow(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h)
{
	uint8_t DataToTransfer[4];
	// Calculate end ranges
	uint16_t x2 = (x1 + w - 1), y2 = (y1 + h - 1);

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

void ILI9341_WritePixel(int16_t x, int16_t y, uint16_t color)
{
	uint8_t DataToTransfer[2];

	if ((x >= 0) && (x < ILI9341_TFTWIDTH) && (y >= 0) && (y < ILI9341_TFTHEIGHT))
	{
		// Set Window for 1x1 pixel
		ILI9341_SetAddrWindow(x, y, 1, 1);

		// Fulfill buffer with color
		DataToTransfer[0] = color >> 8;
		DataToTransfer[1] = color & 0xFF;
		// Push color bytes to RAM
		ILI9341_SendCommandAndData(ILI9341_RAMWR, DataToTransfer, 2);
	}
}

void ILI9341_DrawImage(uint16_t x, uint16_t y, const uint8_t *img, uint16_t w, uint16_t h)
{
	// Check if image will fit into screen - cannot make it outside by hardware
	if ((x >= 0) && ((x + w) <= ILI9341_TFTWIDTH) && (y >= 0) && ((y + h) <= ILI9341_TFTHEIGHT))
	{
		// Set window for image
		ILI9341_SetAddrWindow(x, y, w, h);
		// Push image to RAM
		ILI9341_SendCommandAndData(ILI9341_RAMWR, (uint8_t *)img, (w*h*2));
	}
}

void ILI9341_ClearDisplay(uint16_t Color)
{
	// Set window for whole screen
	ILI9341_SetAddrWindow(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT);
	// Set RAM writing
	ILI9341_SendCommand(ILI9341_RAMWR);

	for(uint32_t i = 0; i < (ILI9341_TFTWIDTH * ILI9341_TFTHEIGHT); i++)
	{
		// Send Color bytes
		ILI9341_SendData16(Color);
	}
}

void ILI9341_fillRect(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t color) {
    uint16_t tbuf[w*h];
    ILI9341_SetAddrWindow(x1, y1,w,h);
	ILI9341_SendCommand(ILI9341_RAMWR);  //Ta komenda przed zapisem!!!

	uint16_t tempi=0;
		for(tempi=0; tempi < (w*h); tempi++)
		{
			tbuf[tempi]=color;
		}
        Send_DMA_Data16(tbuf,(w*h));
}




void ILI9341_Init(SPI_HandleTypeDef *hspi)
{

    uint8_t cmd, x, numArgs;
    const uint8_t *addr = initcmd;

	  LL_SPI_Enable(SPI2);


#if (ILI9341_USE_HW_RESET == 1)
	ILI9341_RST_LOW;
	ILI9341_Delay(10);
	ILI9341_RST_HIGH;
	ILI9341_Delay(10);
#else
	ILI9341_SendCommand(ILI9341_SWRESET); // Engage software reset
    ILI9341_Delay(150);
#endif

    while ((cmd = *(addr++)) > 0)
    {
      x = *(addr++);
      numArgs = x & 0x7F;
      // Push Init data
      ILI9341_SendCommandAndData(cmd, (uint8_t *)addr, numArgs);

      addr += numArgs;

      if (x & 0x80)
      {
    	  ILI9341_Delay(150);
      }
    }

    // Set selected Rotation
    ILI9341_SetRotation(ILI9341_ROTATION);
}






