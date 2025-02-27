#include "GasSensorController.h"
#include <cstdio>
#include "CayenneLPP.h"
#include "ThisThread.h"

GasSensorController::GasSensorController(PinName sda, PinName scl, uint8_t addr)
    :
    _gas_sensor(sda, scl),
    _address(addr << 1)
{
    init();
}

void GasSensorController::init()
{
    // Write I2C address
    this->_gas_sensor.write(_address);

    // Warm up
    this->_gas_sensor.write(0xFE);

    rtos::ThisThread::sleep_for(std::chrono::seconds(1));

    this->_no2      = 0.0;
    this->_c2h5oh   = 0.0;
    this->_voc      = 0.0;
    this->_co       = 0.0;
            
    printf("\r\n [Gas] Controller init! \r\n");
}

uint32_t GasSensorController::get_GM102B()
{
    this->_gas_sensor.write(GM_102B);
    return read_32();
}

uint32_t GasSensorController::get_GM302B()
{
    this->_gas_sensor.write(GM_302B);
    return read_32();
}

uint32_t GasSensorController::get_502B()
{
    this->_gas_sensor.write(GM_502B);
    return read_32();
}

uint32_t GasSensorController::get_702B()
{
    this->_gas_sensor.write(GM_702B);
    return read_32();
}

uint32_t GasSensorController::read_32()
{
    uint8_t idx = 0;
    uint32_t val = 0;
    char data[4];

    this->_gas_sensor.read(this->_address, data, 4);

    while(idx < 4)
    {
        val += (uint8_t) atoi(data) << (8 * idx);
        idx++;
    }

    rtos::ThisThread::sleep_for(std::chrono::seconds(1));

    return val;
}

void GasSensorController::read(CayenneLPP* clpp)
{
    printf("\r\n [Gas] ");

    this->_no2 = calcVol(get_GM102B());
    this->_c2h5oh = calcVol(get_GM302B());
    this->_voc = calcVol(get_502B());
    this->_co = calcVol(get_702B());

    if(this->_no2 > 0 || this->_c2h5oh > 0 || this->_voc > 0 || this->_co > 0)
    {
        printf(" Detected NO2 = %f, C2H5OH = %f, VOC = %f, CO = %f", this->_no2, this->_c2h5oh, this->_voc, this->_co);
        clpp->addPresence(LPP_PRESENCE, this->_no2);
    }
    else 
    {
        printf(" No gas detected");    
    }
    printf("\r\n");
}