#include <Ethernet.h>
#include <ArduinoHA.h>

#define INPUT_PIN_FRONT      9
#define INPUT_PIN_BACK       8
#define BROKER_ADDR          IPAddress(192,168,0,3)
#define BROKER_USERNAME      "arduino" // replace with your credentials
#define BROKER_PASSWORD      "foobar"

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
unsigned long lastReadAt = millis();
//unsigned long lastAvailabilityToggleAt = millis();
bool lastInputStateFront = false;
bool lastInputStateBack = false;

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "input" may be anything you want to be displayed in HA panel
// "door" is device class (based on the class HA displays different icons in the panel)
// "true" is initial state of the sensor. In this example it's "true" as we use pullup resistor
HABinarySensor sensor_front("pir_front", "occupancy", false);
HABinarySensor sensor_back("pir_back", "occupancy", false);

void setup() {
    pinMode(INPUT_PIN_FRONT, INPUT);
    pinMode(INPUT_PIN_BACK, INPUT);
    lastInputStateFront = digitalRead(INPUT_PIN_FRONT);
    lastInputStateBack  = digitalRead(INPUT_PIN_BACK);

    // you don't need to verify return status
    Ethernet.begin(mac);

    // turn on "availability" feature
    // this method also sets initial availability so you can use "true" or "false"
//    sensor_front.setAvailability(false);
//    sensor_back.setAvailability(false);

    lastReadAt = millis();
//    lastAvailabilityToggleAt = millis();

    // set device's details (optional)
    device.setName("Pir Sensors Flur");
    device.setSoftwareVersion("1.0.2");

    mqtt.begin(BROKER_ADDR, BROKER_USERNAME, BROKER_PASSWORD);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    if ((millis() - lastReadAt) > 30) { // read in 30ms interval
        // library produces MQTT message if a new state is different than the previous one
        sensor_front.setState(digitalRead(INPUT_PIN_FRONT));
        sensor_back.setState(digitalRead(INPUT_PIN_BACK));
        lastInputStateFront = sensor_front.getState();
        lastInputStateBack  = sensor_back.getState();
        lastReadAt = millis();
    }

//        if ((millis() - lastAvailabilityToggleAt) > 5000) {
//        sensor_front.setAvailability(!sensor_front.isOnline());
//        sensor_back.setAvailability(!sensor_back.isOnline());
//        lastAvailabilityToggleAt = millis();
//    }
}
