#include "GPSController.h"
#include "CayenneLPP.h"
#include "mbed_assert.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>

GPSController::GPSController(PinName tx, PinName rx, int baudrate)
    :
    _gps(new BufferedSerial(tx, rx, baudrate)),
    _lat(0.0),
    _lon(0.0),
    _fix(false)
{
    init();

    printf("\r\n [GPS] Controller init! \r\n");
}

void GPSController::init()
{
    this->_nmea = (char*) malloc(sizeof(char) * MAX_SENTENCE_LENGTH);

    MBED_ASSERT(this->_gps->writable());

    this->_gps->write(PMTK_GPRMC, sizeof(PMTK_GPRMC));
    this->_gps->write(PMTK_1HZ, sizeof(PMTK_1HZ));
}

void GPSController::getline()
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

/**
 * Disclaimer: code taken from https://stackoverflow.com/a/63767823
 */
bool GPSController::position_deviation(float lat, float lon, float current_lat, float current_lon)
{
    lat *= RADIAN_DEGREES;
    lon *= RADIAN_DEGREES;
    current_lat *= RADIAN_DEGREES;
    current_lon *= RADIAN_DEGREES;

    float haversine, temp, min, dist;

    haversine = (pow(sin((1.0 / 2) * (current_lat - lat)), 2)) + ((cos(lat)) * (cos(current_lat)) * (pow(sin((1.0 / 2) * (current_lon - lon)), 2)));

    sqrt(haversine) < 1.0 ? min = sqrt(haversine) : 1.0;
    temp = 2 * asin(min);

    dist = EARTH_RADIUS * temp;

    return (dist > MIN_DISTANCE_TRESHOLD);
}

void GPSController::read(CayenneLPP* clpp)
{
    printf("\r\n [GPS] ");

    float lat, lon;

    this->_tmr.start();

    while(this->_tmr.elapsed_time() <= std::chrono::seconds(10))
    {
        getline();

        //printf("%s\n", this->_nmea);

        // GPRMC sentence: UTC time, status, lat, N/S, lon, E/W
        if(sscanf(this->_nmea, "GPRMC,%f,%c,%f,%c,%f,%c", &this->_UTCtime, &this->_status, &lat, &this->_NS, &lon, &this->_EW) > 0)
        {   
            if(this->_status == 'A')
            {
                this->_fix = true;

                lat = convert(lat);
                lon = convert(lon);

                if(this->_NS == 'S') { lat *= -1.0; }
                if(this->_EW == 'W') { lon *= -1.0; }

                if(position_deviation(this->_lat, this->_lon, lat, lon))
                {
                    printf(" Threshold met --");

                    this->_lat = lat;
                    this->_lon = lon;

                    clpp->addGPS(LPP_GPS, this->_lat, this->_lon, 0);
                }

                break;
            }
        }

        // Clear the NMEA buffer
        memset(this->_nmea, 0, strlen(this->_nmea));
    }

    this->_tmr.stop();
    this->_tmr.reset();

    if(this->_fix)
    {
        printf(" Lat = %f, Lon = %f", this->_lat, this->_lon);
    }
    else 
    {
        printf(" No fix");
    }

    printf("\r\n");
}