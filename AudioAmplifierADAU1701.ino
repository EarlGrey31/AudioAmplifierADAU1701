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

#define NBR_EQ_FREQ_BANDS 5

// The first parameter is the Wire object we'll be using when communicating wth the DSP
// The second parameter is the DSP i2c address, which is defined in the parameter file
// The third parameter is the sample rate
// An optional fourth parameter is the pin to physically reset the DSP
SigmaDSP dsp(Wire, DSP_I2C_ADDRESS, 96000.00f, PIN_ADAU_RST);
Frontpanel *frontpanel;

uint16_t vcc_threshold_low;
uint16_t vcc_threshold_high;
bool vcc_good;
uint16_t vee_threshold_low;
uint16_t vee_threshold_high;
bool vee_good;

uint16_t voutForVee(float vee, float vref = 3.3, float R1=10e3, float R2=150e3) {
  return (uint16_t)(4096.0/3.3*(vref*R2/(R1+R2)+vee*R1/(R1+R2)));
}

uint16_t voutForVcc(float vcc, float vref = 3.3, float R1=10e3, float R2=150e3) {
  return (uint16_t)(4096.0/3.3*(vcc*R1/(R1+R2)));
}

uint8_t __cnt;

void updateFrontpanel() {
  uint32_t currentLevelCH0  = dsp.readBack(MOD_CH0_RMS_ALG0_VAL0_ADDR,  MOD_CH0_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelCH1  = dsp.readBack(MOD_CH1_RMS_ALG0_VAL0_ADDR,  MOD_CH1_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelBand0  = dsp.readBack(MOD_BAND0_RMS_ALG0_VAL0_ADDR,  MOD_BAND0_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelBand1  = dsp.readBack(MOD_BAND1_RMS_ALG0_VAL0_ADDR,  MOD_BAND1_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelBand2  = dsp.readBack(MOD_BAND2_RMS_ALG0_VAL0_ADDR,  MOD_BAND2_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelBand3  = dsp.readBack(MOD_BAND3_RMS_ALG0_VAL0_ADDR,  MOD_BAND3_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t currentLevelBand4  = dsp.readBack(MOD_BAND4_RMS_ALG0_VAL0_ADDR,  MOD_BAND4_RMS_ALG0_VAL0_VALUES,  3);
  uint32_t bands[] = {currentLevelBand0, currentLevelBand1, currentLevelBand2, currentLevelBand3, currentLevelBand4};

  //Serial.printf("B0: %d; B1: %d; B2: %d; B3: %d; B4: %d\n", currentLevelBand0, currentLevelBand1, currentLevelBand2, currentLevelBand3, currentLevelBand4);

  frontpanel->read();
  uint8_t led_left = Frontpanel::ledFromRMS(currentLevelCH0);
  uint8_t led_right = Frontpanel::ledFromRMS(currentLevelCH1);
  uint8_t led_eq_band[NBR_EQ_FREQ_BANDS];
  for (int i = 0; i < NBR_EQ_FREQ_BANDS; i++) {
    led_eq_band[i] = Frontpanel::ledFromRMS(bands[i]*10);
  }
  //Serial.printf("rms_left: %d; decoded: %d\n", currentLevelCH0, currentLevelCH1);
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
    for (int i = 0; i < NBR_EQ_FREQ_BANDS; i++) {
      if (current_led <= led_eq_band[i]) {
        frontpanel->ledOnBar(i+2, current_led);
      } else {
        frontpanel->ledOffBar(i+2, current_led);
      }
    }
  }
  frontpanel->write();
}

void checkSupplyLevels() {
  uint16_t vcc = analogRead(A0);
  uint16_t vee = analogRead(A1);
  Serial.printf("VCC: %d; VEE: %d\n", vcc, vee);
  Serial.printf("VCC_Threshold: (%d; %d)\n", vcc_threshold_low, vcc_threshold_high);
  Serial.printf("VEE_Threshold: (%d; %d)\n", vee_threshold_low, vee_threshold_high);
  if (vcc_good) {
    if (vcc < vcc_threshold_low) {
      Serial.printf("Warning, VCC below threshold! Meas: %d; Thresh: %d\n", vcc, vcc_threshold_low);
      vcc_good = false;
    }
  } else {
    if (vcc > vcc_threshold_high) {
      Serial.printf("Info, VCC above threshold. Meas: %d; Thresh: %d\n", vcc, vcc_threshold_high);
      vcc_good = true;
    }
  }

  if (vee_good) {
    if (vee > vee_threshold_low) {
      Serial.printf("Warning, VEE above threshold! Meas: %d; Thresh: %d\n", vee, vee_threshold_low);
      vee_good = false;
    }
  } else {
    if (vee < vee_threshold_high) {
      Serial.printf("Info, VEE below threshold. Meas: %d; Thresh: %d\n", vee, vee_threshold_high);
      vee_good = true;
    }
  }
}

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

  analogReadResolution(12);

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

  vcc_threshold_low = voutForVcc(25);
  vcc_threshold_high = voutForVcc(28);

  vee_threshold_low = voutForVee(-25);
  vee_threshold_high = voutForVee(-28);

  vcc_good = false;
  vee_good = false;

  __cnt = 0;
}

void loop() {
  uint32_t checksum  = dsp.readBack(MOD_READBACK1_ALG0_VAL0_ADDR,  MOD_READBACK1_ALG0_VAL0_VALUES,  3);
  //Serial.printf("Readback checksum: %x\n", checksum);
  updateFrontpanel();
  __cnt++;
  if (__cnt > 10) {
    __cnt = 0;
    checkSupplyLevels();
  }
  digitalWrite(PIN_STATUS, !digitalRead(PIN_STATUS));
}
