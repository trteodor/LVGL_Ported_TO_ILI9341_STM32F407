/*
 * GFX_EnhancedFonts.c
 *
 *  Created on: Nov 12, 2020
 *      Author: Mateusz Salamon
 */
#include "main.h"
#include "GFX_EnhancedFonts.h"
#include "GFX_Color.h"
#include "../ili9341/core.h"

#include "string.h"

const FONT_INFO *CurrentFont;

void EF_SetFont(const FONT_INFO *Font)
{
	CurrentFont = Font;
}

uint8_t EF_GetFontHeight(void)
{
	return CurrentFont->CharHeight;
}

uint8_t EF_GetFontStartChar(void)
{
	return CurrentFont->StartChar;
}

uint8_t EF_GetFontEndChar(void)
{
	return CurrentFont->EndChar;
}

uint8_t EF_GetFontSpaceWidth(void)
{
	return CurrentFont->SpaceWidth;
}

char EF_DecodePolish(uint8_t chMsb, uint8_t chLsb)
{
	switch(chMsb) // Check first byte
	{
	case 0xC3: // Ó, ó
		switch(chLsb) // Check second byte
		{
		case 0x93: // Ó
			return 0x84;
			break;
		case 0xB3: // ó
			return 0x8D;
			break;
		}
		break;
	case 0xC4: // Ą, Ę, Ć, ą, ę, ć
		switch(chLsb) // Check second byte
		{
		case 0x84: // Ą
			return 0x7F;
			break;
		case 0x98: // Ę
			return 0x81;
			break;
		case 0x86: // Ć
			return 0x80;
			break;
		case 0x85: // ą
			return 0x88;
			break;
		case 0x99: // ę
			return 0x8A;
			break;
		case 0x87: // ć
			return 0x89;
			break;
		}
		break;
	case 0xC5: // Ł, Ń, Ś, Ź, Ż, ł, ń, ś, ź, ż
		switch(chLsb) // Check second byte
		{
		case 0x81: // Ł
			return 0x82;
			break;
		case 0x83: // Ń
			return 0x83;
			break;
		case 0x9A: // Ś
			return 0x85;
			break;
		case 0xB9: // Ź
			return 0x86;
			break;
		case 0xBB: // Ż
			return 0x87;
			break;
		case 0x82: // ł
			return 0x8B;
			break;
		case 0x84: // ń
			return 0x8C;
			break;
		case 0x9B: // ś
			return 0x8E;
			break;
		case 0xBA: // ź
			return 0x8F;
			break;
		case 0xBC: // ż
			return 0x90;
			break;
		}
		break;
	}

	return 0;
}

uint8_t EF_PutChar(char ch, uint16_t PosX, uint16_t PosY, uint16_t Fcolor, BG_FONT Btransparent, uint16_t Bcolor)
{
	uint16_t PixelHeight;
	uint8_t WidthLoops, CharBits, CharBitsLeft;
	uint8_t CharNumber = ch - EF_GetFontStartChar();

	if(ch == ' ') // Check if we have a Space char
	{
		if(Btransparent == BG_COLOR) // If draw BG by color...
		{
			// Draw filled rectangle on Space char
			GFX_DrawFillRectangle(PosX, PosY, SPACE_WIDHT * EF_GetFontSpaceWidth(), CurrentFont->CharHeight,  Bcolor);
		}
		// Return width for String function
		return SPACE_WIDHT * EF_GetFontSpaceWidth();
	}

	// Return if char is not in our font  (smaller than the first)
	if(ch < EF_GetFontStartChar())
	{
		return 0;
	}

	// Return if char is not in our font (bigger than the lastlast)
	if(CharNumber > EF_GetFontEndChar())
	{
		return 0;
	}

	// Get Current char info descriptor - {width, bitmap start byte}
	const FONT_CHAR_INFO *CurrentChar = &CurrentFont->CharDescription[CharNumber];
	// Get bitmap pointer for our char
	uint8_t *CharPointer = (uint8_t*)&CurrentFont->CharBitmaps[CurrentFont->CharDescription[CharNumber].CharOffset];

	// 1st iteration - for each Row by Height
	for(PixelHeight = 0; PixelHeight < CurrentFont->CharHeight; PixelHeight++)
	{
		// Check how many bits left to draw in current Row
		CharBitsLeft = CurrentChar->CharWidth;

		// 2nd Iterator - For each byte in that Row
		for(WidthLoops = 0; WidthLoops <= (CurrentChar->CharWidth-1) / 8; WidthLoops++)
		{
			// Take one line
			uint8_t line = (uint8_t)*CharPointer;
			// Define how many bit to draw - full byte or less
			if(CharBitsLeft >= 8)
			{
				CharBits = 8; // Full byte
				CharBitsLeft -= 8; // Decrease bits left counter
			}
			else
			{
				// If less than byte - only few bits
				CharBits = CharBitsLeft;
			}

			// 3rd Iterator - for each bit in current byte in current row ;)
			for(uint8_t i = 0; i < CharBits; i++, line <<= 1)
			{
				// Check bit first from left
				if(line & 0x80)
				{
					// If pixel exists - draw it
					GFX_DrawPixel(PosX + (WidthLoops*8) + i,  PosY + PixelHeight, Fcolor);
				}
				else if(Btransparent == BG_COLOR) // Check if we want to draw background
				{
					// If no character pixel, then BG pixel
					GFX_DrawPixel(PosX + (WidthLoops*8) + i, PosY + PixelHeight, Bcolor);
				}
			}

			// Take next byte in current char bitmap
			CharPointer++;
		}
	}
	// Return char width for String function
	return CurrentFont->CharDescription[CharNumber].CharWidth;
}

uint8_t EF_PutString(const uint8_t *pStr, uint16_t PosX, uint16_t PosY, uint16_t Fcolor, BG_FONT Btransparent, uint16_t Bcolor)
{
	uint8_t Lenght = strlen((char*)pStr); // Take a string length in bytes

	uint8_t Loop; // Loop Iterator for current char
	uint16_t Shift = 0; // Shift in X-axis for each char drawing
	uint8_t CharWidth; // Current char width
	uint8_t CharToPrint; // Current char to print

	for(Loop = 0; Loop < Lenght; Loop++) // Go threw each char
	{

		if((pStr[Loop] <= 0xC5) && (pStr[Loop] >= 0xC3)) // check if current char is a Polish sign
		{
			// If yes - decode that char
			CharToPrint = EF_DecodePolish(pStr[Loop], pStr[Loop+1]);
			// Skip one byte - Polish chars are 2-bytes
			Loop++;
		}
		else
		{
			// If not polish - just take that char
			CharToPrint = pStr[Loop];
		}

		// Draw char and take its width
		CharWidth = EF_PutChar(CharToPrint, PosX + Shift, PosY, Fcolor, Btransparent, Bcolor);

		// Add last char width to drawing Shift
		Shift = Shift + CharWidth;

		if(Btransparent == BG_COLOR)
		{
			// If draw background - draw the BG color between chars
			GFX_DrawFillRectangle(PosX + Shift, PosY, EF_GetFontSpaceWidth(), EF_GetFontHeight(), Bcolor);
		}

		// Add between-char distance to Shift
		Shift = Shift + EF_GetFontSpaceWidth();
	}
	// Return whole String shift - if you want to use it higher in app.
	return Shift - EF_GetFontSpaceWidth();
}

