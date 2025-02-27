// app.js

// map
var mapScript = document.createElement('script');
mapScript.src = 'https://maps.googleapis.com/maps/api/js?key=<your_key>&callback=initMap';
mapScript.async = true;
document.head.appendChild(mapScript);
var map;

// mqtt
var mqttScript = document.createElement('script');
mqttScript.src = "js/mqttws31.min.js";
mqttScript.async = true;
document.head.appendChild(mqttScript);
var mqtt;

// devices
var devices = [];

// custom markers
const icons = 
{
    FGINFCREW:
    {
        //icon: "../icons/friend-inf-crew.png",
        icon: "../icons/marker.png",
    },
    GATEWAY:
    {
        icon: "../icons/gateway.svg",
    },
    MEDEVAC:
    {
        icon: "../icons/medevac.png",
    },
    BIOHAZARD:
    {
        icon: "../icons/biohazard.png"
    },
};

// custom DL commands
const commands =
{
    ENABLE_RADIO_SILENCE:
    {
        text: "Enable radio silence",
        value: '{"confirmed":false,"fPort":1,"data":"ZW1jb246dHJ1ZQ=="}}',   //emcon:true
    },
    DISABLE_RADIO_SILENCE:
    {
        text: "Disable radio silence",
        value: '{"confirmed":false,"fPort":1,"data":"ZW1jb246ZmFsc2U="}}',  //emcon:false
    },
};

window.onload = function() 
{
    // init map
    map = new google.maps.Map(document.getElementById('map'), 
    {
        center: { lat: 59.648406, lng: 10.8272753 },
        zoom: 15,
        mapTypeId: "terrain",
        disableDefaultUI: true,
        mapTypeControl: false,
    });

    // init mqtt
    mqtt = new Paho.MQTT.Client("192.168.1.203", Number(9001), "MIoT-UI_" + parseInt(Math.random() * 100));
    mqtt.onMessageArrived = onMessageArrived;
    mqtt.onConnectionLost = onConnectionLost;
    mqtt.connect({onSuccess:onConnect});

    // populate commands list
    for(var command in commands)
    {
        var cmd = document.createElement("option");

        cmd.text = commands[command].text;
        cmd.value = commands[command].value;        

        document.getElementById("selectPayload").add(cmd);
    }
}

function onConnect() 
{
    console.log("MQTT client connected");
    mqtt.subscribe("miot");
}

function getDeviceSymbol(devEUI)
{
    return image =
    {
        url: icons.FGINFCREW.icon,
        scaledSize: new google.maps.Size(30, 40),   //95, 64 for APP-6A
        optimized: false,
    };
}

function uplinkDataHandler(data)
{
    var dev = 
    { 
        devEUI: "", 
        callsign: "",
        infowindow: null,
        biohazard:
        {
            dispatch_event: null,
            detections: [],
        },
        medical:
        {
            dispatch_event: null,
            health_status: null,
        },
        logistics:
        {
            dispatch_event: null,
            inventory_status: null,
        },
        prevPos: [] 
    };

    var tmpLat, tmpLon, tmpMed, tmpBio;

    // iterate the JSON data to extract key values
    data.forEach(element => 
    {
        switch(element[0]) 
        {
            case "devEUI":
                dev.devEUI = element[1];
                break;
            case "callsign":
                dev.callsign = element[1];
                break;
            case "lat":
                tmpLat = element[1];
                break;
            case "lon":
                tmpLon = element[1];
                break;
            case "health_status":
                tmpMed = element[1];    //dev.medical.health_status = element[1];
                break;
            case "biohazard_status":
                tmpBio = element[1];    //dev.biohazard.detections.add(element[1]);
                break;
            default:
                break;
        }
    });

    // defaults position values if not present in the MQTT payload
    if(tmpLon === undefined || tmpLat === undefined)
    {
        // TODO: replace with grayscale symbol while using last known position to indicate that data is received, but position cannot be updated at this time
        tmpLat = 59.648406;
        tmpLon = 10.8272753;
    }

    // add biometric status
    if(tmpMed === undefined)
    {
        tmpMed = "UNDEFINED";
    }
    dev.medical.health_status = tmpMed;

    // add gas readings if present
    if(tmpBio != undefined)
    {
        dev.medical.biohazard.detections.push(tmpBio);
    }

    var exists = false;

    // if device already exists -- update and store position
    devices.forEach(element => 
    {
        if(element.devEUI === dev.devEUI) 
        {
            var lat = element.position.getPosition().lat();
            var lng = element.position.getPosition().lng();

            element.prevPos.push({lat,lng});

            element.medical.health_status = dev.medical.health_status;

            element.biohazard = dev.biohazard;

            element.position.setPosition({ lat: tmpLat, lng: tmpLon });

            exists = true;
        }
    });

    // add device to list if it haven't been seen yet
    if(!exists) 
    {
        const contentString = '<div>CS: ' + dev.callsign + '</div>';

        const image = getDeviceSymbol(dev.devEUI);

        dev.position = new google.maps.Marker({
            title: dev.callsign,
            position: { lat: tmpLat, lng: tmpLon },
            icon: image,
        });

        dev.infowindow = new google.maps.InfoWindow({
            content: contentString,
        });

        dev.position.addListener("click", () => {
            dev.infowindow.open(map, dev.position);
        });

        devices.push(dev);

        var selectDeviceItem = document.createElement("option");
        selectDeviceItem.text = "CS: " + dev.callsign + " (DevEUI: " + dev.devEUI + ")";
        selectDeviceItem.value = dev.devEUI;

        document.getElementById("selectDevice").add(selectDeviceItem);
    }

    // update display
    devices.forEach(element => {

        console.log("CS: " + element.callsign + ", LAT: " + element.position.lat + ", LON: " + element.position.lon + ", BIO: " + element.medical.health_status);

        // draw trace
        if(element.prevPos.length > 0)
        {
            const trace = new google.maps.Polyline({
                path: element.prevPos,
                geodesic: true,
                strokeColor: "#FF0000",
                strokeOpacity: 1.0,
                strokeWeigth: 2,
            });

            trace.setMap(map);
        }

        // highlight device in case of unhealthy biometrics
        if(element.medical.health_status === "UNHEALTHY" && element.medical.dispatch_event === null)
        {
            var actionBtn = document.createElement("button");
            actionBtn.innerHTML = "Dispatch MEDEVAC";
            actionBtn.addEventListener("click", function(cs, lat, lon) {

                document.getElementById("eventpane").innerHTML += "<p>Medevac dispatched to CS: " + cs + ", at location LAT: " + lat + " LON: " + lon + "</p>";
                markMedevacSpot.setMap(map);

            }.bind(null, element.callsign, element.position.getPosition().lat(), element.position.getPosition().lng()));

            var infopane = document.getElementById("infopane");
            infopane.innerHTML += "<p>" + element.callsign + " is UNHEALTHY!</p>";
            infopane.appendChild(actionBtn);

            var markMedevacSpot = new google.maps.Marker({
                position: element.position.getPosition(),
                icon: {
                    url: icons.MEDEVAC.icon,
                    scaledSize: new google.maps.Size(40, 40),
                },
            });

            element.medical.dispatch_event = "MEDEVAC";

            //var animationOverlay = new google.maps.OverlayView();
            //animationOverlay.draw = function() {
            //    this.getPanes().markerLayer.id = "markerLayerHighlight";
            //};
            //animationOverlay.setMap(map);
        }

        // highlight device in case of biohazard detections
        if (element.biohazard.detections.length > 0 && element.biohazard.dispatch_event === null)
        {
            var actionBtn = document.createElement("button");
            actionBtn.innerHTML = "Dispatch CBRN";
            actionBtn.addEventListener("click", function(cs, lat, lon) {

                document.getElementById("eventpane").innerHTML += "<p>CBRN dispatched to CS: " + cs + ", at location LAT: " + lat + " LON: " + lon + "</p>";
                markCBRNSpot.setMap(map);

            }.bind(null, element.callsign, element.position.getPosition().lat(), element.position.getPosition().lng()));

            var infopane = document.getElementById("infopane");
            infopane.innerHTML += "<p>" + element.callsign + " has detected " + element.biohazard.detections + "!</p>"
            infopane.appendChild(actionBtn);

            var markCBRNSpot = new google.maps.Marker({
                position: element.position.getPosition(),
                icon: {
                    url: icons.BIOHAZARD.icon,
                    scaledSize: new google.maps.Size(40, 40),
                },
            });

            element.biohazard.dispatch_event = "BIOHAZARD";
        }

        element.position.setMap(map);

    });
}

document.getElementById("downlink-form").addEventListener("submit", function(evt) 
{
    evt.preventDefault();

    var device = document.getElementById("selectDevice").value;
    var payload = document.getElementById("selectPayload").value;

    if(device === null || payload === null)
    {
        alert("Fields cannot be empty");
    }

    var topic = "application/1/device/" + device + "/command/down";

    publishMessage(topic, payload);
})

function onMessageArrived(message) 
{
    var data = JSON.parse(message.payloadString);

    console.log("MQTT client received message: " + message.payloadString);

    uplinkDataHandler(data);
}

function onConnectionLost(responseObject) 
{
    if(responseObject.errorCode !== 0) 
    {
        console.log("MQTT connection lost: " + responseObject.errorMessage);
        mqtt.connect({onSuccess:onConnect});
    }
}

function publishMessage(topic, payload) 
{
    var message;

    message = new Paho.MQTT.Message(payload);
    message.destinationName = topic;
    message.qos = 0;

    mqtt.send(message);

    document.getElementById("eventpane").innerHTML += "<p>Downlink scheduled</p>";

    console.log("MQTT client published message: \"" + message.payloadString + "\" on topic: " + message.destinationName);
}
