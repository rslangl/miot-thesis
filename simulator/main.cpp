#include <iostream>
#include <fstream>
#include <sstream>
#include <mosquitto.h>
#include <thread>
#include <random>
#include <nlohmann/json.hpp>

#define PORT            1883
#define MQTT_SRV        "192.168.1.203"
#define CLEANSESSION    true
#define KEEPALIVE       60
#define QOS             0
#define TOPIC           "miot"
#define CLIENT_ID       13

using json = nlohmann::json;

static std::random_device seeder;
static std::mt19937 rng(seeder());
static std::uniform_int_distribution<int> gen(100, 5000);

void on_connect(struct mosquitto* mosq, void *obj, int rc);

void publish_message(struct mosquitto* mosq, const char* topic, size_t size, const char* msg, int qos);

int main(void)
{
    int rc, client_id = CLIENT_ID;

    mosquitto_lib_init();

    struct mosquitto* mqtt_client;

    mqtt_client = mosquitto_new("MIoT-Simulator", CLEANSESSION, &client_id);

    rc = mosquitto_connect(mqtt_client, MQTT_SRV, PORT, KEEPALIVE);

    if(rc != 0)
    {
        std::cout << "Client could not connect to broker, error code" << rc << std::endl;
        mosquitto_destroy(mqtt_client);
        return EXIT_FAILURE;
    }

    mosquitto_connect_callback_set(mqtt_client, on_connect);

    std::ifstream simdata("simdata.txt", std::ios::in);
    std::istringstream iss;
    std::string line;
    int rand;

    json* j = new json();

    std::string devEUI, callsign, health_status, msg;
    float lat, lon;

    if(simdata.is_open())
    {
        while(getline(simdata, line))
        {
            iss.clear();
            iss.str(line);
            iss >> devEUI >> callsign >> lat >> lon >> health_status;

            j->push_back({"devEUI", devEUI});
            j->push_back({"callsign", callsign});
            j->push_back({"lat", lat});
            j->push_back({"lon", lon});
            j->push_back({"health_status", health_status});

            msg = j->dump();

            publish_message(mqtt_client, TOPIC, msg.length(), msg.c_str(), QOS);

            j->clear();
            
            rand = gen(rng);

            std::this_thread::sleep_for(std::chrono::milliseconds(rand));
        }
    }
    else
    {
        std::cout << "Could not open file, exiting";
        return EXIT_FAILURE;
    }

    mosquitto_disconnect(mqtt_client);
    mosquitto_destroy(mqtt_client);

    mosquitto_lib_cleanup();

    return EXIT_SUCCESS;
}

void on_connect(struct mosquitto* mosq, void *obj, int rc)
{
    std::cout << "Client ID: " << *(int*) obj << std::endl;

    if(rc)
    {
        std::cerr << "Error: " << rc << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected" << std::endl;
}

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
