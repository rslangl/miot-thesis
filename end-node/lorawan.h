#ifndef LORAWAN_H_  
#define LORAWAN_H_

#include "lorawan_types.h"
#include <cstdio>
#include <cstdlib>
#include "lorawan/LoRaWANInterface.h"
#include "EventQueue.h"
#include "radio.h"

#define loraErrChk(ret) { loraAssert((ret), __FILE__, __LINE__); }
inline void loraAssert(lorawan_status_t ret, const char *file, int line)
{
    if(ret != LORAWAN_STATUS_OK)
    {
        printf("\r\n LoRa assert: %d %s %d \r\n", ret, file, line);
        exit(EXIT_FAILURE);
    }
}

static LoRaWANInterface lorawan(radio);

static events::EventQueue lorawan_event_queue(10 * EVENTS_EVENT_SIZE);
static lorawan_app_callbacks_t lorawan_callbacks;

#endif