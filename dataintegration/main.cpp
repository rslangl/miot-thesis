#include <stdlib.h>
#include <iostream>
#include <inttypes.h>
#include <mosquitto.h>
#include <unistd.h>
#include <signal.h>
#include <nlohmann/json.hpp>

#define PORT            1883
#define MQTT_SRV        "192.168.1.203"
#define CLEANSESSION    true
#define KEEPALIVE       60
#define QOS             0
#define SUB_TOPIC       "application/1/device/+/event/up"
#define GW_TOPIC        "gateway/+/event/+"
#define PUB_TOPIC       "miot"
#define CLIENT_ID       12

using json = nlohmann::json;

std::string get_callsign(std::string devEUI);

template<typename T>
void build_message(json* j, std::string key, T value);

void uplink_handler(struct mosquitto* mosq, const struct mosquitto_message* msg);

void downlink_handler(struct mosquitto* mosq, const struct mosquitto_message* msg);

void publish_message(struct mosquitto* mosq, const char* topic, size_t size, const char* msg, int qos);

void on_connect(struct mosquitto* mosq, void *obj, int rc);

void on_message(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg);

void sighandler(int sig);

volatile sig_atomic_t done = 0;

int main(void)
{
    signal(SIGINT, sighandler);

    int client_id = CLIENT_ID;

    mosquitto_lib_init();

    struct mosquitto* mqtt_client;

    // init MQTT instance
    mqtt_client = mosquitto_new("MIoT-DataIntegration", CLEANSESSION, &client_id);

    // connect
    if(mosquitto_connect(mqtt_client, MQTT_SRV, PORT, KEEPALIVE) != MOSQ_ERR_SUCCESS)
    {
        std::cout << "Client could not connect to broker" << std::endl;
        mosquitto_destroy(mqtt_client);
        exit(EXIT_FAILURE);
    }
   
    // specify callbacks
    mosquitto_connect_callback_set(mqtt_client, on_connect);
    mosquitto_message_callback_set(mqtt_client, on_message);

    // start MQTT thread
    if(mosquitto_loop_start(mqtt_client) != MOSQ_ERR_SUCCESS)
    {
        std::cout << "Error starting client loop, exiting" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    while(!done);

    // terminate gracefully
    mosquitto_loop_stop(mqtt_client, true);

    mosquitto_disconnect(mqtt_client);
    mosquitto_destroy(mqtt_client);

    mosquitto_lib_cleanup();

    return EXIT_SUCCESS;
}

/**
 * Helper function that fetches the callsign associated with the passed DevEUI
 */
std::string get_callsign(std::string devEUI)
{
    return "A-6-A-5";
}

/**
 * Callback function for connection established 
 */
void on_connect(struct mosquitto* mosq, void* obj, int rc)
{
    std::cout << "Client ID: " << *(int*) obj << std::endl;

    if(rc)
    {
        std::cerr << "Error: " << rc << std::endl;
        exit(EXIT_FAILURE);
    }
    
    if(mosquitto_subscribe(mosq, NULL, SUB_TOPIC, QOS) != MOSQ_ERR_SUCCESS)
    {
        std::cout << "Error connecting to broker, exiting" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected" << std::endl;
}

/**
 * Helper function for building the outbound JSON payload
 */
template<typename T>
void build_msg(json* j, std::string key, T value)
{
    j->push_back({key, value});
}

/**
 * Interprets biometric data
 */
std::string interpret_biometric_data(int bpm, int muap)
{
    std::string status;

    if((bpm > 60 && bpm < 100) && (muap <= 50))
    {
        status = "HEALTHY";
    }
    else if(bpm > 100 && muap > 50)
    {
        status = "EXHAUSTED";
    } 
    else if(bpm > 100 && muap <= 50)
    {
        status = "UNHEALTHY";
    }
    else
    {
        status = "UNDEFINED";
    }

    return status;
}

/**
 * Handler for all uplink messages
 */
void uplink_handler(struct mosquitto* mosq, const struct mosquitto_message* msg)
{
    std::cout << "Received uplink";

    json* j_msg = new json();

    auto j_in = json::parse((char*) msg->payload);
    
    build_msg<std::string>(j_msg, "direction", "uplink");

    try
    {
        // devEUI and associated callsign
        auto devEUI = j_in.at("devEUI").get<std::string>();
        std::string cs = get_callsign(devEUI);

        build_msg<std::string>(j_msg, "devEUI", devEUI);
        build_msg<std::string>(j_msg, "callsign", cs);

        std::cout << ", ID-data: DevEUI = " << devEUI << " CS = " << cs;
    }
    catch(const std::exception& e)
    {
        std::cout << ", no ID data";
    }

    try
    {
        // location
        auto lat = j_in.at("object").at("gpsLocation").at("136").at("latitude").get<float>();
        auto lon = j_in.at("object").at("gpsLocation").at("136").at("longitude").get<float>();

        build_msg<float>(j_msg, "lat", lat);
        build_msg<float>(j_msg, "lon", lon);

        std::cout << ", NS: Lat = " << lat << " Lon = " << lon;
    }
    catch(const std::exception& e)
    {
        std::cout << ", no NS data";
    }
    
    try
    {
        // BPM and MUAP
        auto bpm = j_in.at("object").at("digitalInput").at("0").get<int>();
        auto muap = j_in.at("object").at("digitalOutput").at("1").get<int>();

        std::string health_status = interpret_biometric_data(bpm, muap);

        build_msg<std::string>(j_msg, "health_status", health_status);

        //build_msg<int>(j_msg, "BPM", bpm);
        //build_msg<int>(j_msg, "MUAP", muap);

        std::cout << ", BIO: BPM = " << bpm << " MUAP = " << muap;
    }
    catch(const std::exception& e)
    {
        std::cout << ", no BIO data";
    }
    
    std::cout << std::endl;

    std::string fwd_msg = j_msg->dump();

    publish_message(mosq, PUB_TOPIC, fwd_msg.length(), fwd_msg.c_str(), 0);

    delete j_msg;
}

/**
 * Handler for all downlink messages
 */
void downlink_handler()
{

}

/**
 * Wrapper function for publishing message
 */
void publish_message(struct mosquitto* mosq, const char* topic, size_t size, const char* msg, int qos)
{
    int retcode = mosquitto_publish(mosq, NULL, topic, size, msg, qos, false);

    if(retcode == MOSQ_ERR_SUCCESS)
    {
        std::cout << "Published message (" << size << " bytes): \"" << msg << "\"" << std::endl;
    }
    else
    {
        std::cout << "Error publishing message, error code " << retcode << std::endl;
    }
}

/**
 * Callback function for received messages
 */
void on_message(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg)
{
    uplink_handler(mosq, msg);
}

/**
 * Interrupt handler, temrinates on all signals
 */
void sighandler(int sig)
{
    std::cout << "Caught signal " << sig << ", terminating" << std::endl;

    done = 1;
}