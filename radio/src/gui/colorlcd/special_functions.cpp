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

#include "special_functions.h"
#include "opentx.h"
#include "libopenui.h"

#define SET_DIRTY()     storageDirty(functions == g_model.customFn ? EE_MODEL : EE_GENERAL)

class SpecialFunctionEditPage : public Page {
  public:
    SpecialFunctionEditPage(CustomFunctionData * functions, uint8_t index) :
      Page(functions == g_model.customFn ? ICON_MODEL_SPECIAL_FUNCTIONS : ICON_RADIO_GLOBAL_FUNCTIONS),
      functions(functions),
      index(index)
    {
      buildHeader(&header);
      buildBody(&body);
    }

  protected:
    CustomFunctionData * functions;
    uint8_t index;
    FormGroup * specialFunctionOneWindow = nullptr;
    StaticText * headerSF = nullptr;
    bool active = false;

    bool isActive() const
    {
      return ((functions == g_model.customFn ? modelFunctionsContext.activeSwitches : globalFunctionsContext.activeSwitches) & ((MASK_CFN_TYPE)1 << index) ? 1 : 0);
    }

    void checkEvents() override
    {
      Page::checkEvents();
      if (active != isActive()) {
        invalidate();
        headerSF->setTextFlags(isActive() ? FONT(BOLD) |HIGHLIGHT_COLOR : MENU_COLOR);
        active = !active;
      }
    }

    void buildHeader(Window * window)
    {
      new StaticText(window, { PAGE_TITLE_LEFT, PAGE_TITLE_TOP, LCD_W - PAGE_TITLE_LEFT, 20 }, functions == g_model.customFn ? STR_MENUCUSTOMFUNC : STR_MENUSPECIALFUNCS, 0, MENU_COLOR);
      headerSF = new StaticText(window, { PAGE_TITLE_LEFT, PAGE_TITLE_TOP + PAGE_LINE_HEIGHT, LCD_W - PAGE_TITLE_LEFT, 20 }, (functions == g_model.customFn ? "SF" : "GF" ) + std::to_string(index), 0, MENU_COLOR);
    }

    void updateSpecialFunctionOneWindow()
    {
      FormGridLayout grid;
      specialFunctionOneWindow->clear();

      CustomFunctionData * cfn = &functions[index];
      uint8_t func = CFN_FUNC(cfn);

      // Func param
      switch (func) {
        case FUNC_OVERRIDE_CHANNEL:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_CH);
          new NumberEdit(specialFunctionOneWindow, grid.getFieldSlot(), 1, MAX_OUTPUT_CHANNELS, GET_SET_VALUE_WITH_OFFSET(CFN_CH_INDEX(cfn), 1));
          grid.nextLine();

          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new NumberEdit(specialFunctionOneWindow, grid.getFieldSlot(), -100, 100, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_TRAINER: {
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          auto choice = new Choice(specialFunctionOneWindow, grid.getFieldSlot(), 0,NUM_STICKS + 1,GET_SET_DEFAULT(CFN_PARAM(cfn)));
          choice->setTextHandler([=](int32_t value) {
              if (value == 0)
                return std::string(STR_STICKS);
              else if (value == NUM_STICKS + 1)
                return std::string(STR_CHANS);
              else
                return TEXT_AT_INDEX(STR_VSRCRAW, value);;
          });
          grid.nextLine();
          break;
        }

        case FUNC_RESET:
          if (CFN_PARAM(cfn) < FUNC_RESET_PARAM_FIRST_TELEM) {
            new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_RESET);
            auto choice = new Choice(specialFunctionOneWindow, grid.getFieldSlot(), 0,
                                     FUNC_RESET_PARAM_FIRST_TELEM + lastUsedTelemetryIndex(),
                                     GET_SET_DEFAULT(CFN_PARAM(cfn)));
            choice->setAvailableHandler(isSourceAvailableInResetSpecialFunction);
            choice->setTextHandler([=](int32_t value) {
              if (value < FUNC_RESET_PARAM_FIRST_TELEM)
                return TEXT_AT_INDEX(STR_VFSWRESET, value);
              else
                return std::string(g_model.telemetrySensors[value - FUNC_RESET_PARAM_FIRST_TELEM].label, TELEM_LABEL_LEN);
            });
            grid.nextLine();
          }
          break;

        case FUNC_VOLUME:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VOLUME);
          new SourceChoice(specialFunctionOneWindow, grid.getFieldSlot(), 0, MIXSRC_LAST_CH, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_BACKLIGHT:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new SourceChoice(specialFunctionOneWindow, grid.getFieldSlot(), 0, MIXSRC_LAST_CH, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_PLAY_SOUND:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new Choice(specialFunctionOneWindow, grid.getFieldSlot(), STR_FUNCSOUNDS, 0,
                     AU_SPECIAL_SOUND_LAST - AU_SPECIAL_SOUND_FIRST - 1, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_PLAY_TRACK:
        case FUNC_BACKGND_MUSIC:
        case FUNC_PLAY_SCRIPT:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new FileChoice(specialFunctionOneWindow, grid.getFieldSlot(),
                         func == FUNC_PLAY_SCRIPT ? SCRIPTS_FUNCS_PATH : std::string(SOUNDS_PATH, SOUNDS_PATH_LNG_OFS) +
                                                                         std::string(currentLanguagePack->id, 2),
                         func == FUNC_PLAY_SCRIPT ? SCRIPTS_EXT : SOUNDS_EXT,
                         sizeof(cfn->play.name),
                         [=]() {
                           return std::string(cfn->play.name, ZLEN(cfn->play.name));
                         },
                         [=](std::string newValue) {
                           strncpy(cfn->play.name, newValue.c_str(), sizeof(cfn->play.name));
                           SET_DIRTY();
                         });
          grid.nextLine();
          break;

        case FUNC_SET_TIMER: {
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_TIMER);
          auto timerchoice = new Choice(specialFunctionOneWindow, grid.getFieldSlot(), 0, TIMERS - 1, GET_SET_DEFAULT(CFN_TIMER_INDEX(cfn)));
          timerchoice->setTextHandler([](int32_t value) {
            return std::string(STR_TIMER) + std::to_string(value + 1);
          });
          grid.nextLine();

          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new TimeEdit(specialFunctionOneWindow, grid.getFieldSlot(), 0, 9 * 60 * 60 - 1, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;
        }

        case FUNC_SET_FAILSAFE:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_MODULE);
          new Choice(specialFunctionOneWindow, grid.getFieldSlot(), "\004Int.Ext.", 0, NUM_MODULES - 1, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_PLAY_VALUE:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new SourceChoice(specialFunctionOneWindow, grid.getFieldSlot(), 0, MIXSRC_LAST_TELEM, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_HAPTIC:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new NumberEdit(specialFunctionOneWindow, grid.getFieldSlot(), 0, 3, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_LOGS: {
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          auto edit = new NumberEdit(specialFunctionOneWindow, grid.getFieldSlot(), 0, 255, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          edit->setDisplayHandler([=](BitmapBuffer * dc, LcdFlags flags, int32_t value) {
            dc->drawNumber(2, 2, CFN_PARAM(cfn), PREC1, sizeof(CFN_PARAM(cfn)), nullptr, "s");
          });
          break;
        }
      }

      if (HAS_ENABLE_PARAM(func)) {
        new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_ENABLE);
        new CheckBox(specialFunctionOneWindow, grid.getFieldSlot(), GET_SET_DEFAULT(CFN_ACTIVE(cfn)));
        grid.nextLine();
      }
      else if (HAS_REPEAT_PARAM(func)) { // !1x 1x 1s 2s 3s ...
        new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_REPEAT);
        auto repeat = new NumberEdit(specialFunctionOneWindow, grid.getFieldSlot(2, 1), -1, 60/CFN_PLAY_REPEAT_MUL, GET_SET_DEFAULT(CFN_PLAY_REPEAT(cfn)));
        repeat->setDisplayHandler([](BitmapBuffer * dc, LcdFlags flags, int32_t value) {
          if (value == 0)
            dc->drawText(3, 0, "1x", flags);
          else if (value == CFN_PLAY_REPEAT_NOSTART)
            dc->drawText(3, 0, "!1x", flags);
          else
            dc->drawNumber(3, 0, value * CFN_PLAY_REPEAT_MUL, flags, 0, nullptr, "s");
        });
        grid.nextLine();
      }

      specialFunctionOneWindow->adjustHeight();
    }

    void buildBody(FormWindow * window)
    {
      // SF.one
      FormGridLayout grid;
      grid.spacer(PAGE_PADDING);

      CustomFunctionData * cfn = &functions[index];

      // Switch
      new StaticText(window, grid.getLabelSlot(), STR_SWITCH);
      auto switchChoice = new SwitchChoice(window, grid.getFieldSlot(), SWSRC_FIRST, SWSRC_LAST, GET_SET_DEFAULT(CFN_SWITCH(cfn)));
      switchChoice->setAvailableHandler([=](int value) {
          return (functions == g_model.customFn ? isSwitchAvailable(value, ModelCustomFunctionsContext)
                                                : isSwitchAvailable(value,
                                                                    GeneralCustomFunctionsContext));
      });
      grid.nextLine();

      // Function
      new StaticText(window, grid.getLabelSlot(), STR_FUNC);
      auto functionChoice = new Choice(window, grid.getFieldSlot(), STR_VFSWFUNC, 0, FUNC_MAX - 1, GET_DEFAULT(CFN_FUNC(cfn)));
      functionChoice->setSetValueHandler([=](int32_t newValue) {
          CFN_FUNC(cfn) = newValue;
          CFN_RESET(cfn);
          SET_DIRTY();
          updateSpecialFunctionOneWindow();
      });
      functionChoice->setAvailableHandler([=](int value) {
        return isAssignableFunctionAvailable(value, functions);
      });
      grid.nextLine();

      specialFunctionOneWindow = new FormGroup(window, {0, grid.getWindowHeight(), LCD_W, 0}, FORM_FORWARD_FOCUS);
      updateSpecialFunctionOneWindow();
      grid.addWindow(specialFunctionOneWindow);
    }
};

static constexpr coord_t line1 = FIELD_PADDING_TOP;
static constexpr coord_t line2 = line1 + PAGE_LINE_HEIGHT;
static constexpr coord_t col1 = 20;
static constexpr coord_t col2 = (LCD_W - 100) / 3 + col1;
static constexpr coord_t col3 = ((LCD_W - 100) / 3) * 2 + col1 + 20;

class SpecialFunctionButton : public Button {
  public:
    SpecialFunctionButton(FormWindow * parent, const rect_t &rect, CustomFunctionData * functions, uint8_t index) :
      Button(parent, rect),
      functions(functions),
      index(index)
    {
      const CustomFunctionData * cfn = &functions[index];
      uint8_t func = CFN_FUNC(cfn);
      if (!cfn->isEmpty() &&
          (HAS_ENABLE_PARAM(func) || HAS_REPEAT_PARAM(func) || (func == FUNC_PLAY_TRACK || func == FUNC_BACKGND_MUSIC || func == FUNC_PLAY_SCRIPT))) {
        setHeight(height() + 20);
      }
    }

#if defined(DEBUG_WINDOWS)
    std::string getName() const override
    {
      return "SpecialFunctionButton";
    }
#endif

    bool isActive() const
    {
      return ((functions == g_model.customFn ? modelFunctionsContext.activeSwitches : globalFunctionsContext.activeSwitches) & ((MASK_CFN_TYPE)1 << index) ? 1 : 0);
    }

    void checkEvents() override
    {
      Button::checkEvents();
      if (active != isActive()) {
        invalidate();
        active = !active;
      }
    }

    void paintSpecialFunctionLine(BitmapBuffer * dc)
    {
      const CustomFunctionData * cfn = &functions[index];
      if (functions[index].func == FUNC_OVERRIDE_CHANNEL && functions != g_model.customFn) {
        functions[index].func = FUNC_OVERRIDE_CHANNEL + 1;
      }
      uint8_t func = CFN_FUNC(cfn);

      drawSwitch(dc, col1, line1, CFN_SWITCH(cfn), 0);
      if (cfn->isEmpty())
        return;

      dc->drawTextAtIndex(col2, line1, STR_VFSWFUNC, func, 0);
      int16_t val_min = 0;
      int16_t val_max = 255;

      switch (func) {
        case FUNC_OVERRIDE_CHANNEL:
          drawChn(dc, col1, line2, CFN_CH_INDEX(cfn) + 1, 0);
          getMixSrcRange(MIXSRC_FIRST_CH, val_min, val_max);
          dc->drawNumber(col2, line2, CFN_PARAM(cfn));
          break;

        case FUNC_TRAINER:
          drawSource(dc, col1, line2, CFN_CH_INDEX(cfn) == 0 ? 0 : MIXSRC_Rud + CFN_CH_INDEX(cfn) - 1);
          break;

        case FUNC_RESET:
          if (CFN_PARAM(cfn) < FUNC_RESET_PARAM_FIRST_TELEM) {
            dc->drawTextAtIndex(col1, line2, STR_VFSWRESET, CFN_PARAM(cfn));
          }
          else {
            TelemetrySensor * sensor = &g_model.telemetrySensors[CFN_PARAM(cfn) - FUNC_RESET_PARAM_FIRST_TELEM];
            dc->drawSizedText(col1, line2, sensor->label, TELEM_LABEL_LEN);
          }
          break;

        case FUNC_VOLUME:
          drawSource(dc, col1, line2, CFN_PARAM(cfn), 0);
          break;

        case FUNC_BACKLIGHT:
          drawSource(dc, col1, line2, CFN_PARAM(cfn), 0);
          break;

        case FUNC_PLAY_SOUND:
          dc->drawTextAtIndex(col1, line2, STR_FUNCSOUNDS, CFN_PARAM(cfn));
          break;

        case FUNC_PLAY_TRACK:
        case FUNC_BACKGND_MUSIC:
        case FUNC_PLAY_SCRIPT:
          if (ZEXIST(cfn->play.name))
            dc->drawSizedText(col1, line2, cfn->play.name, sizeof(cfn->play.name), 0);
          else
            dc->drawTextAtIndex(col1, line2, STR_VCSWFUNC, 0, 0);
          break;

        case FUNC_SET_TIMER:
          drawStringWithIndex(dc, col1, line2, STR_TIMER, CFN_TIMER_INDEX(cfn) + 1, 0);
          break;

        case FUNC_SET_FAILSAFE:
          dc->drawTextAtIndex(col1, line2, "\004Int.Ext.", CFN_PARAM(cfn), 0);
          break;

        case FUNC_PLAY_VALUE:
          drawSource(dc, col1, line2, CFN_PARAM(cfn), 0);
          break;

        case FUNC_HAPTIC:
          dc->drawNumber(col1, line2, CFN_PARAM(cfn), 0);
          break;

        case FUNC_LOGS:
          dc->drawNumber(col3, line1, CFN_PARAM(cfn), PREC1, sizeof(CFN_PARAM(cfn)), nullptr, "s");
          break;
      }
      if (HAS_ENABLE_PARAM(func)) {
        theme->drawCheckBox(dc, CFN_ACTIVE(cfn), col3, line2);
      }
      else if (HAS_REPEAT_PARAM(func)) {
        if (CFN_PLAY_REPEAT(cfn) == 0) {
          dc->drawText(col3, line2, "1x", 0);
        }
        else if (CFN_PLAY_REPEAT(cfn) == CFN_PLAY_REPEAT_NOSTART) {
          dc->drawText(col3, line2, "!1x", 0);
        }
        else {
          dc->drawNumber(col3 + 12, line2, CFN_PLAY_REPEAT(cfn) * CFN_PLAY_REPEAT_MUL, 0 | RIGHT, 0, nullptr, "s");
        }
      }
    }

    void paint(BitmapBuffer * dc) override
    {
      if (active)
        dc->drawSolidFilledRect(2, 2, rect.w - 4, rect.h - 4, HIGHLIGHT_COLOR);

      paintSpecialFunctionLine(dc);

      // The bounding rect
      dc->drawSolidRect(0, 0, rect.w, rect.h, 2, hasFocus() ? CHECKBOX_COLOR : DISABLE_COLOR);
    }

  protected:
    CustomFunctionData * functions;
    uint8_t index;
    bool active = false;
};

SpecialFunctionsPage::SpecialFunctionsPage(CustomFunctionData * functions) :
  PageTab(functions == g_model.customFn ? STR_MENUCUSTOMFUNC : STR_MENUSPECIALFUNCS,
          functions == g_model.customFn ? ICON_MODEL_SPECIAL_FUNCTIONS : ICON_RADIO_GLOBAL_FUNCTIONS),
  functions(functions)
{
}

void SpecialFunctionsPage::rebuild(FormWindow * window, int8_t focusSpecialFunctionIndex)
{
  coord_t scrollPosition = window->getScrollPositionY();
  window->clear();
  build(window, focusSpecialFunctionIndex);
  window->setScrollPositionY(scrollPosition);
}

void SpecialFunctionsPage::editSpecialFunction(FormWindow * window, uint8_t index)
{
  auto editPage = new SpecialFunctionEditPage(functions, index);
  editPage->setCloseHandler([=]() {
    rebuild(window, index);
  });
}

void SpecialFunctionsPage::build(FormWindow * window, int8_t focusIndex)
{
  FormGridLayout grid;
  grid.spacer(PAGE_PADDING);
  grid.setLabelWidth(66);

  Window::clearFocus();

  char s[] = "SFxx";
  if (functions == g_eeGeneral.customFn)
    s[0] = 'G';

  for (uint8_t i = 0; i < MAX_SPECIAL_FUNCTIONS; i++) {
    CustomFunctionData * cfn = &functions[i];
    strAppendUnsigned(&s[2], i);

    if (cfn->swtch == 0) {
      auto button = new TextButton(window, grid.getLabelSlot(), s);
      button->setPressHandler([=]() {
          editSpecialFunction(window, i);
          return 0;
      });
      grid.spacer(button->height() + 5);
    }
    else {
      new StaticText(window, grid.getLabelSlot(), s, BUTTON_BACKGROUND, CENTERED);

      auto button = new SpecialFunctionButton(window, grid.getFieldSlot(), functions, i);
      button->setPressHandler([=]() {
          button->bringToTop();
          Menu * menu = new Menu(window);
          menu->addLine(STR_EDIT, [=]() {
              editSpecialFunction(window, i);
          });
          menu->addLine(STR_COPY, [=]() {
              clipboard.type = CLIPBOARD_TYPE_CUSTOM_FUNCTION;
              clipboard.data.cfn = *cfn;
          });
          if (clipboard.type == CLIPBOARD_TYPE_CUSTOM_FUNCTION) {
            menu->addLine(STR_PASTE, [=]() {
                *cfn = clipboard.data.cfn;
                SET_DIRTY();
                rebuild(window, i);
            });
          }
          if (functions[MAX_SPECIAL_FUNCTIONS - 1].isEmpty()) {
            menu->addLine(STR_INSERT, [=]() {
                memmove(cfn + 1, cfn, (MAX_SPECIAL_FUNCTIONS - i - 1) * sizeof(CustomFunctionData));
                memset(cfn, 0, sizeof(CustomFunctionData));
                SET_DIRTY();
                rebuild(window, i);
            });
          }
          menu->addLine(STR_CLEAR, [=]() {
              memset(cfn, 0, sizeof(CustomFunctionData));
              SET_DIRTY();
          });
          for (int j = i; j < MAX_SPECIAL_FUNCTIONS; j++) {
            if (!functions[j].isEmpty()) {
              menu->addLine(STR_DELETE, [=]() {
                  memmove(cfn, cfn + 1, (MAX_SPECIAL_FUNCTIONS - i - 1) * sizeof(CustomFunctionData));
                  memset(&functions[MAX_SPECIAL_FUNCTIONS - 1], 0, sizeof(CustomFunctionData));
                  SET_DIRTY();
                  rebuild(window, i);
              });
              break;
            }
          }
          return 0;
      });

      if (focusIndex == i) {
        button->setFocus(SET_FOCUS_DEFAULT);
      }

      grid.spacer(button->height() + 5);
    }
  }

  grid.nextLine();

//  window->setLastField();
  window->setInnerHeight(grid.getWindowHeight());
}
