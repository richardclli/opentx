/*
 * Copyright (C) OpenTX
 *
 * Source:
 *  https://github.com/opentx/libopenui
 *
 * This file is a part of libopenui library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#include "message_dialog.h"
#include "static.h"

MessageDialog::MessageDialog(Window * parent, const char * title, const char * message, const char * info):
  Dialog(parent, title, {50, 73, LCD_W - 100, LCD_H - 146})
{
  messageWidget = new StaticText(this, {0, coord_t(height() - PAGE_LINE_HEIGHT) / 2, width(), PAGE_LINE_HEIGHT}, message, 0, CENTERED);
  infoWidget = new StaticText(this, {0, 30 + coord_t(height() - PAGE_LINE_HEIGHT) / 2, width(), PAGE_LINE_HEIGHT}, info, 0, CENTERED);
}

#if defined(HARDWARE_KEYS)
void MessageDialog::onEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  if (event == EVT_KEY_BREAK(KEY_EXIT) || event == EVT_KEY_BREAK(KEY_ENTER)) {
    deleteLater();
  }
}
#endif
