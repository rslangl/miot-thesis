#include "BUILD/DISCO_L072CZ_LRWAN1/ARMC6/mbed_config.h"
#include "mbed.h"
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "lorawan.h"
#include "emcon.h"
#include "sensor.h"
#include "utils.h"
#include "downlink.h"
#include "uplink.h"

#include "PushbuttonController.h"

#define DEFAULT_PAYLOAD_SIZE            51
#define TX_TIMER                        10s
#define MAX_NUMBER_OF_EVENTS            10
#define CONFIRMED_MSG_RETRY_COUNTER     3
#define ADR                             0

static void lora_event_handler(lorawan_event_t event);

static PushbuttonController local_emcon_control(PB_2, &toggle_emcon);

int main()
{
    init_payload(DEFAULT_PAYLOAD_SIZE);

    loraErrChk(lorawan.initialize(&lorawan_event_queue));
    printf("\r\n [MAIN] Mbed LoRaWANStack initialized \r\n");

    lorawan_callbacks.events = mbed::callback(lora_event_handler);
    //lorawan_callbacks.battery_level = mbed::callback(battery_handler);
    lorawan.add_app_callbacks(&lorawan_callbacks);

    loraErrChk(lorawan.set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER));
    printf("\r\n [MAIN] CONFIRMED message retries : %d \r\n", CONFIRMED_MSG_RETRY_COUNTER);

#if (ADR)
    loraErrChk(lorawan.enable_adaptive_datarate());
    printf("\r\n [MAIN] Adaptive data rate (ADR) - Enabled \r\n");
#endif

    lorawan_status_t retcode = lorawan.connect();
    if (retcode == LORAWAN_STATUS_OK || retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS) 
    {

    } 
    else 
    {
        printf("\r\n [MAIN] Connection error, code = %d \r\n", retcode);
        return EXIT_FAILURE;
    }
    printf("\r\n [MAIN] Connection - In Progress ...\r\n");

    lorawan_event_queue.dispatch_forever();

    print_cpu_stats();

    return EXIT_SUCCESS;
}

static void lora_event_handler(lorawan_event_t event)
{
    switch (event) 
    {
        case CONNECTED:
        {
            printf("\r\n [MAIN] Connection - Successful \r\n");

            if(MBED_CONF_LORA_DUTY_CYCLE_ON)
            {
                send_message();
            }
            else 
            {
                lorawan_event_queue.call_every(TX_TIMER, send_message);
            }

            break;
        }
        case DISCONNECTED:
        {
            lorawan_event_queue.break_dispatch();
            printf("\r\n [MAIN] Disconnected Successfully \r\n");
            break;
        }
        case TX_DONE:
        {
            printf("\r\n [MAIN] Message Sent to Network Server \r\n");

            print_cpu_stats();

            sleep();

            if(emcon_active)
            {
                printf("\r\n [MAIN] EMCON active, will not transmit \r\n");
                lorawan_event_queue.call(print_cpu_stats);
            }
            else 
            {            
                if(MBED_CONF_LORA_DUTY_CYCLE_ON)
                {
                    send_message();
                }
                else 
                {
                    lorawan_event_queue.call_every(TX_TIMER, send_message);
                }
            }
            break;
        }
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
        {
            printf("\r\n [MAIN] Transmission Error - EventCode = %d \r\n", event);

            // try again
            if(emcon_active)
            {
                printf("\r\n [MAIN] EMCON active, will not transmit \r\n");
                lorawan_event_queue.call(print_cpu_stats);
            }
            else 
            {
                if (MBED_CONF_LORA_DUTY_CYCLE_ON) 
                {
                    send_message();
                }
            }
            break;
        }
        case RX_DONE:
        {
            printf("\r\n [MAIN] Received message from Network Server \r\n");
            receive_message();
            break;
        }
        case RX_TIMEOUT:
        case RX_ERROR:
        case JOIN_FAILURE:
        {
            printf("\r\n [MAIN] OTAA Failed - Check Keys \r\n");
            break;
        }
        case UPLINK_REQUIRED:
        {
            printf("\r\n [MAIN] Uplink required by NS \r\n");

            if(emcon_active)
            {
                printf("\r\n [MAIN] EMCON active, will not transmit \r\n");
                lorawan_event_queue.call(print_cpu_stats);
            }
            else 
            {
                if (MBED_CONF_LORA_DUTY_CYCLE_ON) 
                {
                    if(MBED_CONF_LORA_DUTY_CYCLE_ON)
                    {
                        PONG;
                    }
                }
            }
            break;
        }
        default:
            MBED_ASSERT("Unknown LoRaWAN Event");
    }
}