/*
 * Project particle-relay-hub
 * Description: Control a particle relay board over http
 * Author: Daniel Klevebring
 * Date: 2018-01-04
 */

// ------------
// particle-relay-hub
// ------------

#include "MQTT.h"

/* ======================= prototypes =============================== */

void callback(char *topic, byte *payload, unsigned int length);
int setState(int relayNumber, int relayState);

/* ======================= main.ino ======================== */

MQTT client("worker0", 1883, callback);

char *COMMANDS_TOPICS = "commands/relay/#";
char *EVENTS_TOPIC_TEMPLATE = "events/relay/";

int PINS[] = {D3, D4, D5, D6};
int STATES[] = {0, 0, 0, 0};

int FAILED_TO_PARSE_PIN = -100;
int FAILED_TO_PARSE_STATE = -200;
int ttl = 16777215;

void callback(char *_topic, byte *payload, unsigned int length)
{
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    String topic = String(_topic);
    int relayNumber = topic.charAt(topic.length() - 1) - '0';

    if (String("ON").equals(p))
    {
        setState(relayNumber, HIGH);
        client.publish(String(EVENTS_TOPIC_TEMPLATE) + String(relayNumber), "ON");
    }
    else if (String("OFF").equals(p))
    {
        setState(relayNumber, LOW);
        client.publish(String(EVENTS_TOPIC_TEMPLATE) + String(relayNumber), "OFF");
    }
}

void reset_handler()
{
    // tell the world what we are doing
    Particle.publish("reset", "going down for reboot NOW!", ttl);
}

int setState(int relayNumber, int relayState)
{
    // write to the appropriate relay
    digitalWrite(PINS[relayNumber - 1], relayState);

    // update states variable
    STATES[relayNumber - 1] = relayState;

    return 0;
}

void ready()
{
    // code for displaying a notification that the system is ready
    // for example blinking each thing once
    Particle.publish("booted", "particle-relay-hub is ready.", ttl);
}

int getRelayNumber(String args)
{
    Particle.publish("get_pin", "Getting pin index from args " + args, ttl);
    // parse the relay number
    int pin = args.charAt(0) - '0';
    Particle.publish("get_pin", "Pin was " + String(pin) , ttl);    
    // do a sanity check
    if (pin < 1 || pin > 4) return -1;
    else return pin;
}

int getTargetState(String args)
{
    unsigned int pin, value;    
    int n = sscanf(args, "%u,%u", &pin, &value);
    Particle.publish("got_value", "value is " + String(value), ttl);    
    // do a sanity check
    if (value < 0 || value > 1) return -1;
    else return value;
}

/*
command format: pin,state[0=LOW, 1=HIGH]
examples: `1,1`, `4,0`
POST /v1/devices/{DEVICE_ID}/pin
# EXAMPLE REQUEST
curl https://api.particle.io/v1/devices/0123456789abcdef/pin \
-d access_token=123412341234 -d params=1,1
*/
int pinControl(String args)
{
    Particle.publish("set_state", "Setting state using args " + args, ttl);
    // parse the relay number
    int targetRelay = getRelayNumber(args);
    if (targetRelay == -1) return FAILED_TO_PARSE_PIN;

    // find out the state of the relay
    int targetState = getTargetState(args);
    if (targetState == -1) return FAILED_TO_PARSE_STATE;
    setState(targetRelay, targetState);
    return 1;
 }

void setup()
{
    Particle.publish("booting", "particle-relay-hub is setting up.", ttl);

    // register the reset handler
    System.on(reset, reset_handler);

    for (int pin : PINS)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    // register control function
    Particle.function("pin", pinControl);

    // connect to the mqtt broker
    client.connect("relay-hub");

    // publish/subscribe
    if (client.isConnected())
    {
        Particle.publish("MQTT: connected", PRIVATE);

        client.publish(String(EVENTS_TOPIC_TEMPLATE) + "general", "Particle relay hub came online.");
        client.subscribe(COMMANDS_TOPICS);
    }

    ready();
}

void loop()
{
    if (client.isConnected())
        client.loop();
}