# Data Integration

This application acts as a intermediary between the LoRaWAN backend and the UI, where it will filter the incoming uplink data to a flat file containining a filtered message which will be published to the MQTT broker on topic `miot`. 

## Usage

### Installation

Clone the repository, and run `setup.sh` as `sudo`. This will compile the application and place the executable in `/opt/miot-data-integration/` and enable it as a service. The Mosquitto broker will be configured using two ports, namely the default 1883, and 9001 using the WebSockets protocol, which the UI will use.

### Service

Use `systemctl` (`stop`|`start`) using the handle `miot-data-integration.service`. You can view the log live by running the command `sudo journalctl -f -n 50 -u miot-data-integration.service`.

