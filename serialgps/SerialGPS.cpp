/* mbed GPS Module Library
 * Copyright (c) 2008-2010, sford
 * Copyright (c) 2013, B.Adryan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#include "SerialGPS.h"

SerialGPS::SerialGPS(PinName tx, PinName rx, int baud) 
    : 
    _gps_p(new BufferedSerial(tx, rx, baud)),
    _gps(*_gps_p)
{
    this->_gps_p->set_baud(baud);
    this->_UTCtime    = 0.0;
    this->_lat        = 0.0;
    this->_lon        = 0.0;
    this->_lock       = 0;
    this->_fix        = false;
}

SerialGPS::SerialGPS(BufferedSerial* serial)
    :
    _gps_p(serial),
    _gps(*_gps_p)
{
    this->_UTCtime    = 0.0;
    this->_lat        = 0.0;
    this->_lon        = 0.0;
    this->_lock       = 0;
    this->_fix        = false;
}

bool SerialGPS::fix()
{
    return this->_fix;
}

float SerialGPS::get_lat()
{
    return this->_lat;
}

float SerialGPS::get_lon()
{
    return this->_lon;
}

void SerialGPS::send_command(const char* cmd)
{
    this->_gps_p->write(cmd, sizeof(cmd));
}

void SerialGPS::read() 
{
        getline();

        //printf("NMEA sentence: %s\n", this->_nmea);

        // GPRMC sentence: UTC time, status, lat, N/S, lon, E/W, speed, course, date
        if(sscanf(this->_nmea, "GPRMC,%f,%c,%f,%c,%f,%c", &this->_UTCtime, &this->_status, &this->_lat, &this->_NS, &this->_lon, &this->_EW) > 0)
        {
            if(this->_status != 'A')
            {
                this->_fix = false;
            }
            else
            {
                set_vals();
            }
        }
        // GPGGA sentence: UTC time, lat, N/S, lon, E/W, fix indicator
        else if(sscanf(this->_nmea, "GPGGA,%f,%f,%c,%f,%c,%d", &this->_UTCtime, &this->_lat, &this->_NS, &this->_lon, &this->_EW, &this->_lock) > 0)
        {
            if(!this->_lock)
            {
                this->_fix = false;
            }
            else 
            {
                set_vals();
            }
        }
        // GPGLL sentence: lat, N/S, lo, E/W, UTC time, status
        else if(sscanf(this->_nmea, "GPGLL,%f,%c,%f,%c,%f,%c", &this->_lat, &this->_NS, &this->_lon, &this->_EW, &this->_UTCtime, &this->_status) > 0)
        {
            if(this->_status != 'A')
            {
                this->_fix = false;
            }
            else 
            {
                set_vals();
            }
        }
        else
        {
            this->_fix = false;
        }

    // Clear the NMEA buffer
    memset(this->_nmea, 0, strlen(this->_nmea));
}

void SerialGPS::set_vals()
{

    this->_lat = convert(this->_lat);
    this->_lon = convert(this->_lon);

    if(this->_NS == 'S') { this->_lat *= -1.0; }
    if(this->_EW == 'W') { this->_lon *= -1.0; }

    this->_fix = true;
}

void SerialGPS::getline() 
{
    char ch;
    uint8_t idx = 0;

    //MBED_ASSERT(this->_gps->readable());

    // Wait for start of sentence
    while(read_char() != '$');

    // Read character until EOL
    while((ch = read_char()) != '\r')
    {
        this->_nmea[idx] = ch;
        idx++;
    }
}