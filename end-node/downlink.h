#ifndef DOWNLINK_H_
#define DOWNLINK_H_

#include "emcon.h"
#include "mbed_assert.h"

typedef enum
{
    EMCON_ENABLE = 1036,
    EMCON_DISABLE = 1111,
    HEARTBEAT

} downlink_cmd_t;

static uint8_t rx_buffer[LORAMAC_PHY_MAXPAYLOAD];

void downlink_command_handler(const int cmd)
{
    switch(cmd)
    {
        case EMCON_ENABLE:
        {
            set_emcon(true);
            break;
        }
        case EMCON_DISABLE:
        {
            set_emcon(false);
            break;
        }
        case HEARTBEAT:
        {
            //lorawan.send(MBED_CONF_LORA_APP_PORT, (uint8_t*) "pong", 4, MSG_UNCONFIRMED_FLAG);
            //PONG;
            break;
        }
        default:
            MBED_ASSERT("Unknown command");
    }
};

static void receive_message()
{
    uint8_t port;
    int flags, chksum = 0;
    int16_t retcode;

    retcode = lorawan.receive(rx_buffer, sizeof(rx_buffer), port, flags);

    if (retcode < 0) {
        printf("\r\n [MAIN] receive() - Error code %d \r\n", retcode);
        return;
    }

    for(uint8_t i = 0; i < retcode; i++)
    {
        chksum += rx_buffer[i];
    }

    downlink_command_handler(chksum);
    
    memset(rx_buffer, 0, sizeof(rx_buffer));
};

#endif