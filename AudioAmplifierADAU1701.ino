// Include Wire and SigmaDSP library
#include <Wire.h>
#include <SigmaDSP.h>
#include <Rotary.h>
#include "frontpanel.h"
#include "gui.h"
// Include generated parameter file
#include "SigmaDSP_parameters.h"

#define PIN_ADDR_ADAU 19
#define PIN_ADAU_RST 15
#define PIN_ADAU_PLL_MODE0 13
#define PIN_ADAU_PLL_MODE1 14
#define PIN_MUTE_B 10
#define PIN_RELAY_ON 11
#define PIN_STATUS 12

#define PIN_ROTARY0 6
#define PIN_ROTARY1 7

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
GUI *gui;

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

float fixed2float(uint32_t val) {
  return float(val)/float(1<<19);
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

void printStatus() {
  uint32_t balance  = dsp.readBack(MOD_BALANCEADC_ALG0_VAL0_ADDR,  MOD_BALANCEADC_ALG0_VAL0_VALUES,  3);
  uint32_t volume  = dsp.readBack(MOD_VOLUMEADC_ALG0_VAL0_ADDR,  MOD_VOLUMEADC_ALG0_VAL0_VALUES,  3);
  Serial.printf("Balance: %f\n", fixed2float(balance));
  Serial.printf("Volume: %f\n", fixed2float(volume));
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

  gui = new GUI(&dsp, frontpanel, PIN_ROTARY0, PIN_ROTARY1);
  //frontpanel->printAddresses();

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
  gui->handle();
  __cnt++;
  if (__cnt > 10) {
    __cnt = 0;
    checkSupplyLevels();
    printStatus();
  }
  digitalWrite(PIN_STATUS, !digitalRead(PIN_STATUS));
}
