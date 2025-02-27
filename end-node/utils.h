#ifndef UTILS_H_
#define UTILS_H_

#include "lorawan_types.h"
#include "mbed_stats.h"
#include <cstdio>
#include <cstdint>

#define SAMPLE_TIME_MS 2000

static uint64_t prev_idle_time = 0;

static void print_cpu_stats()
{
    mbed_stats_cpu_t stats;
    mbed_stats_cpu_get(&stats);

    uint64_t diff_usec = (stats.idle_time - prev_idle_time);
    uint8_t idle = (diff_usec * 100) / (SAMPLE_TIME_MS * 1000);
    uint8_t usage = 100 - ((diff_usec * 100) / (SAMPLE_TIME_MS * 1000));
    prev_idle_time = stats.idle_time;

    printf("\r\n=============== CPU stats ===============\n");
    printf("Time(us): \t\t\t%llu\n", stats.uptime);
    printf("Idle: \t\t\t\t%llu\n", stats.idle_time);
    printf("Sleep: \t\t\t\t%llu\n", stats.sleep_time);
    printf("DeepSleep: \t\t\t\t%llu\n", stats.deep_sleep_time);
    printf("Idle: %d%% Usage: %d%%\n\n", idle, usage);
};

static uint8_t battery_handler()
{
    /* Needs to adhere to the DevStatusAns MAC command
    *
    * if connected to power
    *   return 0
    * if cannot measure level
    *   return 255
    *
    * measure batterylevel, return in range 1 to 254 to indicate level
    */
    return 0;
};

#endif