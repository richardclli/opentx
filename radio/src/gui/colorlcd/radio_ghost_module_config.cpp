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
#include "radio_ghost_module_config.h"

class GhostModuleConfigWindow: public Window
{
  public:
    GhostModuleConfigWindow(Window * parent, const rect_t & rect) :
    Window(parent, rect, FORWARD_SCROLL | FORM_FORWARD_FOCUS | REFRESH_ALWAYS)
    {
      setFocus(SET_FOCUS_DEFAULT);
    }

    void paint(BitmapBuffer * dc) override
    {
      constexpr coord_t xOffset = 140;
      constexpr coord_t xOffset2 = 260;
      constexpr coord_t yOffset = 20;
      constexpr coord_t lineSpacing = 25;

      for (uint8_t line = 0; line < GHST_MENU_LINES; line++) {
        if (reusableBuffer.ghostMenu.line[line].splitLine) {
          if (reusableBuffer.ghostMenu.line[line].lineFlags & GHST_LINE_FLAGS_LABEL_SELECT) {
            dc->drawSolidFilledRect(xOffset, yOffset + line * lineSpacing, getTextWidth(reusableBuffer.ghostMenu.line[line].menuText, 0,FONT(L)), getFontHeight(FONT(L)), FONT(L) | FOCUS_BGCOLOR);
            dc->drawText(xOffset, yOffset + line * lineSpacing, reusableBuffer.ghostMenu.line[line].menuText, FONT(L) | DEFAULT_BGCOLOR);
          }
          else {
            dc->drawText(xOffset, yOffset + line * lineSpacing, reusableBuffer.ghostMenu.line[line].menuText, FONT(L));
          }

          if (reusableBuffer.ghostMenu.line[line].lineFlags & GHST_LINE_FLAGS_VALUE_SELECT) {
            dc->drawSolidFilledRect(xOffset, yOffset + line * lineSpacing, getTextWidth( &reusableBuffer.ghostMenu.line[line].menuText[reusableBuffer.ghostMenu.line[line].splitLine], 0,FONT(L)), getFontHeight(0), FOCUS_BGCOLOR);
            dc->drawText(xOffset, yOffset + line * lineSpacing,  &reusableBuffer.ghostMenu.line[line].menuText[reusableBuffer.ghostMenu.line[line].splitLine], FONT(L) | DEFAULT_BGCOLOR);
          }
          else {
            dc->drawText(xOffset2, yOffset + line * lineSpacing, &reusableBuffer.ghostMenu.line[line].menuText[reusableBuffer.ghostMenu.line[line].splitLine], FONT(L) | DEFAULT_COLOR);
          }
        }
        else {
          if (reusableBuffer.ghostMenu.line[line].lineFlags & GHST_LINE_FLAGS_LABEL_SELECT) {
            dc->drawSolidFilledRect(xOffset, yOffset + line * lineSpacing, getTextWidth(reusableBuffer.ghostMenu.line[line].menuText, 0, FONT(L)), getFontHeight(FONT(L)), FOCUS_BGCOLOR);
            dc->drawText(xOffset, yOffset + line * lineSpacing, reusableBuffer.ghostMenu.line[line].menuText, FONT(L) | DEFAULT_BGCOLOR);
          }
          else if (reusableBuffer.ghostMenu.line[line].lineFlags & GHST_LINE_FLAGS_VALUE_EDIT) {
            if (BLINK_ON_PHASE) {
              dc->drawText(xOffset, yOffset + line * lineSpacing, reusableBuffer.ghostMenu.line[line].menuText, FONT(L));
            }
          }
          else {
            dc->drawText(xOffset, yOffset + line * lineSpacing, reusableBuffer.ghostMenu.line[line].menuText, FONT(L) | DEFAULT_COLOR);
          }
        }
      }
    }
  protected:
};

RadioGhostModuleConfig::RadioGhostModuleConfig(uint8_t moduleIdx) :
  Page(ICON_RADIO_TOOLS),
  moduleIdx(moduleIdx)
{
  init();
  buildHeader(&header);
  buildBody(&body);
  setFocus(SET_FOCUS_DEFAULT);
}

void RadioGhostModuleConfig::buildHeader(Window * window)
{
  new StaticText(window, {PAGE_TITLE_LEFT, PAGE_TITLE_TOP + 10, LCD_W - PAGE_TITLE_LEFT, PAGE_LINE_HEIGHT}, "GHOST MODULE", 0, MENU_COLOR);
}

void RadioGhostModuleConfig::buildBody(FormWindow * window)
{
  new GhostModuleConfigWindow(window, {0, 0, LCD_W, LCD_H - MENU_HEADER_HEIGHT - 5});
}

#if defined(HARDWARE_KEYS)
void RadioGhostModuleConfig::onEvent(event_t event)
{
  switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_LEFT:
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_JOYUP;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_NONE;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      break;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_RIGHT:
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_JOYDOWN;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_NONE;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      break;
#endif

    case EVT_KEY_FIRST(KEY_ENTER):
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_JOYPRESS;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_NONE;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      break;

    case EVT_KEY_BREAK(KEY_EXIT):
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_JOYLEFT;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_NONE;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      break;

    case EVT_KEY_LONG(KEY_EXIT):
      menuVerticalOffset = 0;
      memclear(&reusableBuffer.ghostMenu, sizeof(reusableBuffer.ghostMenu));
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_NONE;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_CLOSE;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      RTOS_WAIT_MS(10);
      deleteLater();
      break;
  }
  Page::onEvent(event);
}
#endif

void RadioGhostModuleConfig::init()
{
  memclear(&reusableBuffer.ghostMenu, sizeof(reusableBuffer.ghostMenu));
  reusableBuffer.ghostMenu.buttonAction = GHST_BTN_NONE;
  reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_OPEN;
  moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
}

