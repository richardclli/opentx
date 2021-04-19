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

#include "opentx.h"

const uint16_t font_xxs_specs[] = {
#include "font_9.specs"
};

const unsigned char font_xxs[] = {
#include "font_9.lbm"
};

#if LCD_H > 272
const uint16_t font_std_en_specs[] = {
#include "font_17en.specs"
};

const unsigned char font_std_en[] = {
#include "font_17en.lbm"
};

const uint16_t font_xs_specs[] = {
#include "font_15.specs"
};

const unsigned char font_xs[] = {
#include "font_15.lbm"
};

const uint16_t font_std_specs[] = {
#include "font_17.specs"
};

const unsigned char font_std[] = {
#include "font_17.lbm"
};

const uint16_t font_bold_specs[] = {
#include "font_bold17.specs"
};

const unsigned char font_bold[] = {
#include "font_bold17.lbm"
};
#else
const uint16_t font_std_en_specs[] = {
#include "font_16en.specs"
};

const unsigned char font_std_en[] = {
#include "font_16en.lbm"
};

const uint16_t font_xs_specs[] = {
#include "font_13.specs"
};

const unsigned char font_xs[] = {
#include "font_13.lbm"
};

const uint16_t font_std_specs[] = {
#include "font_16.specs"
};

const unsigned char font_std[] = {
#include "font_16.lbm"
};

const uint16_t font_bold_specs[] = {
#include "font_bold16.specs"
};

const unsigned char font_bold[] = {
#include "font_bold16.lbm"
};
#endif

const uint16_t font_l_specs[] = {
#include "font_24.specs"
};

const unsigned char font_l[] = {
#include "font_24.lbm"
};

const uint16_t font_xl_specs[] = {
#include "font_32.specs"
};

const unsigned char font_xl[] = {
#include "font_32.lbm"
};

const uint16_t font_xxl_specs[] = {
#include "font_64.specs"
};

const unsigned char font_xxl[] = {
#include "font_64.lbm"
};

#if defined(BOOT)
const uint16_t * const fontspecsTable[FONTS_COUNT] = { font_std_en_specs };
const uint8_t * fontsTable[FONTS_COUNT] = { font_std_en };
#else
const uint16_t * const fontspecsTable[FONTS_COUNT] = { font_std_specs, font_bold_specs, font_xxs_specs, font_xs_specs, font_l_specs, font_xl_specs, font_xxl_specs };
const uint8_t * fontsTable[FONTS_COUNT] = { font_std, font_bold, font_xxs, font_xs, font_l, font_xl, font_xxl };
#endif

uint8_t * decompressFont(const uint8_t * font)
{
  uint16_t width = *((uint16_t *)font);
  uint16_t height = *(((uint16_t *)font)+1);

  size_t font_size = width * height;
  uint8_t * dec_buf = (uint8_t *)malloc(font_size + 4);

  // copy width / height
  memcpy(dec_buf, font,4);

  RLEBitmap::decode(dec_buf+4, font_size, font+4);
  return dec_buf;
}

void loadFonts()
{
  static bool fontsLoaded = false;
  if (fontsLoaded)
    return;

#if defined(BOOT)
  fontsTable[0] = decompressFont(fontsTable[0]);
#else
  for (int i = 0; i < FONTS_COUNT; i++) {
    fontsTable[i] = decompressFont(fontsTable[i]);
  }
#endif

  fontsLoaded = true;
}
