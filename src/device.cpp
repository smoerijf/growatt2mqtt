#include "device.h"


Device::Device(const MqttDataPoint* datapoints, uint8_t numberOfDatapoints) : datapoints(datapoints), numberOfDatapoints(numberOfDatapoints)
{
}


void Device::Set(MqttDataPoint& datapoint, uint16_t value)
{
    Set(datapoint.modbus_address_type, datapoint.modbus_address, value);
}

uint16_t Device::Get(MqttDataPoint& datapoint)
{
    return Get(datapoint.modbus_address_type, datapoint.modbus_address);
}

uint32_t Device::Get32(MqttDataPoint& datapoint)
{
    return (Get(datapoint.modbus_address_type, datapoint.modbus_address) << 16 | Get(datapoint.modbus_address_type, datapoint.modbus_address + 1));
}

void Device::Set(RegisterType type, uint16_t reg, uint16_t value)
{
    if (type == Input)
    {
        SetInputRegister(reg, value);
    }
}

uint16_t Device::Get(RegisterType type, uint16_t reg)
{
    if (type == Input)
    {
        return GetInputRegister(reg);
    }

    return 0;
}

void Device::SetInputRegister(uint16_t reg, uint16_t value)
{
    _inputData[reg] = value;
}

uint16_t Device::GetInputRegister(uint16_t reg)
{
    return _inputData[reg];
}
