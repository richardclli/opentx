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

#pragma once

#include "debug.h"
#include "libopenui_defines.h"
#include "colors.h"
#include "board.h"
#include "keys.h"

typedef uint16_t pixel_t;

constexpr bool WRAP_FORM_FIELDS_WITHIN_PAGE = true;
constexpr short SLIDE_SPEED_REDUCTION = 5;

constexpr uint32_t MENU_HEADER_BUTTON_WIDTH =      33;
constexpr uint32_t MENU_HEADER_BUTTONS_LEFT =      47;

constexpr uint32_t MENU_HEADER_HEIGHT =            45;
constexpr uint32_t MENU_TITLE_TOP =                48;
constexpr uint32_t MENU_TITLE_HEIGHT =             21;
constexpr uint32_t MENU_BODY_TOP =                 MENU_TITLE_TOP + MENU_TITLE_HEIGHT;
constexpr uint32_t MENU_CONTENT_TOP =              MENU_BODY_TOP + 1;
constexpr uint32_t MENU_FOOTER_HEIGHT =            0;
constexpr uint32_t MENU_FOOTER_TOP =               LCD_H - MENU_FOOTER_HEIGHT;
constexpr uint32_t MENU_BODY_HEIGHT =              MENU_FOOTER_TOP - MENU_BODY_TOP;
constexpr uint32_t MENUS_MARGIN_LEFT =             6;

constexpr coord_t PAGE_PADDING =                   6;
constexpr uint32_t PAGE_LINE_HEIGHT =              20;
constexpr uint32_t PAGE_LINE_SPACING =             2;
constexpr uint32_t PAGE_INDENT_WIDTH =             10;
constexpr uint32_t PAGE_LABEL_WIDTH =              240;
constexpr uint32_t FH =                            PAGE_LINE_HEIGHT;
constexpr uint32_t NUM_BODY_LINES =                MENU_BODY_HEIGHT / PAGE_LINE_HEIGHT;
constexpr uint32_t TEXT_VIEWER_LINES =             (MENU_FOOTER_TOP - MENU_HEADER_HEIGHT) / FH;

constexpr uint32_t FIELD_PADDING_LEFT =            3;
constexpr uint32_t FIELD_PADDING_TOP =             2;

constexpr uint32_t CURVE_SIDE_WIDTH =              100;
constexpr uint32_t CURVE_CENTER_X =                LCD_W - CURVE_SIDE_WIDTH - 7;
constexpr uint32_t CURVE_CENTER_Y =                151;
constexpr uint32_t CURVE_COORD_WIDTH =             36;
constexpr uint32_t CURVE_COORD_HEIGHT =            17;

constexpr uint32_t DATETIME_SEPARATOR_X =          LCD_W - 53;
constexpr uint32_t DATETIME_LINE1 =                7;
constexpr uint32_t DATETIME_LINE2 =                22;
constexpr uint32_t DATETIME_MIDDLE =               (LCD_W + DATETIME_SEPARATOR_X + 1) / 2;

constexpr uint32_t MENUS_TOOLBAR_BUTTON_WIDTH =    30;
constexpr uint32_t MENUS_TOOLBAR_BUTTON_PADDING =  3;
constexpr rect_t MENUS_TOOLBAR_RECT =              { 100, 51, 30, 209 };

constexpr uint32_t ALERT_FRAME_TOP =               70;
constexpr uint32_t ALERT_FRAME_HEIGHT =            (LCD_H - 2 * ALERT_FRAME_TOP);
constexpr uint32_t ALERT_BITMAP_TOP =              ALERT_FRAME_TOP + 15;
constexpr uint32_t ALERT_BITMAP_LEFT =             40;
constexpr uint32_t ALERT_TITLE_TOP =               ALERT_FRAME_TOP + 10;
constexpr uint32_t ALERT_TITLE_LEFT =              186;
constexpr uint32_t ALERT_TITLE_LINE_HEIGHT =       30;
constexpr uint32_t ALERT_MESSAGE_TOP =             ALERT_TITLE_TOP + 90;
constexpr uint32_t ALERT_MESSAGE_LEFT =            ALERT_TITLE_LEFT;
constexpr uint32_t ALERT_ACTION_TOP =              240;
constexpr uint32_t ALERT_BUTTON_TOP =              300;

constexpr uint32_t PAGE_TITLE_TOP =                2;
constexpr uint32_t PAGE_TITLE_LEFT =               50;

// TODO common code, not in target
enum LcdColorIndex
{
  DEFAULT_COLOR_INDEX,
  DEFAULT_BGCOLOR_INDEX,
  FOCUS_COLOR_INDEX,
  FOCUS_BGCOLOR_INDEX,
  TEXT_STATUSBAR_COLOR_INDEX,
  LINE_COLOR_INDEX,
  MENU_LINE_COLOR_INDEX = LINE_COLOR_INDEX,
  CHECKBOX_COLOR_INDEX,
  SCROLLBAR_COLOR_INDEX,
  MENU_BGCOLOR_INDEX,
  MENU_HIGHLIGHT_BGCOLOR_INDEX = MENU_BGCOLOR_INDEX,
  MENU_COLOR_INDEX,
  MENU_TITLE_DISABLE_COLOR_INDEX,
  HEADER_COLOR_INDEX,
  ALARM_COLOR_INDEX,
  HIGHLIGHT_COLOR_INDEX,
  MENU_HIGHLIGHT_COLOR_INDEX = HIGHLIGHT_COLOR_INDEX,
  TEXT_DISABLE_COLOR_INDEX,
  DISABLE_COLOR_INDEX,
  CURVE_COLOR_INDEX,
  CURVE_AXIS_COLOR_INDEX,
  CURVE_CURSOR_COLOR_INDEX,
  HEADER_BGCOLOR_INDEX,
  HEADER_ICON_BGCOLOR_INDEX,
  HEADER_CURRENT_BGCOLOR_INDEX,
  TITLE_BGCOLOR_INDEX,
  TRIM_BGCOLOR_INDEX,
  TRIM_SHADOW_COLOR_INDEX,
  MAINVIEW_PANES_COLOR_INDEX,
  MAINVIEW_GRAPHICS_COLOR_INDEX,
  OVERLAY_COLOR_INDEX,
  CUSTOM_COLOR_INDEX,
  BARGRAPH1_COLOR_INDEX,
  BARGRAPH2_COLOR_INDEX,
  BARGRAPH_BGCOLOR_INDEX,
  SCROLLBOX_COLOR_INDEX,
  LCD_COLOR_COUNT,

  TABLE_HEADER_BGCOLOR_INDEX,
  TABLE_BGCOLOR_INDEX,
};

enum FontIndex
{
  FONT_STD_INDEX,
#if !defined(BOLD)
  FONT_BOLD_INDEX,
  FONT_XXS_INDEX,
  FONT_XS_INDEX,
  FONT_L_INDEX,
  FONT_XL_INDEX,
  FONT_XXL_INDEX,
#endif
  FONTS_COUNT
};

constexpr uint32_t INPUT_EDIT_LABELS_WIDTH = 120;
constexpr coord_t INPUT_EDIT_CURVE_WIDTH = 158;
constexpr coord_t INPUT_EDIT_CURVE_HEIGHT = LCD_H - MENU_HEADER_HEIGHT;
constexpr coord_t INPUT_EDIT_CURVE_LEFT = LCD_W - 158;
constexpr coord_t INPUT_EDIT_CURVE_TOP = MENU_HEADER_HEIGHT;
constexpr coord_t MENUS_LINE_HEIGHT = 30;
constexpr coord_t MENUS_WIDTH = 200;
constexpr coord_t MENUS_OFFSET_TOP = 20;
constexpr coord_t POPUP_HEADER_HEIGHT = 30;
constexpr coord_t MENUS_MIN_HEIGHT = 2 * MENUS_LINE_HEIGHT - 1;
constexpr coord_t MENUS_MAX_HEIGHT = 7 * MENUS_LINE_HEIGHT - 1;
constexpr coord_t MODEL_SELECT_FOOTER_HEIGHT = 24;
constexpr coord_t SCROLLBAR_WIDTH = 3;
constexpr coord_t TABLE_LINE_HEIGHT = 50;
constexpr coord_t TABLE_HEADER_HEIGHT = 48;

constexpr coord_t ROLLER_LINE_HEIGHT = 40;

constexpr LcdFlags MENU_HEADER_FONT = FONT(BOLD);
constexpr LcdFlags MENU_FONT = FONT(STD);
constexpr LcdFlags TABLE_HEADER_FONT = FONT(STD);
constexpr LcdFlags TABLE_BODY_FONT = FONT(STD);

constexpr int CJK_FIRST_LETTER_INDEX = 128 - 32 + 21;
constexpr coord_t CHAR_SPACING = 0;

#define ROTARY_ENCODER_SPEED() rotencSpeed
