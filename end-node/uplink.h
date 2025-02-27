#ifndef UPLINK_H_
#define UPLINK_H_

#include <cstdint>
#include "lorawan.h"
#include "sensor.h"
#include "utils.h"

#define PONG lorawan.send(MBED_CONF_LORA_APP_PORT, (uint8_t*) "pong", 4, MSG_UNCONFIRMED_FLAG);

static void send_message()
{
    int16_t retcode;

    gps_controller.read(clpp);
    gas_sensor_controller.read(clpp);
    heart_sensor_controller.read(clpp);
    muap_sensor_controller.read(clpp);

    retcode = lorawan.send(MBED_CONF_LORA_APP_PORT, clpp->getBuffer(), clpp->getSize(), MSG_UNCONFIRMED_FLAG);

    if (retcode < 0) 
    {
        retcode == LORAWAN_STATUS_WOULD_BLOCK ? printf("\r\n [MAIN] send() - WOULD BLOCK\r\n") : printf("\r\n send() - Error code %d \r\n", retcode);
        
        if (retcode == LORAWAN_STATUS_WOULD_BLOCK) 
        {
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) 
            {
                lorawan_event_queue.call_in(3s, send_message);
            }
        }
    }
    printf("\r\n [MAIN] %d bytes scheduled for transmission \r\n", retcode);

    clpp->reset();
};

#endif