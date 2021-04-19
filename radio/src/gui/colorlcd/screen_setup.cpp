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

#include <algorithm>
#include "screen_setup.h"
#include "opentx.h"
#include "view_main.h"
#include "widget_settings.h"

#define SET_DIRTY()     storageDirty(EE_MODEL)

ScreenUserInterfacePage::ScreenUserInterfacePage():
  PageTab(STR_USER_INTERFACE, ICON_THEME_SETUP)
{
}

void ScreenUserInterfacePage::build(FormWindow * window)
{
  FormGridLayout grid;

  // Theme choice
  new StaticText(window, grid.getLabelSlot(), STR_THEME, 0, 0);
  // TODO: Theme picklist
  grid.nextLine();

  // Theme options ?

  // Top Bar
  new StaticText(window, grid.getLabelSlot(), STR_TOP_BAR, 0, 0);
  // TODO: enable settings topbar widgets
  grid.nextLine();
}

ScreenAddPage::ScreenAddPage(ScreenMenu * menu, uint8_t pageIndex):
  PageTab(),
  menu(menu),
  pageIndex(pageIndex)
{
  setTitle(STR_ADD_MAIN_VIEW);
  setIcon(ICON_THEME_ADD_VIEW);
}

void ScreenAddPage::build(FormWindow * window)
{
  rect_t buttonRect = {LCD_W / 2 - 100, (window->height() - 24) / 2, 200, 24};
  auto button = new TextButton(window, buttonRect, STR_ADD_MAIN_VIEW);
  button->setPressHandler([=]() -> uint8_t {

      auto& screen     = customScreens[pageIndex];
      auto& screenData = g_model.screenData[pageIndex];

      const LayoutFactory * factory = getRegisteredLayouts().front();

      screen = factory->create(&screenData.layoutData);
      strncpy(screenData.LayoutId, factory->getId(), sizeof(screenData.LayoutId));

      auto tab = new ScreenSetupPage(menu, screen, screenData);
      std::string title(STR_MAIN_VIEW_X);
      title.back() = pageIndex + '0';      
      tab->setTitle(title);
      tab->setIcon(ICON_THEME_VIEW1 + pageIndex);
      
      menu->removeTab(pageIndex);
      menu->addTab(tab);
      menu->setCurrentTab(pageIndex);

      if (pageIndex < MAX_CUSTOM_SCREENS - 1) {
        menu->addTab(new ScreenAddPage(menu, pageIndex + 1));
      }
      storageDirty(EE_MODEL);
      return 0;
  });
}

class LayoutChoice: public FormField {
  public:
    LayoutChoice(Window * parent, const rect_t & rect, std::function<const LayoutFactory *()> getValue, std::function<void(const LayoutFactory *)> setValue):
      FormField(parent, {rect.x, rect.y, 59, 33}),
      getValue(std::move(getValue)),
      setValue(std::move(setValue))
    {
    }

    void paint(BitmapBuffer * dc) override
    {
      FormField::paint(dc);
      auto layout = getValue();
      layout->drawThumb(dc, 4, 4, editMode ? FOCUS_COLOR : LINE_COLOR);
    }

#if defined(HARDWARE_KEYS)
    void onEvent(event_t event) override
    {
      TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

      if (event == EVT_KEY_BREAK(KEY_ENTER)) {
        editMode = true;
        invalidate();
        openMenu();
      }
      else {
        FormField::onEvent(event);
      }
    }
#endif

#if defined(HARDWARE_TOUCH)
    bool onTouchEnd(coord_t, coord_t) override
    {
      if (enabled) {
        if (!hasFocus()) {
          setFocus(SET_FOCUS_DEFAULT);
        }
        onKeyPress();
        openMenu();
      }
      return true;
    }
#endif

    void openMenu()
    {
      auto menu = new Menu(parent);

      for (auto layout: getRegisteredLayouts()) {
        menu->addCustomLine([=](BitmapBuffer * dc, coord_t x, coord_t y, LcdFlags flags) {
            layout->drawThumb(dc, x + 2, y + 2, flags);
            dc->drawText(65, y + 5, layout->getName(), flags);
        }, [=]() {
            setValue(layout);
        });
      }

      auto it = std::find(getRegisteredLayouts().begin(), getRegisteredLayouts().end(), getValue());
      menu->select(std::distance(getRegisteredLayouts().begin(), it));

      menu->setCloseHandler([=]() {
          editMode = false;
          setFocus(SET_FOCUS_DEFAULT);
      });
    }

  protected:
    std::function<const LayoutFactory *()> getValue;
    std::function<void(const LayoutFactory *)> setValue;
};

ScreenSetupPage::ScreenSetupPage(ScreenMenu * menu, Layout*& screen, CustomScreenData& screenData):
  PageTab(),
  menu(menu),
  screen(screen),
  screenData(screenData)
{
}

class SetupWidgetsPageSlot: public Button
{
  public:
    SetupWidgetsPageSlot(FormGroup * parent, const rect_t & rect, Layout* screen, uint8_t slotIndex):
      Button(parent, rect),
      screen(screen),
      slotIndex(slotIndex)
    {
      widget = screen->getWidget(slotIndex);
      setPressHandler([=]() -> uint8_t {
          if (widget) {
            Menu * menu = new Menu(this);
            menu->addLine(TR_SELECT_WIDGET, [=]() {
                Menu * menu = new Menu(this);
                for (auto factory: getRegisteredWidgets()) {
                  menu->addLine(factory->getName(), [=]() {
                      if (widget) widget->deleteLater();
                      widget = screen->createWidget(slotIndex, factory);
                  });
                }
            });
            menu->addLine(TR_WIDGET_SETTINGS, [=]() {
                new WidgetSettings(this, widget);
            });

            menu->addLine(STR_REMOVE_WIDGET, [=]() {
                widget->deleteLater();
                widget = nullptr;
            });
            return 0;
          }
          else {
            Menu * menu = new Menu(this);
            for (auto factory: getRegisteredWidgets()) {
              menu->addLine(factory->getName(), [=]() {
                  if (widget) widget->deleteLater();
                  widget = screen->createWidget(slotIndex, factory);
              });
            }
          }
          return 0;
      });
    }

    void paint(BitmapBuffer * dc) override
    {
      if (hasFocus()) {
        dc->drawSolidRect(0, 0, width(), height(), 1, DEFAULT_COLOR);
      }
      else {
        dc->drawRect(0, 0, width(), height(), 1, DOTTED, DEFAULT_COLOR);
      }
    }

  protected:
    Layout * screen;
    uint8_t slotIndex;
    Widget * widget = nullptr;
};

class SetupWidgetsPage: public FormWindow
{
  public:
    SetupWidgetsPage(Layout * screen):
      FormWindow(MainWindow::instance(), {0, 0, LCD_W, LCD_H}, OPAQUE | FORM_FORWARD_FOCUS),
      screen(screen),
      oldParent(screen->getParent())
    {
      Layer::push(this);
      clearFocus();

      // attach this custom screen here so we can display it 
      screen->attach(this);
      
      for (unsigned i = 0; i < screen->getZonesCount(); i++) {
        auto rect = screen->getZone(i);
        new SetupWidgetsPageSlot(this, {rect.x - 1, rect.y - 1, rect.w + 2, rect.h + 2}, screen, i);
      }
    }

    void deleteLater(bool detach = true, bool trash = true) override
    {
      Layer::pop(this);

#if defined(HARDWARE_TOUCH)
      Keyboard::hide();
#endif
      if (oldParent) {
        // give it back to its old parent before it gets deleted
        screen->attach(oldParent);
      }

      FormWindow::deleteLater(detach, trash);
    }

#if defined(HARDWARE_KEYS)
    void onEvent(event_t event) override
    {
      TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

      if (event == EVT_KEY_LONG(KEY_EXIT) || event == EVT_KEY_BREAK(KEY_EXIT)) {
        killEvents(event);
        deleteLater();
        SET_DIRTY();
      }
    }
#endif

  protected:
    Layout* screen;
    Window* oldParent;

    void paint(BitmapBuffer * dc) override
    {
      dc->clear(DEFAULT_BGCOLOR);
    }

};

void ScreenSetupPage::build(FormWindow * window)
{
  FormGridLayout grid;
  grid.spacer(PAGE_PADDING);

  // Layout choice...
  new StaticText(window, grid.getLabelSlot(false), STR_LAYOUT);
  auto layoutSlot = grid.getFieldSlot();
  layoutSlot.h = 2 * PAGE_LINE_HEIGHT - 1;
  layoutChoice = new LayoutChoice(window, layoutSlot, GET_VALUE(screen->getFactory()), [=](const LayoutFactory *factory) {
      delete screen;
      screen = factory->create(&screenData.layoutData);
      strncpy(screenData.LayoutId, factory->getId(), sizeof(CustomScreenData::LayoutId));
      SET_DIRTY();
      updateLayoutOptions();
  });
  grid.nextLine(layoutChoice->height());

  // Setup widgets button...
  setupWidgetsButton = new TextButton(window, grid.getFieldSlot(), STR_SETUP_WIDGETS);
  setupWidgetsButton->setPressHandler([=]() -> uint8_t {
      new SetupWidgetsPage(screen);
      return 0;
  });
  grid.nextLine();

  // Dynamic options window...
  optionsWindow = new FormGroup(window, {0, grid.getWindowHeight(), LCD_W, 0}, FORWARD_SCROLL | FORM_FORWARD_FOCUS);
  grid.addWindow(optionsWindow);
  updateLayoutOptions();
}

void ScreenSetupPage::updateLayoutOptions()
{
  FormGridLayout grid;
  optionsWindow->clear();

  // Layout options...
  int index = 0;
  for (auto * option = screen->getFactory()->getOptions(); option->name; option++, index++) {
    ZoneOptionValue * value = screen->getOptionValue(index);

    // Option label
    new StaticText(optionsWindow, grid.getLabelSlot(false), option->name);

    // Option value
    switch (option->type) {
      case ZoneOption::Bool:
        new CheckBox(optionsWindow, grid.getFieldSlot(), GET_SET_DEFAULT(value->boolValue));
        break;

      case ZoneOption::Color:
        new ColorEdit(optionsWindow, grid.getFieldSlot(), GET_SET_DEFAULT(value->unsignedValue));
        break;

      default:
        break;
    }
    grid.nextLine();
  }

  //if (pageIndex > 0 || customScreens[1]) {
    auto button = new TextButton(optionsWindow, grid.getFieldSlot(), STR_REMOVE_SCREEN);
    button->setPressHandler([=]() -> uint8_t {
        // if (pageIndex < MAX_CUSTOM_SCREENS - 1) {
        //   memmove(&g_model.screenData[pageIndex], &g_model.screenData[pageIndex + 1], sizeof(CustomScreenData) * (MAX_CUSTOM_SCREENS - pageIndex - 1));
        // }
        // memset(&g_model.screenData[MAX_CUSTOM_SCREENS - 1], 0, sizeof(CustomScreenData));
        memset(&screenData, 0, sizeof(CustomScreenData));
        loadCustomScreens();
        menu->updateTabs();
        storageDirty(EE_MODEL);
        return 0;
    });
    //}

  optionsWindow->adjustHeight();
  optionsWindow->getParent()->adjustInnerHeight();
}
