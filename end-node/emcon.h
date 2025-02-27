#ifndef EMCON_H_
#define EMCON_H_

#include <cstdint>
#include <cstdio>
#include "DigitalOut.h"
#include "lorawan.h"
#include "uplink.h"

static bool emcon_active = false;

static mbed::DigitalOut emcon_led(D7);

static void toggle_emcon()
{
    if(emcon_active)
    {
        emcon_active = false;
        lorawan_event_queue.call(send_message);
    }
    else 
    {
        emcon_active = true;
    }
};

static void set_emcon(bool emcon)
{
    if(emcon)
    {
        printf("\r\n [EMCON] Activating \r\n");
        emcon_active = true;
    }
    else
    {
        printf("\r\n [EMCON] Deactivating \r\n");
        emcon_active = false;

        lorawan_event_queue.call(send_message);
    }
};

#endif