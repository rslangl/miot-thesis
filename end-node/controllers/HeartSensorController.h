#ifndef HEART_SENSOR_CONTROLLER_H_
#define HEART_SENSOR_CONTROLLER_H_

#include "CayenneLPP.h"
#include "PinNames.h"
#include "AnalogIn.h"
#include "DigitalIn.h"
#include "Timer.h"

#define BPM_THRESHOLD_MIN 60
#define BPM_THRESHOLD_MAX 100

class HeartSensorController
{
    public:
        HeartSensorController(PinName output, PinName lo_neg, PinName lo_pos);
        void init();
        void read(CayenneLPP* clpp);
    private:
        mbed::AnalogIn _output;
        mbed::DigitalIn _lo_neg, _lo_pos;
        mbed::LowPowerTimer _tmr;
        uint8_t _bpm;
};

#endif