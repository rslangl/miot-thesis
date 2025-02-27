/*
    Multichannel_Gas_GMXXX.cpp
    Description: A drive for Seeed Grove Multichannel gas sensor V2.0.
    2019 Copyright (c) Seeed Technology Inc.  All right reserved.
    Author: Hongtai Liu(lht856@foxmail.com)
    2019-6-18

    The MIT License (MIT)
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.1  USA
*/

#include "Multichannel_Gas_GMXXX.h"

GAS_GMXXX::GAS_GMXXX(PinName sda, PinName scl, uint8_t addr) 
    :
    _i2c_p(new I2C(sda, scl)),
    _i2c(*_i2c_p),
    GMXXX_ADDRESS((addr << 1))
{
    init();
}

void GAS_GMXXX::init()
{
    this->_no2          = 0;
    this->_c2h5oh       = 0;
    this->_voc          = 0;
    this->_co           = 0;
    this->gasDetected   = false;
    this->_i2c_p->write(GMXXX_ADDRESS);
    this->preheated();
}

/**
    @description:  set the I2C address, use after init.
    @param {type}  address(uint_8)
    @return: None
*/
void GAS_GMXXX::setAddress(uint8_t address) 
{
    GMXXX_ADDRESS = address;
    preheated();
    isPreheated = true;
}


/**
    @description:  transmit a byte to I2C device
    @param {type}  cmd(uint_8)
    @return: None
*/
void GAS_GMXXX::GMXXXWriteByte(uint8_t cmd) 
{
    _i2c.write(cmd);
    ThisThread::sleep_for(1s);
}

/**
    @description:  read 4 bytes from I2C device
    @param {type}  cmd(uint_8)
    @return: uint32_t
*/
uint32_t GAS_GMXXX::GMXXXRead32() 
{
    uint8_t index = 0;
    uint32_t value = 0;
    char data[4];

    this->_i2c_p->read(this->GMXXX_ADDRESS, data, 4);
    while(index < 4)
    {
        value += (uint8_t) atoi(data) << (8 * index);
        index++;
    }
    ThisThread::sleep_for(1s);
    return value;
}

/**
    @description:  warmming up the gas sensor
    @param {type}:  None
    @return: uint32_t
*/
void GAS_GMXXX::preheated() 
{
    GMXXXWriteByte(WARMING_UP);
    isPreheated = true;
}

/**
    @description:  disable warmming up the gas sensor
    @param {type}:  None
    @return: uint32_t
*/
void GAS_GMXXX::unPreheated() 
{
    GMXXXWriteByte(WARMING_DOWN);
    isPreheated = false;
}

/**
    @description:  get the adc value of GM102B
    @param {type}:  None
    @return: uint32_t
*/
uint32_t GAS_GMXXX::getGM102B() 
{
    if (!isPreheated) 
    {
        preheated();
    }
    GMXXXWriteByte(GM_102B);
    return GMXXXRead32();
}

/**
    @description:  get the adc value of GM302B
    @param {type}:  None
    @return: uint32_t
*/
uint32_t GAS_GMXXX::getGM302B() 
{
    if (!isPreheated) 
    {
        preheated();
    }
    GMXXXWriteByte(GM_302B);
    return GMXXXRead32();
}

#ifdef GM_402B
/**
    @description:  get the adc value of GM402B
    @param {type}:  None
    @return: uint32_t
*/
uint32_t GAS_GMXXX::getGM402B() 
{
    if (!isPreheated) 
    {
        preheated();
    }
    GMXXXWriteByte(GM_402B);
    return GMXXXRead32();
}
#endif

/**
    @description:  get the adc value of GM502B
    @param {type}:  None
    @return: uint32_t
*/
uint32_t GAS_GMXXX::getGM502B() 
{
    if (!isPreheated) 
    {
        preheated();
    }
    GMXXXWriteByte(GM_502B);
    return GMXXXRead32();
}

/**
    @description:  get the adc value of GM702B
    @param {type}:  None
    @return: uint32_t
*/
uint32_t GAS_GMXXX::getGM702B() 
{
    if (!isPreheated) 
    {
        preheated();
    }
    GMXXXWriteByte(GM_702B);
    return GMXXXRead32();
}

#ifdef GM_802B
/**
    @description:  get the adc value of GM802B
    @param {type}:  None
    @return: uint32_t
*/
uint32_t GAS_GMXXX::getGM802B() 
{
    if (!isPreheated) 
    {
        preheated();
    }
    GMXXXWriteByte(GM_802B);
    return GMXXXRead32();
}
#endif

void GAS_GMXXX::read()
{
    this->_no2 = calcVol(measure_NO2());
    this->_c2h5oh = calcVol(measure_C2H5OH());
    this->_voc = calcVol(measure_VOC());
    this->_co = calcVol(measure_CO());

    if(this->_no2 > 0 || this->_c2h5oh > 0 || this->_voc > 0 || this->_co > 0)
    {
        this->gasDetected = true;
    }

    this->gasDetected = false;
}

bool GAS_GMXXX::gas_detected()
{
    return this->gasDetected;
}

float GAS_GMXXX::get_no2()
{
    return this->_no2;
}

float GAS_GMXXX::get_c2h5oh()
{
    return this->_c2h5oh;
}

float GAS_GMXXX::get_voc()
{
    return this->_voc;
}

float GAS_GMXXX::get_co()
{
    return this->_co;
}

/**
    @description:  change the I2C address of gas sensor
    @param {type}:  addres(uint8_t)
    @return: None
*/
void GAS_GMXXX::changeGMXXXAddr(uint8_t address) 
{
    if (address == 0 || address > 127) 
    {
        address = 0x08;
    }
    this->_i2c_p->write(CHANGE_I2C_ADDR);
    this->_i2c_p->write(address);
    GMXXX_ADDRESS = address;
}