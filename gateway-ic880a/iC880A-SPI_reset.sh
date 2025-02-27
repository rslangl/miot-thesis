#!/bin/bash

# RPi physical RSTPIN = 22, RPi logical RSTPIN = GPIO17, iC880A RSTPIN = 13
RSTPIN=17

echo "RSTPIN" > /sys/class/gpio/export
echo "out" > /sys/class/gpio$RSTPIN/direction
echo "1" > /sys/class/gpio$RSTPIN/value

sleep 5

echo "0" > /sys/class/gpio/gpio$RSTPIN/value

sleep 1

echo "0" > /sys/class/gpio$RSTPIN/value
