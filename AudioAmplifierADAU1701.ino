// Include Wire and SigmaDSP library
#include <Wire.h>
#include <SigmaDSP.h>
#include "frontpanel.h"
// Include generated parameter file
#include "SigmaDSP_parameters.h"

#define PIN_ADDR_ADAU 19
#define PIN_ADAU_RST 15
#define PIN_ADAU_PLL_MODE0 13
#define PIN_ADAU_PLL_MODE1 14
#define PIN_MUTE_B 10
#define PIN_RELAY_ON 11
#define PIN_STATUS 12

#define I2C_SDA_ADAU 16
#define I2C_SCL_ADAU 17

#define I2C_SDA_AUX 2
#define I2C_SCL_AUX 3

// The first parameter is the Wire object we'll be using when communicating wth the DSP
// The second parameter is the DSP i2c address, which is defined in the parameter file
// The third parameter is the sample rate
// An optional fourth parameter is the pin to physically reset the DSP
SigmaDSP dsp(Wire, DSP_I2C_ADDRESS, 96000.00f, PIN_ADAU_RST);
Frontpanel *frontpanel;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_STATUS, OUTPUT);

  Wire.setSDA(I2C_SDA_ADAU);
  Wire.setSCL(I2C_SCL_ADAU);
  Wire1.setSDA(I2C_SDA_AUX);
  Wire1.setSCL(I2C_SCL_AUX);

  Wire.begin();
  Wire1.begin();

  dsp.begin();
  //ee.begin();
  pinMode(PIN_ADAU_RST, OUTPUT);
  pinMode(PIN_ADAU_PLL_MODE0, OUTPUT);
  pinMode(PIN_ADAU_PLL_MODE1, OUTPUT);
  
  pinMode(PIN_ADDR_ADAU, OUTPUT);
  pinMode(PIN_MUTE_B, OUTPUT);
  pinMode(PIN_RELAY_ON, OUTPUT);

  digitalWrite(PIN_ADDR_ADAU, LOW);

  digitalWrite(PIN_ADAU_RST, HIGH);
  digitalWrite(PIN_ADAU_RST, LOW);
  digitalWrite(PIN_ADAU_PLL_MODE0, HIGH);
  digitalWrite(PIN_ADAU_PLL_MODE1, HIGH);
  digitalWrite(PIN_ADAU_RST, HIGH);

  digitalWrite(PIN_MUTE_B, HIGH);
  digitalWrite(PIN_RELAY_ON, HIGH);

  delay(2000);


  Serial.println(F("Pinging i2c lines...\n0 -> device is present\n2 -> device is not present"));
  Serial.print(F("DSP response: "));
  Serial.println(dsp.ping());
  //Serial.print(F("EEPROM ping: "));
  //Serial.println(ee.ping());


  // Use this step if no EEPROM is present
  Serial.print(F("\nLoading DSP program... "));
  loadProgram(dsp);
  Serial.println("Done!\n");

  frontpanel = new Frontpanel();
  //frontpanel->printAddresses();
  frontpanel->initAnimation();
}

void updateFrontpanel(uint32_t rms_left, uint32_t rms_right) {
  frontpanel->read();
  uint8_t led_left = Frontpanel::ledFromRMS(rms_left);
  uint8_t led_right = Frontpanel::ledFromRMS(rms_right);
  Serial.printf("rms_left: %d; decoded: %d\n", rms_left, led_left);
  for (int current_led = 0; current_led < 10; current_led++) {
    if (current_led <= led_left) {
      frontpanel->ledOnBar(0, current_led);
    } else {
      frontpanel->ledOffBar(0, current_led);
    }
    if (current_led <= led_right) {
      frontpanel->ledOnBar(1, current_led);
    } else {
      frontpanel->ledOffBar(1, current_led);
    }
  }
  frontpanel->write();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t currentLevelCH0  = dsp.readBack(MOD_CH0_RMS_ALG0_VAL0_ADDR,  MOD_CH0_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelCH1  = dsp.readBack(MOD_CH1_RMS_ALG0_VAL0_ADDR,  MOD_CH1_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t checksum  = dsp.readBack(MOD_READBACK1_ALG0_VAL0_ADDR,  MOD_READBACK1_ALG0_VAL0_VALUES,  3);
  Serial.printf("Readback checksum: %x\n", checksum);
  Serial.printf("Readback CH0: %d\n", currentLevelCH0);
  Serial.printf("Readback CH1: %d\n", currentLevelCH1);
  updateFrontpanel(currentLevelCH0, currentLevelCH1);
  digitalWrite(PIN_STATUS, !digitalRead(PIN_STATUS));
}
