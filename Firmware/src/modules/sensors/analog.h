#ifndef ANALOG_SENSOR_H
#define ANALOG_SENSOR_H

#include "../../interfaces/sensor.h"

class AnalogSensor : public Sensor {
private:
    uint8_t num_adc;
public:
    AnalogSensor(const char* description, uint8_t num_adc );
    int32_t read_raw() override;
    double read_scaled() override;
    uint8_t get_status() override;
    double read_velocity() override;
};

#endif // ANALOG_SENSOR_H
