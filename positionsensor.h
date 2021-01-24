#include "bno055.h"

class PositionSensor
{
public:
    PositionSensor(): _sensor() {}

    bool isCalibrated() const
    {
        return _sensor.isFullyCalibrated();
    }
    const sensors_vec_t& getPosition()
    {
        _sensor.getEvent(&_event);
        return _event.orientation;
    }

private:
    Adafruit_BNO055 _sensor;
    sensors_event_t _event;
};