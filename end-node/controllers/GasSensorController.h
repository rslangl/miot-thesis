#ifndef GAS_SENSOR_CONTROLLER_H_
#define GAS_SENSOR_CONTROLLER_H_

#include "CayenneLPP.h"
#include "PinNames.h"
#include "I2C.h"
#include <cstdint>

#define GM_RESOLUTION 1023
#define GM_102B 0x01
#define GM_302B 0x03
#define GM_502B 0x05
#define GM_702B 0x07

class GasSensorController
{
    public:
        GasSensorController(PinName sda, PinName scl, uint8_t addr);
        void init();
        inline float calcVol(uint32_t adc)
        {
            return (adc * 3.3) / GM_RESOLUTION;
        }
        uint32_t get_GM102B();
        uint32_t get_GM302B();
        uint32_t get_502B();
        uint32_t get_702B();
        uint32_t read_32();
        void read(CayenneLPP* clpp);
    private:
        mbed::I2C _gas_sensor;
        uint8_t _address;
        float _no2, _c2h5oh, _voc, _co;
};

#endif