#include "MUAPController.h"
#include "CayenneLPP.h"

MUAPController::MUAPController(PinName sig) : _sig(sig)
{
    init();
}

void MUAPController::init()
{
    this->_muap = 0;

    printf("\r\n [MUAP] Controller init! \r\n");
}

void MUAPController::read(CayenneLPP* clpp)
{
    float muap      = 0.0;
    int samples     = 0;

    printf("\r\n [MUAP] ");

    _tmr.start();

    while(_tmr.elapsed_time() <= std::chrono::seconds(10))
    {
        muap += _sig.read() * 100;
        samples++;
    }

    _tmr.stop();

    this->_muap = muap / samples;

    printf(" MUAP = %d \r\n", this->_muap);

    clpp->addDigitalOutput(LPP_DIGITAL_OUTPUT, this->_muap);

    _tmr.reset();
}