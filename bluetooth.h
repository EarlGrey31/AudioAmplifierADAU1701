#ifndef BLUETOOTH_H
#define BLUETOOTH_H
#include <Arduino.h>
#include <TCA9555.h>
#include <Wire.h>

#define Bluetooth_OK        0x00
#define Bluetooth_ERROR		0xFF
#define	VCO_Offset			0x01
#define	EnableVCXO			0b00000101
#define	EnableXO			0b00000001
#define bluetoothenablereg	0x00// TODO pin finden um rp2040 die leitung zu pullen/pushen
class Bluetooth
{
	public:
		Bluetooth(uint8_t address, TwoWire *wire = &Wire);
		bool state; // 1=VCXO 0 = XO
	
	
		bool write8(uint8_t reg, uint8_t data);
	
	
	protected:
		bool writeRegister(uint8_t reg, uint8_t data);
			uint8_t  _address;
			TwoWire* _wire;
			uint8_t  _error;
};

#endif