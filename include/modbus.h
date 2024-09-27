#ifndef MODBUS_h
#define MODBUS_h

#include "Arduino.h"
#include <ModbusMaster.h>
#include <SoftwareSerial.h> 
#include "data2mqtt.h"


class Modbus
{    
private:
    ModbusMaster _modbus;
    SoftwareSerial* _serial;
    
    uint8_t _lastModbusTransmissionStatus;

    void readBlock(Device& device, RegisterType registerType, uint16_t startAddress, uint16_t nToRea);

public:
    Modbus(int pin_rx, int pin_tx, int slave_id, uint32_t baud);

    void read(Device& device);
};

#endif