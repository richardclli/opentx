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

#ifndef _DRAW_FUNCTIONS_H_
#define _DRAW_FUNCTIONS_H_

#include "dataconstants.h"
#include "telemetry/telemetry_sensors.h"
#include <stdio.h>

#define OPTION_MENU_NO_FOOTER          0x01
#define OPTION_MENU_TITLE_BAR          0x02
#define OPTION_MENU_NO_SCROLLBAR       0x04

#define OPTION_SLIDER_INVERS           INVERS
#define OPTION_SLIDER_BLINK            BLINK
#define OPTION_SLIDER_VERTICAL         0x04
#define OPTION_SLIDER_EMPTY_BAR        0x08
#define OPTION_SLIDER_DBL_COLOR        0x10
#define OPTION_SLIDER_TICKS            0x20
#define OPTION_SLIDER_BIG_TICKS        0x40
#define OPTION_SLIDER_TRIM_BUTTON      0x80
#define OPTION_SLIDER_NUMBER_BUTTON    0x100
#define OPTION_SLIDER_SQUARE_BUTTON    0x200

coord_t drawStringWithIndex(BitmapBuffer * dc, coord_t x, coord_t y, const char * str, int idx, LcdFlags flags = 0, const char * prefix = nullptr, const char * suffix = nullptr);
void drawFatalErrorScreen(const char * message);
void runFatalErrorScreen(const char * message);
void drawPower(coord_t x, coord_t y, int8_t dBm, LcdFlags att);
void drawSource(BitmapBuffer * dc, coord_t x, coord_t y, mixsrc_t idx, LcdFlags flags=0);
coord_t drawSwitch(BitmapBuffer * dc, coord_t x, coord_t y, int32_t idx, LcdFlags flags=0);
void drawTrimMode(BitmapBuffer * dc, coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags flags = 0);
void drawCurveRef(BitmapBuffer * dc, coord_t x, coord_t y, const CurveRef & curve, LcdFlags flags = 0);

#if defined(FLIGHT_MODES)
void drawFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att);
#endif

void drawStatusText(BitmapBuffer * dc, const char * text);
void drawVerticalScrollbar(BitmapBuffer * dc, coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible);
void drawProgressScreen(BitmapBuffer * dc, const char * title, const char * message, int num, int den);
void drawTrimSquare(BitmapBuffer * dc, coord_t x, coord_t y, LcdFlags color = TRIM_BGCOLOR);
void drawHorizontalTrimPosition(BitmapBuffer * dc, coord_t x, coord_t y, int16_t dir);
void drawVerticalTrimPosition(BitmapBuffer * dc, coord_t x, coord_t y, int16_t dir);
void drawVerticalSlider(BitmapBuffer * dc, coord_t x, coord_t y, int len, int val, int min, int max, uint8_t steps, uint32_t options);
void drawHorizontalSlider(BitmapBuffer * dc, coord_t x, coord_t y, int len, int val, int min, int max, uint8_t steps, uint32_t options);
void drawSlider(coord_t x, coord_t y, int len, int val, int min, int max, uint8_t steps, uint32_t options);
void drawValueOrGVar(BitmapBuffer * dc, coord_t x, coord_t y, gvar_t value, gvar_t vmin, gvar_t vmax, LcdFlags flags = 0);
void drawGVarValue(BitmapBuffer * dc, coord_t x, coord_t y, uint8_t gvar, gvar_t value, LcdFlags flags = 0);
void drawTimer(BitmapBuffer * dc, coord_t x, coord_t y, int32_t tme, LcdFlags flags = 0);
void drawSourceValue(BitmapBuffer * dc, coord_t x, coord_t y, source_t source, LcdFlags flags = 0);
void drawSourceCustomValue(BitmapBuffer * dc, coord_t x, coord_t y, source_t source, int32_t value, LcdFlags flags);
void drawSensorCustomValue(BitmapBuffer * dc, coord_t x, coord_t y, uint8_t sensor, int32_t value, LcdFlags flags = 0);
void drawGPSPosition(BitmapBuffer * dc, coord_t x, coord_t y, int32_t longitude, int32_t latitude, LcdFlags flags = 0);
void drawDate(BitmapBuffer * dc, coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags flags = 0);
void drawValueWithUnit(BitmapBuffer * dc, coord_t x, coord_t y, int val, uint8_t unit, LcdFlags flags = 0);
void drawHexNumber(BitmapBuffer * dc, coord_t x, coord_t y, uint32_t val, LcdFlags flags = 0);
inline void drawChn(BitmapBuffer * dc, coord_t x, coord_t y, uint8_t idx, LcdFlags flags)
{
  drawSource(dc, x, y, MIXSRC_CH1 + idx - 1, flags);
}

// Screen templates
void drawSplash();
void drawSleepBitmap();
void drawShutdownAnimation(uint32_t duration, uint32_t totalDuration, const char * message);

// Main view standard widgets
void drawMainPots();
void drawTrims(uint8_t flightMode);

#endif // _DRAW_FUNCTIONS_H_
