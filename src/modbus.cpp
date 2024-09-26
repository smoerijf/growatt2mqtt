#include "modbus.h"


Modbus::Modbus(int pin_rx, int pin_tx, int slave_id, uint32_t baud)
{
    _serial = new SoftwareSerial(pin_rx, pin_tx, false);
    _serial->begin(baud);
    _modbus.begin(slave_id , *_serial);
}

void Modbus::read(Device& device)
{
    // ESP.wdtDisable();
    // _lastModbusTransmissionStatus = modbus.readInputRegisters(0 * 64, 64);
    // ESP.wdtEnable(1);

    device.SetInputRegister(0, 1); // status
    device.SetInputRegister(1, 2); // solar_power
    device.SetInputRegister(2, 3);
    device.SetInputRegister(3, 4); // pv1 v
    device.SetInputRegister(4, 5); // pv1 a
    device.SetInputRegister(5, 6); // pv1 w
    device.SetInputRegister(6, 7);
}