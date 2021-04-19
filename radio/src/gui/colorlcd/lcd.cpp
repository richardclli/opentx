/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "lcd.h"
#include "opentx.h"

#if defined(SIMU)
pixel_t displayBuf[DISPLAY_BUFFER_SIZE];
#endif

uint8_t getMappedChar(uint8_t c)
{
  uint8_t result;

  result = c - 0x20;

//  TRACE("getMappedChar '%c' (0x%x) = %d", c, c, result);

  return result;
}

int getFontPatternWidth(const uint16_t * spec, int index)
{
  return spec[index + 2] - spec[index + 1];
}

int getCharWidth(uint8_t c, const uint16_t * spec)
{
  return getFontPatternWidth(spec, getMappedChar(c));
}

uint8_t getFontHeight(LcdFlags flags)
{
  uint32_t fontindex = FONT_INDEX(flags);
  return fontspecsTable[fontindex][0];
}

int getTextWidth(const char * s, int len, LcdFlags flags)
{
  const uint16_t * specs = fontspecsTable[FONT_INDEX(flags)];

  int result = 0;
  for (int i = 0; len == 0 || i < len; ++i) {
    unsigned c = uint8_t(*s);
    if (!c) {
      break;
    }
    else if (c >= 0xFE) {
      s++;
      c = uint8_t(*s) + ((c & 0x01) << 8) - 1;
      if (c >= 0x101)
        c -= 1;
      c += CJK_FIRST_LETTER_INDEX;
      result += getFontPatternWidth(specs, c) + 1;
    }
    else if (c >= 0x20) {
      result += getCharWidth(c, specs);
    }

    ++s;
  }

  return result;
}

void lcdSetContrast()
{
  lcdSetRefVolt(g_eeGeneral.contrast);
}

#if defined(SIMU)
BitmapBuffer _lcd(BMP_RGB565, LCD_W, LCD_H, displayBuf);
BitmapBuffer * lcd = &_lcd;

void DMAFillRect(uint16_t * dest, uint16_t destw, uint16_t desth, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
#if defined(LCD_VERTICAL_INVERT)
  x = destw - (x + w);
  y = desth - (y + h);
#endif

  for (int i=0; i<h; i++) {
    for (int j=0; j<w; j++) {
      dest[(y+i)*destw+x+j] = color;
    }
  }
}

void DMACopyBitmap(uint16_t * dest, uint16_t destw, uint16_t desth, uint16_t x, uint16_t y, const uint16_t * src, uint16_t srcw, uint16_t srch, uint16_t srcx, uint16_t srcy, uint16_t w, uint16_t h)
{
#if defined(LCD_VERTICAL_INVERT)
  x = destw - (x + w);
  y = desth - (y + h);
  srcx = srcw - (srcx + w);
  srcy = srch - (srcy + h);
#endif

  for (int i=0; i<h; i++) {
    memcpy(dest+(y+i)*destw+x, src+(srcy+i)*srcw+srcx, 2*w);
  }
}

void DMACopyAlphaBitmap(uint16_t * dest, uint16_t destw, uint16_t desth, uint16_t x, uint16_t y, const uint16_t * src, uint16_t srcw, uint16_t srch, uint16_t srcx, uint16_t srcy, uint16_t w, uint16_t h)
{
#if defined(LCD_VERTICAL_INVERT)
  x = destw - (x + w);
  y = desth - (y + h);
  srcx = srcw - (srcx + w);
  srcy = srch - (srcy + h);
#endif

  for (coord_t line=0; line<h; line++) {
    uint16_t * p = dest + (y+line)*destw + x;
    const uint16_t * q = src + (srcy+line)*srcw + srcx;
    for (coord_t col=0; col<w; col++) {
      uint8_t alpha = *q >> 12;
      uint8_t red = ((((*q >> 8) & 0x0f) << 1) * alpha + (*p >> 11) * (0x0f-alpha)) / 0x0f;
      uint8_t green = ((((*q >> 4) & 0x0f) << 2) * alpha + ((*p >> 5) & 0x3f) * (0x0f-alpha)) / 0x0f;
      uint8_t blue = ((((*q >> 0) & 0x0f) << 1) * alpha + ((*p >> 0) & 0x1f) * (0x0f-alpha)) / 0x0f;
      *p = (red << 11) + (green << 5) + (blue << 0);
      p++; q++;
    }
  }
}

void DMABitmapConvert(uint16_t * dest, const uint8_t * src, uint16_t w, uint16_t h, uint32_t format)
{
  if (format == DMA2D_ARGB4444) {
    for (int row = 0; row < h; ++row) {
      for (int col = 0; col < w; ++col) {
        *dest = ARGB(src[0], src[1], src[2], src[3]);
        ++dest;
        src += 4;
      }
    }
  }
  else {
    for (int row = 0; row < h; ++row) {
      for(int col = 0; col < w; ++col) {
        *dest = RGB(src[1], src[2], src[3]);
        ++dest;
        src += 4;
      }
    }
  }
}
#endif

event_t getWindowEvent()
{
  return getEvent(false);
}
