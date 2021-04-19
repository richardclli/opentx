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

#include "hardware.h"
#include "ui_hardware.h"

void HardwarePanel::setupSwitchType(int index, QLabel * label, AutoLineEdit * name, AutoComboBox * type, bool threePos)
{
  Board::Type board = getCurrentBoard();
  if (IS_STM32(board) && index < getBoardCapability(board, Board::Switches)) {
    type->addItem(tr("None"), Board::SWITCH_NOT_AVAILABLE);
    type->addItem(tr("2 Positions Toggle"), Board::SWITCH_TOGGLE);
    type->addItem(tr("2 Positions"), Board::SWITCH_2POS);
    if (threePos) {
      type->addItem(tr("3 Positions"), Board::SWITCH_3POS);
    }
    name->setField(generalSettings.switchName[index], 3, this);
    type->setField(generalSettings.switchConfig[index], this);
    if (IS_TARANIS_X7(board)) {
      if (index == 4) {
        label->setText("SF");
      }
      else if (index == 5) {
        label->setText("SH");
      }
      if (index == 6) {
        label->setText("SI");
      }
      else if (index == 7) {
        label->setText("SJ");
      }
    }
    else if (IS_RADIOMASTER_TX12(board)) {
      if (index == 6) {
        label->setText("SI");
      }
      else if (index == 7) {
        label->setText("SJ");
      }
    }
    else if (IS_FAMILY_T12(board)) {
      if (index == 4) {
        label->setText("SG");
      }
      else if (index == 5) {
        label->setText("SH");
      }
    }
  }
  else {
    label->hide();
    name->hide();
    type->hide();
  }
}

void HardwarePanel::setupPotType(int index, QLabel * label, AutoLineEdit * name, AutoComboBox * type)
{
  Board::Type board = firmware->getBoard();

  if (IS_STM32(board) && index < getBoardCapability(board, Board::Pots)) {
    label->setText(RawSource(SOURCE_TYPE_STICK, CPN_MAX_STICKS+index).toString());
    type->addItem(tr("None"), Board::POT_NONE);
    type->addItem(tr("Pot with detent"), Board::POT_WITH_DETENT);
    type->addItem(tr("Multipos switch"), Board::POT_MULTIPOS_SWITCH);
    type->addItem(tr("Pot without detent"), Board::POT_WITHOUT_DETENT);
    name->setField(generalSettings.potName[index], 3, this);
    type->setField(generalSettings.potConfig[index], this);
  }
  else {
    label->hide();
    name->hide();
    type->hide();
  }
}

void HardwarePanel::setupSliderType(int index, QLabel *label, AutoLineEdit *name, AutoComboBox *type)
{
  Board::Type board = firmware->getBoard();

  if (IS_STM32(board) && index < getBoardCapability(board, Board::Sliders)) {
    label->setText(RawSource(SOURCE_TYPE_STICK, CPN_MAX_STICKS+getBoardCapability(board, Board::Pots)+index).toString());
    type->addItem(tr("None"), Board::SLIDER_NONE);
    type->addItem(tr("Slider with detent"), Board::SLIDER_WITH_DETENT);
    name->setField(generalSettings.sliderName[index], 3, this);
    type->setField(generalSettings.sliderConfig[index], this);
  }
  else {
    label->hide();
    name->hide();
    type->hide();
  }
}

bool HardwarePanel::isSwitch3Pos(int idx)
{
  Board::Type board = firmware->getBoard();
  Board::SwitchInfo switchInfo = Boards::getSwitchInfo(board, idx);

  switchInfo.config = Board::SwitchType(generalSettings.switchConfig[idx]);
  return switchInfo.config == Board::SWITCH_3POS;
};

HardwarePanel::HardwarePanel(QWidget * parent, GeneralSettings & generalSettings, Firmware * firmware):
  GeneralPanel(parent, generalSettings, firmware),
  ui(new Ui::Hardware)
{
  ui->setupUi(this);

  Board::Type board = firmware->getBoard();

  if (IS_STM32(board)) {
    ui->rudName->setField(generalSettings.stickName[0], 3, this);
    ui->eleName->setField(generalSettings.stickName[1], 3, this);
    ui->thrName->setField(generalSettings.stickName[2], 3, this);
    ui->ailName->setField(generalSettings.stickName[3], 3, this);
  }
  else {
    ui->rudLabel->hide();
    ui->rudName->hide();
    ui->eleLabel->hide();
    ui->eleName->hide();
    ui->thrLabel->hide();
    ui->thrName->hide();
    ui->ailLabel->hide();
    ui->ailName->hide();
    ui->potsTypeSeparator_1->hide();
    ui->potsTypeSeparator_2->hide();
  }
  
  if (firmware->getCapability(HasSportConnector)) {
    ui->sportPower->setChecked(generalSettings.sportPower);
  }
  else {
    ui->sportPower->hide();
    ui->sportPowerLabel->hide();
  }

  setupPotType(0, ui->pot1Label, ui->pot1Name, ui->pot1Type);
  setupPotType(1, ui->pot2Label, ui->pot2Name, ui->pot2Type);
  setupPotType(2, ui->pot3Label, ui->pot3Name, ui->pot3Type);
  setupPotType(3, ui->pot4Label, ui->pot4Name, ui->pot4Type);
  setupPotType(4, ui->pot5Label, ui->pot5Name, ui->pot5Type);

  setupSliderType(0, ui->lsLabel, ui->lsName, ui->lsType);
  setupSliderType(1, ui->rsLabel, ui->rsName, ui->rsType);
  setupSliderType(2, ui->ls2Label, ui->ls2Name, ui->ls2Type);
  setupSliderType(3, ui->rs2Label, ui->rs2Name, ui->rs2Type);

  setupSwitchType(0, ui->saLabel, ui->saName, ui->saType, isSwitch3Pos(0));
  setupSwitchType(1, ui->sbLabel, ui->sbName, ui->sbType, isSwitch3Pos(1));
  setupSwitchType(2, ui->scLabel, ui->scName, ui->scType, isSwitch3Pos(2));
  setupSwitchType(3, ui->sdLabel, ui->sdName, ui->sdType, isSwitch3Pos(3));
  setupSwitchType(4, ui->seLabel, ui->seName, ui->seType, isSwitch3Pos(4));
  setupSwitchType(5, ui->sfLabel, ui->sfName, ui->sfType, isSwitch3Pos(5));
  setupSwitchType(6, ui->sgLabel, ui->sgName, ui->sgType, isSwitch3Pos(6));
  setupSwitchType(7, ui->shLabel, ui->shName, ui->shType, isSwitch3Pos(7));
  setupSwitchType(8, ui->siLabel, ui->siName, ui->siType, isSwitch3Pos(8));
  setupSwitchType(9, ui->sjLabel, ui->sjName, ui->sjType, isSwitch3Pos(9));
  setupSwitchType(10, ui->skLabel, ui->skName, ui->skType); // Here starts X9E, only 3 switches
  setupSwitchType(11, ui->slLabel, ui->slName, ui->slType);
  setupSwitchType(12, ui->smLabel, ui->smName, ui->smType);
  setupSwitchType(13, ui->snLabel, ui->snName, ui->snType);
  setupSwitchType(14, ui->soLabel, ui->soName, ui->soType);
  setupSwitchType(15, ui->spLabel, ui->spName, ui->spType);
  setupSwitchType(16, ui->sqLabel, ui->sqName, ui->sqType);
  setupSwitchType(17, ui->srLabel, ui->srName, ui->srType);

  if (IS_TARANIS(board) && !IS_TARANIS_SMALL(board)) {
    ui->serialPortMode->setCurrentIndex(generalSettings.auxSerialMode);
  }
  else {
    ui->serialPortMode->setCurrentIndex(0);
    ui->serialPortMode->hide();
    ui->serialPortLabel->hide();
  }

  if (!IS_SKY9X(board)) {
    ui->txCurrentCalibration->hide();
    ui->txCurrentCalibrationLabel->hide();
  }

  if (IS_TARANIS_X7(board) || IS_TARANIS_XLITE(board)|| IS_TARANIS_X9E(board) || IS_TARANIS_X9DP_2019(board) || IS_FAMILY_HORUS_OR_T16(board)) {
    ui->bluetoothMode->addItem(tr("OFF"), 0);
    if (IS_TARANIS_X9E(board)) {
      ui->bluetoothMode->addItem(tr("Enabled"), 1);
    }
    else {
      ui->bluetoothMode->addItem(tr("Telemetry"), 1);
      ui->bluetoothMode->addItem(tr("Trainer"), 2);
    }
    ui->bluetoothMode->setField(generalSettings.bluetoothMode, this);
    ui->bluetoothName->setField(generalSettings.bluetoothName, 10, this);
  }
  else {
    ui->bluetoothLabel->hide();
    ui->bluetoothWidget->hide();
  }

  if ((IS_FAMILY_HORUS_OR_T16(board) && board != Board::BOARD_X10_EXPRESS) || (IS_TARANIS_XLITE(board) && !IS_TARANIS_XLITES(board))) {
    ui->antennaMode->addItem(tr("Internal"), -2);
    ui->antennaMode->addItem(tr("Ask"), -1);
    ui->antennaMode->addItem(tr("Per model"), 0);
    ui->antennaMode->addItem(IS_HORUS_X12S(board) ? tr("Internal + External") : tr("External"), 1);
    ui->antennaMode->setField(generalSettings.antennaMode, this);
  }
  else {
    ui->antennaLabel->hide();
    ui->antennaMode->hide();
  }

  if (IS_HORUS_OR_TARANIS(board)) {
    ui->filterEnable->setChecked(!generalSettings.jitterFilter);
  }
  else {
    ui->filterEnable->hide();
    ui->filterLabel->hide();
  }

  if (IS_STM32(board)) {
    ui->rtcCheckDisable->setChecked(!generalSettings.rtcCheckDisable);
  }
  else {
    ui->rtcCheckDisable->hide();
    ui->rtcCheckLabel->hide();
  }

  disableMouseScrolling();

  setValues();
}

HardwarePanel::~HardwarePanel()
{
  delete ui;
}

void HardwarePanel::on_filterEnable_stateChanged()
{
  generalSettings.jitterFilter = !ui->filterEnable->isChecked();
}

void HardwarePanel::on_sportPower_stateChanged()
{
  generalSettings.sportPower = ui->sportPower->isChecked();
}

void HardwarePanel::on_rtcCheckDisable_stateChanged()
{
  generalSettings.rtcCheckDisable = !ui->rtcCheckDisable->isChecked();
}

void HardwarePanel::on_PPM_MultiplierDSB_editingFinished()
{
  int val = (int)(ui->PPM_MultiplierDSB->value()*10)-10;
  if (generalSettings.PPM_Multiplier != val) {
    generalSettings.PPM_Multiplier = val;
    emit modified();
  }
}

void HardwarePanel::on_PPM1_editingFinished()
{
  if (generalSettings.trainer.calib[0] != ui->PPM1->value()) {
    generalSettings.trainer.calib[0] = ui->PPM1->value();
    emit modified();
  }
}

void HardwarePanel::on_PPM2_editingFinished()
{
  if (generalSettings.trainer.calib[1] != ui->PPM2->value()) {
    generalSettings.trainer.calib[1] = ui->PPM2->value();
    emit modified();
  }
}

void HardwarePanel::on_PPM3_editingFinished()
{
  if (generalSettings.trainer.calib[2] != ui->PPM3->value()) {
    generalSettings.trainer.calib[2] = ui->PPM3->value();
    emit modified();
  }
}

void HardwarePanel::on_PPM4_editingFinished()
{
  if (generalSettings.trainer.calib[3] != ui->PPM4->value()) {
    generalSettings.trainer.calib[3] = ui->PPM4->value();
    emit modified();
  }
}


void HardwarePanel::on_txCurrentCalibration_editingFinished()
{
  if (generalSettings.txCurrentCalibration != ui->txCurrentCalibration->value()) {
    generalSettings.txCurrentCalibration = ui->txCurrentCalibration->value();
    emit modified();
  }
}

void HardwarePanel::setValues()
{
  ui->txVoltageCalibration->setValue((double)generalSettings.txVoltageCalibration/10);
  ui->txCurrentCalibration->setValue((double)generalSettings.txCurrentCalibration);

  ui->PPM1->setValue(generalSettings.trainer.calib[0]);
  ui->PPM2->setValue(generalSettings.trainer.calib[1]);
  ui->PPM3->setValue(generalSettings.trainer.calib[2]);
  ui->PPM4->setValue(generalSettings.trainer.calib[3]);
  ui->PPM_MultiplierDSB->setValue((qreal)(generalSettings.PPM_Multiplier+10)/10);
}

void HardwarePanel::on_txVoltageCalibration_editingFinished()
{
  if (generalSettings.txVoltageCalibration != ui->txVoltageCalibration->value()*10) {
    generalSettings.txVoltageCalibration = ui->txVoltageCalibration->value()*10;
    emit modified();
  }
}

void HardwarePanel::on_serialPortMode_currentIndexChanged(int index)
{
  generalSettings.auxSerialMode = index;
  emit modified();
}
