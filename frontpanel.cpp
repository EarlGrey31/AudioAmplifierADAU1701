#include <TCA9555.h>
#include "frontpanel.h"

//  REGISTERS duplicated because not in orginal header file ....
#define TCA9555_INPUT_PORT_REGISTER_0     0x00    //  read()
#define TCA9555_INPUT_PORT_REGISTER_1     0x01
#define TCA9555_OUTPUT_PORT_REGISTER_0    0x02    //  write()
#define TCA9555_OUTPUT_PORT_REGISTER_1    0x03
#define TCA9555_POLARITY_REGISTER_0       0x04    //  get/setPolarity()
#define TCA9555_POLARITY_REGISTER_1       0x05
#define TCA9555_CONFIGURATION_PORT_0      0x06    //  pinMode()
#define TCA9555_CONFIGURATION_PORT_1      0x07

FrontpanelTCA9555Wrapper::FrontpanelTCA9555Wrapper(uint8_t address, TwoWire *wire): TCA9555(address, wire) {read();}

bool FrontpanelTCA9555Wrapper::pinMode1(uint8_t pin, uint8_t mode) {
  if (pin > 15)
  {
    _error = TCA9555_PIN_ERROR;
    return false;
  }
  if ( (mode != INPUT) && (mode != OUTPUT) )
  {
    _error = TCA9555_VALUE_ERROR;
    return false;
  }
  uint16_t mask = 1 << pin;
  if (mode == INPUT)  _ddr |= mask;
  if (mode == OUTPUT) _ddr &= ~mask;
  _error = TCA9555_OK;
  return true;
}

bool FrontpanelTCA9555Wrapper::write1(uint8_t pin, uint8_t value) {
  if (pin > 15)
  {
    _error = TCA9555_PIN_ERROR;
    return false;
  }
  uint16_t mask = 1 << pin;
  if (!value) _odr |= mask;
  else _odr &= ~mask;
  _error = TCA9555_OK;
  return true;
}

uint8_t FrontpanelTCA9555Wrapper::read1(uint8_t pin) {
  if (pin > 15)
  {
    _error = TCA9555_PIN_ERROR;
    return TCA9555_INVALID_READ;
  }
  uint16_t mask = 1 << pin;
  _error = TCA9555_OK;
  if (_idr & mask) return HIGH;
  return LOW;
}

bool FrontpanelTCA9555Wrapper::setPolarity(uint8_t pin, uint8_t value) {
  if (pin > 15)
  {
    _error = TCA9555_PIN_ERROR;
    return false;
  }
  uint16_t mask = 1 << pin;
  if (value == HIGH) _pdr |= mask;
  if (value == LOW)  _pdr &= ~mask;
  _error = TCA9555_OK;
  return true;
}

uint8_t FrontpanelTCA9555Wrapper::getPolarity(uint8_t pin) {
  if (pin > 15)
  {
    _error = TCA9555_PIN_ERROR;
    return false;
  }
  _error = TCA9555_OK;
  return (_pdr >> pin) >= 0x01;
}

bool FrontpanelTCA9555Wrapper::pinMode8(uint8_t port, uint8_t mask) {
  if (port > 1)
  {
    _error = TCA9555_PORT_ERROR;
    return false;
  }
  _error = TCA9555_OK;
  if (port == 0) _ddr = (_ddr & 0xff00) | mask;
  if (port == 1) _ddr = (_ddr & 0x00ff) | (mask<<8);
  return true;
}

bool FrontpanelTCA9555Wrapper::write8(uint8_t port, uint8_t mask) {
  if (port > 1)
  {
    _error = TCA9555_PORT_ERROR;
    return false;
  }
  _error = TCA9555_OK;
  if (port == 0) _odr = (_odr && 0xff00) | mask;
  if (port == 1) _odr = (_odr && 0x00ff) | (mask<<8);
  return true;
}
int FrontpanelTCA9555Wrapper::read8(uint8_t port) {
  if (port > 1)
  {
    _error = TCA9555_PORT_ERROR;
    return TCA9555_INVALID_READ;
  }
  _error = TCA9555_OK;
  if (port == 0) return _idr & 0xFF;
  if (port == 1) return (_idr >> 8) & 0xFF;
  return 0; // keeps compiler happy
}

bool FrontpanelTCA9555Wrapper::setPolarity8(uint8_t port, uint8_t value) {
  if (port > 1)
  {
    _error = TCA9555_PORT_ERROR;
    return false;
  }
  _error = TCA9555_OK;
  if (port == 0) _pdr = (_pdr & 0xff00) | value;
  if (port == 1) _pdr = (_pdr & 0x00ff) | (value<<8);
  return true;
}

uint8_t FrontpanelTCA9555Wrapper::getPolarity8(uint8_t port) {
  if (port > 1)
  {
    _error = TCA9555_PORT_ERROR;
    return TCA9555_INVALID_READ;
  }
  _error = TCA9555_OK;
  if (port == 0) return _pdr & 0xFF;
  if (port == 1) return (_pdr >> 8) & 0xFF;
  return 0; // keeps compiler happy
}

bool FrontpanelTCA9555Wrapper::pinMode16(uint16_t mask) {
  _ddr = mask;
  return true;
}

bool FrontpanelTCA9555Wrapper::write16(uint16_t mask) {
  _odr = mask;
  return true;
}

uint16_t FrontpanelTCA9555Wrapper::read16() {
  return _idr;
}

bool FrontpanelTCA9555Wrapper::setPolarity16(uint16_t mask) {
  _pdr = mask;
  return true;
}

uint8_t FrontpanelTCA9555Wrapper::getPolarity16() {
  return _pdr;
}

void FrontpanelTCA9555Wrapper::read() {
  _ddr = ((uint16_t)(readRegister(TCA9555_CONFIGURATION_PORT_1))<<8) || readRegister(TCA9555_CONFIGURATION_PORT_0);
  _idr = ((uint16_t)(readRegister(TCA9555_INPUT_PORT_REGISTER_1))<<8) || readRegister(TCA9555_INPUT_PORT_REGISTER_0);
  _odr = ((uint16_t)(readRegister(TCA9555_OUTPUT_PORT_REGISTER_1))<<8) || readRegister(TCA9555_OUTPUT_PORT_REGISTER_0);
  _pdr = ((uint16_t)(readRegister(TCA9555_POLARITY_REGISTER_1))<<8) || readRegister(TCA9555_POLARITY_REGISTER_0);
}

void FrontpanelTCA9555Wrapper::write() {
  TCA9555::pinMode16(_ddr);
  TCA9555::write16(_odr);
  TCA9555::setPolarity16(_pdr);
}

void FrontpanelTCA9555Wrapper::print() {
  uint8_t myAddr = getAddress();
  Serial.printf("Wrapper at address %x\n", myAddr);
  Serial.printf("DDR: 0x%04x\n", _ddr);
  Serial.printf("IDR: 0x%04x\n", _idr);
  Serial.printf("ODR: 0x%04x\n", _odr);
  Serial.printf("PDR: 0x%04x\n", _pdr);
}

Frontpanel::Frontpanel():portExtenders{FrontpanelTCA9555Wrapper(0b0100000, &Wire1),
      FrontpanelTCA9555Wrapper(0b0100001, &Wire1),
      FrontpanelTCA9555Wrapper(0b0100010, &Wire1),
      FrontpanelTCA9555Wrapper(0b0100011, &Wire1),
      FrontpanelTCA9555Wrapper(0b0100100, &Wire1)}
{
  for (uint8_t i=0; i < FRONTPANEL_NBR_TCA; i++) {
    if (!(this->portExtenders[i].begin())) {
      Serial.printf("Error initializing port extender %d", i);
      return;
    }
  }
  read();

  // init LED DDR
  for (int i = 0; i < FRONTPANEL_NBR_LEDS; i++) {
    uint8_t index = indexForGPIO(i);
    uint8_t pin = portBitFromGPIO(i);
    this->portExtenders[index].pinMode1(pin, OUTPUT);
  }

  for (int i = 0; i < FRONTPANEL_NBR_LEDS; i++) {
    uint8_t index = indexForGPIO(i);
    uint8_t pin = portBitFromGPIO(i);
    this->portExtenders[index].write1(pin, LOW);
  }

  write();
}

void Frontpanel::ledOn(uint8_t led) {
  uint8_t index = indexForGPIO(led);
  uint8_t pin = portBitFromGPIO(led);
  this->portExtenders[index].write1(pin, HIGH);
}

void Frontpanel::ledOff(uint8_t led) {
  uint8_t index = indexForGPIO(led);
  uint8_t pin = portBitFromGPIO(led);
  this->portExtenders[index].write1(pin, LOW);
}

void Frontpanel::ledOnAll() {
  for (int i = 0;i < FRONTPANEL_NBR_LEDS; i++) {
    ledOn(i);
  }
  write();
}

void Frontpanel::ledOffAll() {
  for (int i = 0;i < FRONTPANEL_NBR_LEDS; i++) {
    ledOff(i);
  }
  write();
}

void Frontpanel::ledOnBar(uint8_t bar, uint8_t led) {
  uint8_t effective_gpio = 10*bar + led;
  if (effective_gpio < FRONTPANEL_NBR_LEDS) {
    ledOn(effective_gpio);
  } else {
    Serial.printf("Invalid LED %d, %d\n", bar, led);
  }
}

void Frontpanel::ledOffBar(uint8_t bar, uint8_t led) {
  uint8_t effective_gpio = 10*bar + led;
  if (effective_gpio < FRONTPANEL_NBR_LEDS) {
    ledOff(effective_gpio);
  } else {
    Serial.printf("Invalid LED %d, %d\n", bar, led);
  }
}

void Frontpanel::initAnimation() {
  for (int i = 0;i < FRONTPANEL_NBR_LEDS; i++) {
        ledOff(i);
    }
    for (int bar = 0; bar < 7; bar++) {
        for (int led = 0; led < 10; led++) {
            ledOnBar(bar, led);
            write();
            delay(100);
        }
        for (int led = 9; led >= 0; led--) {
            ledOffBar(bar, led);
            write();
            delay(100);
        }
    }
    for (int repeats = 0; repeats < 3; repeats++) {
        delay(500);
        for (int i = 0;i < FRONTPANEL_NBR_LEDS; i++) {
            ledOn(i);
        }
        write();
        delay(500);
        for (int i = 0;i < FRONTPANEL_NBR_LEDS; i++) {
            ledOff(i);
        }
        write();
    }
}

void Frontpanel::write() {
  for (uint8_t i=0; i < FRONTPANEL_NBR_TCA; i++) {
    this->portExtenders[i].write();
  }
}

void Frontpanel::read() {
  for (uint8_t i=0; i < FRONTPANEL_NBR_TCA; i++) {
    this->portExtenders[i].read();
  }
}

void Frontpanel::printAddresses() {
  for (uint8_t i=0; i < FRONTPANEL_NBR_TCA; i++) {
    Serial.println("Port Extender Addresses");
    Serial.println(i);
    Serial.println(this->portExtenders[i].getAddress());
  }
}

void Frontpanel::printRegisters() {
  for (uint8_t i=0; i < FRONTPANEL_NBR_TCA; i++) {
    this->portExtenders[i].print();
  }
}

uint8_t Frontpanel::ledFromRMS(uint32_t rms) {
  switch (rms) {
    //-29 dB
    case 0 ... 18602: return 0;
    //-25 dB
    case 18603 ... 29483: return 1;
    //-21 dB
    case 29484 ... 46727: return 2;
    //-17 dB
    case 46728 ... 74058: return 3;
    //-13 dB
    case 74059 ... 117373: return 4;
    //-9 dB
    case 117374 ... 186024: return 5;
    //-6 dB
    case 186025 ... 262766: return 6;
    //-3 dB
    case 262767 ... 371167: return 7;
    //0 dB
    case 371168 ... 524288: return 8;
    //2 dB
    case 524289 ... 660039: return 9;
    default: return 9;
  }
}

uint8_t Frontpanel::portBitFromGPIO(uint8_t gpio) {
    return (gpio % FRONTPANEL_GPIO_PER_TCA);
}

void Frontpanel::write16raw(uint8_t index, uint16_t mask) {
  this->portExtenders[index].write16(mask);
}