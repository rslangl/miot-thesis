#ifndef PUSH_BUTTON_CONTROLLER_H_
#define PUSH_BUTTON_CONTROLLER_H_

#include "PinNames.h"
#include "InterruptIn.h"

class PushbuttonController
{
    public:
        PushbuttonController(PinName pin, void (*func)(void)) : _pin(pin)
        {
            this->_pin.fall(func);
        }
    private:
        mbed::InterruptIn _pin;
};

#endif