#ifndef GPS_CONTROLLER_H_
#define GPS_CONTROLLER_H_

#include "mbed.h"
#include "CayenneLPP.h"

#define PI 3.14159265358979323846
#define RADIAN_DEGREES PI / 180
#define EARTH_RADIUS 6371000

#define MAX_SENTENCE_LENGTH 255
#define MIN_DISTANCE_TRESHOLD 2

class GPSController
{   
    public:
        GPSController(PinName tx, PinName rx, int baudrate);
        void init();
        void read(CayenneLPP* clpp);
        void getline();
        inline char read_char()
        {
            char ch;
            this->_gps->read(&ch, 1);
            return ch;
        }
        bool position_deviation(float lat, float lon, float current_lat, float current_lon);
        inline float convert(float dms)
        {
            float degmin, sec;
            sec = modf(dms, &degmin);

            float deg = (int) degmin / 100;
            float min = (int) degmin % 100;

            float dd = deg + (min / 60.0) + (sec / 3600);

            return dd;
        }
    private:
        BufferedSerial* _gps;
        LowPowerTimer _tmr;
        char *_nmea, _status, _NS, _EW;;
        float _lat, _lon, _UTCtime;
        bool _fix;

        const char* PMTK_COLD_RESTART = "$PMTK104*37<CR><LF>";
        const char* PMTK_GPRMC = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C<CR><LF>";
        const char* PMTK_1HZ = "$PMTK104*37<CR><LF>";
};

#endif