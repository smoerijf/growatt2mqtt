#include "modbus.h"
#include <WebSerial.h>


Modbus::Modbus(int pin_rx, int pin_tx, int slave_id, uint32_t baud)
{
    _serial = new SoftwareSerial(pin_rx, pin_tx, false);
    _serial->begin(baud);
    _modbus.begin(slave_id , *_serial);
}

void Modbus::readBlock(Device& device, RegisterType registerType, uint16_t startAddress, uint16_t nToRead)
{   
    WebSerial.printf("modbus.readBlock(%i, %i)\n", startAddress, nToRead);

    _lastModbusTransmissionStatus = -1;
    ESP.wdtDisable();
    if (registerType == Input)
    {
        _lastModbusTransmissionStatus = _modbus.readInputRegisters(startAddress, nToRead);
    }
    else if (registerType == Holding)
    {
        _lastModbusTransmissionStatus = _modbus.readHoldingRegisters(startAddress, nToRead);
    }
    ESP.wdtEnable(1);

    if (_lastModbusTransmissionStatus == _modbus.ku8MBSuccess)
    {
        for (uint8_t i = 0; i < nToRead; i++)
        {
            device.Set(registerType, startAddress + i, _modbus.getResponseBuffer(i));
        }
    }
}

#define wordsToRead(datapoint) ((datapoint.size == OneWord) ? 1 : 2)

void Modbus::read(Device& device)
{
    uint16_t startAddress = device.datapoints[0].modbus_address;
    uint16_t nToRead = wordsToRead(device.datapoints[0]);
    uint16_t endAddress = startAddress + nToRead;
    RegisterType registerType = device.datapoints[0].modbus_address_type;
    for (uint8_t i = 1; i < device.numberOfDatapoints; i++)
    {
        MqttDataPoint datapoint = device.datapoints[i];
        uint16_t addressGap = datapoint.modbus_address - endAddress;
        if (addressGap > 5 || registerType != datapoint.modbus_address_type)
        {
            // Gap too large
            readBlock(device, registerType, startAddress, nToRead);

            startAddress = datapoint.modbus_address;
            nToRead = 0;
            addressGap = 0;
            registerType = datapoint.modbus_address_type;
        }

        uint16_t nWords = wordsToRead(datapoint);
        nToRead += nWords + addressGap;
        //WebSerial.printf("modbus.nToRead(#%i [addr=%i] nToRead=%i, #words=%i, gap=%i)\n", i, datapoint.modbus_address, nToRead, nWords, addressGap);
        endAddress = startAddress + nToRead;
    }
    
    if (nToRead > 0)
    {
        readBlock(device, registerType, startAddress, nToRead);
    }
}