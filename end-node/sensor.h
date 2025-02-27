#ifndef SENSOR_H_
#define SENSOR_H_

#include "CayenneLPP.h"
#include "GPSController.h"
#include "GasSensorController.h"
#include "HeartSensorController.h"
#include "MUAPController.h"

static GPSController gps_controller(D8, D2, 9600);
static GasSensorController gas_sensor_controller(I2C_SDA, I2C_SCL, 0x08);
static HeartSensorController heart_sensor_controller(PA_0, D7, D3); //PA_0, D6, D5
static MUAPController muap_sensor_controller(PA_4);

static CayenneLPP* clpp;

static void init_payload(int payload_size)
{
    clpp = new CayenneLPP(payload_size);
};

#endif