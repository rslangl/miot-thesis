#ifndef RADIO_H_
#define RADIO_H_

#include "SX1276_LoRaRadio.h"

SX1276_LoRaRadio radio(MBED_CONF_SX1276_LORA_DRIVER_SPI_MOSI,
                       MBED_CONF_SX1276_LORA_DRIVER_SPI_MISO,
                       MBED_CONF_SX1276_LORA_DRIVER_SPI_SCLK,
                       MBED_CONF_SX1276_LORA_DRIVER_SPI_CS,
                       MBED_CONF_SX1276_LORA_DRIVER_RESET,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO0,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO1,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO2,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO3,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO4,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO5,
                       MBED_CONF_SX1276_LORA_DRIVER_RF_SWITCH_CTL1,
                       MBED_CONF_SX1276_LORA_DRIVER_RF_SWITCH_CTL2,
                       MBED_CONF_SX1276_LORA_DRIVER_TXCTL,
                       MBED_CONF_SX1276_LORA_DRIVER_RXCTL,
                       MBED_CONF_SX1276_LORA_DRIVER_ANT_SWITCH,
                       MBED_CONF_SX1276_LORA_DRIVER_PWR_AMP_CTL,
                       MBED_CONF_SX1276_LORA_DRIVER_TCXO);

#endif