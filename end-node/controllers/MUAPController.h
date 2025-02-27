#ifndef MUAP_CONTROLLER_H_
#define MUAP_CONTROLLER_H_

#include "CayenneLPP.h"
#include "PinNames.h"
#include "Timer.h"
#include "AnalogIn.h"

class MUAPController
{
    public:
        MUAPController(PinName sig);
        void init();
        void read(CayenneLPP* clpp);
    private:
        mbed::AnalogIn _sig;
        mbed::LowPowerTimer _tmr;
        uint32_t _muap;
};

#endif