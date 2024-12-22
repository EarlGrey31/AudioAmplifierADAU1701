#ifndef FRONTPANEL_H
#define FRONTPANEL_H
#include <Arduino.h>
#include <TCA9555.h>
#include <Wire.h>

#define FRONTPANEL_NBR_TCA 5
#define FRONTPANEL_GPIO_PER_TCA 16
#define FRONTPANEL_NBR_LEDS 70

class FrontpanelTCA9555Wrapper : public TCA9555 {
  private:
    uint16_t _ddr;
    uint16_t _odr;
    uint16_t _idr;
    uint16_t _pdr;

  public:
    bool     pinMode1(uint8_t pin, uint8_t mode);
    bool     write1(uint8_t pin, uint8_t value);
    uint8_t  read1(uint8_t pin);
    bool     setPolarity(uint8_t pin, uint8_t value);    //  input pins only.
    uint8_t  getPolarity(uint8_t pin);

    bool     pinMode8(uint8_t port, uint8_t mask);
    bool     write8(uint8_t port, uint8_t mask);
    int      read8(uint8_t port);
    bool     setPolarity8(uint8_t port, uint8_t value);
    uint8_t  getPolarity8(uint8_t port);

    bool     pinMode16(uint16_t mask);
    bool     write16(uint16_t mask);
    uint16_t read16();
    bool     setPolarity16(uint16_t mask);
    uint8_t  getPolarity16();
    
    FrontpanelTCA9555Wrapper(uint8_t address, TwoWire *wire = &Wire);
    void read();
    void write();
    void print();
};

class Frontpanel
{
  private:
    FrontpanelTCA9555Wrapper portExtenders[FRONTPANEL_NBR_TCA];

    static uint8_t indexForGPIO(uint8_t gpio) {
      return gpio / FRONTPANEL_GPIO_PER_TCA;
    }

    TCA9555 tcaForGPIO(uint8_t gpio) {
      return portExtenders[indexForGPIO(gpio)];
    }

  public:
    static uint8_t ledFromRMS(uint32_t rms);
    static uint8_t portBitFromGPIO(uint8_t gpio);

    Frontpanel();
    void ledOn(uint8_t led);
    void ledOnAll();
    void ledOff(uint8_t led);
    void ledOffAll();
    void ledOnBar(uint8_t bar, uint8_t led);
    void ledOffBar(uint8_t bar, uint8_t led);
    void initAnimation();
    void write();
    void read();
    void printAddresses();
    void printRegisters();
    void write16raw(uint8_t index, uint16_t mask);
};
#endif