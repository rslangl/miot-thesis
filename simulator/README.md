# MIoT node simulation

This is a simple simulator used to demonstrate the behavior of the end-node. This application will iterate through the file `simdata.txt` line-by-line and subsequently publish the content in the same manner as the Data Integration application.

## Dependencies

* `mosquitto`
* `libmosquitto-dev`
* `nlohmann-json-dev`
* `g++` 
* `make`

## Usage

Run `make` to compile, and `./simulator` to execute
