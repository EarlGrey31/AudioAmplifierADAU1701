#ifndef GUI_H
#define GUI_H
#include <Arduino.h>
#include "frontpanel.h"
#include <SigmaDSP.h>
#include "SigmaDSP_parameters.h"

#define NBR_EQ_FREQ_BANDS 5

typedef enum GUIState {
  STARTUP,
  IDLE,
  EQ_BAND_ENTER,
  EQ_BAND_SELECTED,
  EQ_BAND_EXIT
} GUIState;

class GUI
{
  private:
    Frontpanel *_frontpanel;
    SigmaDSP *_dsp;
    uint8_t _led_left;
    uint8_t _led_right;
    uint8_t _led_eq_band[NBR_EQ_FREQ_BANDS];
    int8_t _eq_band_selected;
    GUIState _state;

    void calculateSignalLevels();
    void updateLeds();
    void handleStates();
    void handleStatesSTARTUP();
    void handleStatesIDLE();
    void handleStatesEQ_BAND_ENTER();
    void handleStatesEQ_BAND_SELECTED();
    void handleStatesEQ_BAND_EXIT();
    int8_t buttonStateToEQBand(uint8_t buttons);

  public:
    GUI(SigmaDSP *_dsp, Frontpanel *frontpanel);
    void handle();
};
#endif