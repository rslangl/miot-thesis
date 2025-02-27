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

#ifndef MBED_GPS_H
#define MBED_GPS_H

#include "mbed.h"
#include <cstring>
#include "BufferedSerial.h"

#define MAX_LINELENGTH 255

class SerialGPS 
{
    public:
        SerialGPS(PinName tx, PinName rx, int baud);

        SerialGPS(BufferedSerial* serial);
    
        /** 
        * Read the incoming GPS data
        */
        void read();
    
        /**
        * Send PMTK command to GPS
        *
        * @param cmd the PMTK command to send to the GPS module
        */
        void send_command(const char* cmd);

        /**
        * GPS fix
        *
        * @return true if the GPS module has acquired fix
        */ 
        bool fix();

        /**
         * Read one character from the UART interface
         *
         * @return ch the character extracted from the UART interface
         */
        inline char read_char()
        {
            char ch;
            this->_gps_p->read(&ch, 1);
            return ch;
        }

        /**
        * Helper function for converting from Decimal Minutes Seconds (DMS) to Decimal Degrees (DD)
        *
        * @param dms float containing the DMS value
        * @return dd the converted value
        */
        inline float convert(float dms)
        {
            float degmin, sec;
            sec = modf(dms, &degmin);

            float deg = (int) degmin / 100;
            float min = (int) degmin % 100;

            float dd = deg + (min / 60.0) + (sec / 3600);

            return dd;
        };

        /** 
        * The longitude (call read() to set) 
        *
        * @return _lat the latitude component
        */
        float get_lon();

        /** 
        * The latitude (call read() to set) 
        *
        * @return _lon the longitude component
        */
        float get_lat();
    
    private:
        void getline();
        void set_vals();
        char read_char();

        BufferedSerial* _gps_p;
        BufferedSerial& _gps;

        char _nmea[MAX_LINELENGTH];
        bool _fix;

        float _UTCtime, _lat, _lon;
        int _lock;
        char _status, _NS, _EW;
};

#endif