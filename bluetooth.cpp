#include "bluetooth.h"

#define OPCODE_REG_Bitmask 0x7F
#define OPCODE_Writetype_Bitmask 0x80
Bluetooth::Bluetooth(uint8_t address, TwoWire *wire)  
{
  _address = address;
  _wire    = wire;
  _error   = Bluetooth_OK;
}

bool Bluetooth::write8(uint8_t reg, uint8_t data)   
{
	return writeRegister(reg, data);
}


bool Bluetooth::writeRegister(uint8_t reg, uint8_t value)
{
  _wire->beginTransmission(_address); //start with adress + Write bit
  _wire->write(((1<<7)&OPCODE_Writetype_Bitmask)|(reg&OPCODE_REG_Bitmask));//write opcode 1=write 1 byte
  _wire->write(value);
  if (_wire->endTransmission() != 0)
  {
    _error = Bluetooth_ERROR;
    return false;
  }
  _error = Bluetooth_OK;
  return true;
}