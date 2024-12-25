#include "gui.h"

GUI::GUI(SigmaDSP *dsp, Frontpanel *frontpanel): _dsp{dsp}, _frontpanel{frontpanel}, 
  _led_left{0}, _led_right{0}, _led_eq_band{0,0,0,0,0}, _state{STARTUP},
  _eq_band_selected{-1} {

}

void GUI::updateLeds() {
  //Serial.printf("rms_left: %d; decoded: %d\n", currentLevelCH0, currentLevelCH1);
  for (int current_led = 0; current_led < 10; current_led++) {
    if (current_led <= _led_left) {
      _frontpanel->ledOnBar(0, current_led);
    } else {
      _frontpanel->ledOffBar(0, current_led);
    }
    if (current_led <= _led_right) {
      _frontpanel->ledOnBar(1, current_led);
    } else {
      _frontpanel->ledOffBar(1, current_led);
    }
    for (int i = 0; i < NBR_EQ_FREQ_BANDS; i++) {
      if (current_led <= _led_eq_band[i]) {
        _frontpanel->ledOnBar(i+2, current_led);
      } else {
        _frontpanel->ledOffBar(i+2, current_led);
      }
    }
  }
}

void GUI::calculateSignalLevels() {
  uint32_t currentLevelCH0  = _dsp->readBack(MOD_CH0_RMS_ALG0_VAL0_ADDR,  MOD_CH0_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelCH1  = _dsp->readBack(MOD_CH1_RMS_ALG0_VAL0_ADDR,  MOD_CH1_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelBand0  = _dsp->readBack(MOD_BAND0_RMS_ALG0_VAL0_ADDR,  MOD_BAND0_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelBand1  = _dsp->readBack(MOD_BAND1_RMS_ALG0_VAL0_ADDR,  MOD_BAND1_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelBand2  = _dsp->readBack(MOD_BAND2_RMS_ALG0_VAL0_ADDR,  MOD_BAND2_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelBand3  = _dsp->readBack(MOD_BAND3_RMS_ALG0_VAL0_ADDR,  MOD_BAND3_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelBand4  = _dsp->readBack(MOD_BAND4_RMS_ALG0_VAL0_ADDR,  MOD_BAND4_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t bands[] = {currentLevelBand0, currentLevelBand1, currentLevelBand2, currentLevelBand3, currentLevelBand4};

  _led_left = Frontpanel::ledFromRMS(currentLevelCH0);
  _led_right = Frontpanel::ledFromRMS(currentLevelCH1);
  for (int i = 0; i < NBR_EQ_FREQ_BANDS; i++) {
    _led_eq_band[i] = Frontpanel::ledFromRMS(bands[i]*10);
  }
}

int8_t GUI::buttonStateToEQBand(uint8_t buttons) {
  for (int i = 0; i < FRONTPANEL_NBR_EQ_BANDS; i++) {
    if ((buttons & (1<<i))>0) return i;
  }
  return -1;
}

void GUI::handleStatesSTARTUP() {
  _frontpanel->initAnimation();
  _state = IDLE;
}

void GUI::handleStatesIDLE() {
  calculateSignalLevels();
  updateLeds();
  uint8_t buttons = _frontpanel->getButtonStates();
  if (buttons > 0) {
    _eq_band_selected = buttonStateToEQBand(buttons);
  }
  if (_eq_band_selected >= 0) {
    _state = EQ_BAND_ENTER;
  }
  else {
    _state = IDLE;
  }
}

void GUI::handleStatesEQ_BAND_ENTER() {
  calculateSignalLevels();
  updateLeds();
  uint8_t buttons = _frontpanel->getButtonStates();
  if (buttons == 0) {
    _state = EQ_BAND_SELECTED;
  }
  else {
    _eq_band_selected = buttonStateToEQBand(buttons);
    _state = EQ_BAND_ENTER;
  }
}

void GUI::handleStatesEQ_BAND_SELECTED() {
  calculateSignalLevels();
  // ToDo
  _led_eq_band[_eq_band_selected] = 5;
  updateLeds();
  uint8_t buttons = _frontpanel->getButtonStates();
  if (buttons == 0) {
    _state = EQ_BAND_SELECTED;
  }
  else {
    _eq_band_selected = -1;
    _state = EQ_BAND_EXIT;
  }
}

void GUI::handleStatesEQ_BAND_EXIT() {
  calculateSignalLevels();
  updateLeds();
  uint8_t buttons = _frontpanel->getButtonStates();
  if (buttons == 0) {
    _state = IDLE;
  }
  else {
    _state = EQ_BAND_EXIT;
  }
}

void GUI::handleStates() {
  Serial.printf("Current State: %d\n", _state);
  switch (_state) {
    case STARTUP:
      handleStatesSTARTUP();
      break;
    case IDLE:
      handleStatesIDLE();
      break;
    case EQ_BAND_ENTER:
      handleStatesEQ_BAND_ENTER();
      break;
    case EQ_BAND_SELECTED:
      handleStatesEQ_BAND_SELECTED();
      break;
    case EQ_BAND_EXIT:
      handleStatesEQ_BAND_EXIT();
      break;
    default:
      _state = IDLE;
      break;
  }
}

void GUI::handle() {
  _frontpanel->read();
  handleStates();
  _frontpanel->write();
}