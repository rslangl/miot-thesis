#!/bin/bash

set -e

echo "+++ Pre-flight checks +++"

# Check that the executing user is privileged
if [ `whoami` != root]
then
	echo "Please run this script as root or sudo"
	exit
fi

sleep 1

# Use LNS URI from command line or default if not set
if [ $# -eq 1 ]
then
	echo $1 > tc.uri
else
	echo "No LNS URI provided, using defaults"
fi

# Install deps
echo "+++ Installing dependencies +++"
apt-get install git gcc make -y

# Get the source code
echo "+++ Fetching source code +++"
cd /opt/
git clone https://github.com/lorabasics/basicstation.git
cd basicstation

WORKING_DIR=pwd
INSTALL_DIR=/opt/basicstation

sleep 1

# Set DevEUI and hostname
echo "+++ Configure local settings +++"
GATEWAY_EUI_NIC="eth0"
GATEWAY_EUI=$(ip link show $GATEWAY_EUI_NIC | awk '/ether/ {print $2}' | awk -F\: '{printf $1$2$3"FFFE"$4$5$6}')
GATEWAY_EUI=${GATEWAY_EUI^^}
CURRENT_HOSTNAME=$(hostname)
NEW_HOSTNAME="lorawan-gateway"
echo $NEW_HOSTNAME > /etc/hostname
sed -i "s/$CURRENT_HOSTNAME/$NEW_HOSTNAME/" /etc/hosts

sleep 1

# Facilitate logging
touch /var/log/station.log
chmod 777 /var/log/station.log

sleep 1

# Build
echo "+++ Building for Raspberry Pi with IMST concentrator +++"
make platform=rpi variant=std

sleep 1

# Uncomment to Set permanent envvar -- otherwise the SPI settings in station.conf will be used
#echo "export RADIODEV=/dev/spidev0.0" >> /etc/profile.d/station.sh

# Make the binary callable from command line
ln -s $INSTALL_DIR/build-rpi-std/bin/station /usr/local/bin
# Copy config files
cp $WORKING_DIR/station.conf $INSTALL_DIR/build-rpi-std/bin
cp $WORKING_DIR/tc.uri $INSTALL_DIR/build-rpi-std/bin
# Edit gateway EUI in station.conf
sed -i "s/GATEWAY_EUI_PLACEHOLDER/$GATEWAY_EUI/" $INSTALL_DIR/build-rpi-std/bin/station.conf

sleep 1

echo "+++ Enable service +++"
cp ./lorawan-gateway.service /lib/systemd/system/
systemctl enable lorawan-gateway.service

echo "DevEUI: $GATEWAY_EUI"
echo "Hostname: $NEW_HOSTNAME"
echo "Installation complete, rebooting..."

sleep 5

reboot
