#!/bin/bash

set -e

echo "+++ Pre-flight checks +++"

if [ `whoami` != root ]
then
    echo "Please run this script as root or sudo"
    exit
fi

sleep 1

echo "+++ Install dependencies +++"
apt-get install mosquitto libmosquitto-dev nlohmann-json-dev g++ make policykit-1 -y

echo "+++ Build application +++"

make

# Create directory to place the binary
mkdir -p /opt/miot-data-integration
INSTALL_DIR=/opt/miot-data-integration
mv application $INSTALL_DIR

sleep 1

echo "+++ Copy configurations +++"

# Make the binary callable from command line
ln -s $INSTALL_DIR/application /usr/local/bin
# Copy conf file to correct location
cp default.conf /etc/mosquitto/conf.d/

sleep 1

echo "+++ Enable service +++"

systemctl enable mosquitto

cp miot-data-integration.service /etc/systemd/system/
systemctl enable miot-data-integration.service
systemctl start miot-data-integration.service

sleep 1

echo "+++ Cleanup +++"

cd ..

rm -R -f dataintegration/