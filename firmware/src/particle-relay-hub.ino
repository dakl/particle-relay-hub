/*
 * Project particle-relay-hub
 * Description: Control a particle relay board over http
 * Author: Daniel Klevebring
 * Date: 2018-01-04
 */

// ------------
// particle-relay-hub
// ------------

int RELAY1 = D3;
int RELAY2 = D4;
int RELAY3 = D5;
int RELAY4 = D6;

int STATES[] = {0, 0, 0, 0};

int ttl = 16777215;

void reset_handler()
{
    // tell the world what we are doing
    Particle.publish("reset", "going down for reboot NOW!", ttl);
}


int getRelayNumber(String command)
{
    // parse the relay number
    int relayNumber = command.charAt(1) - '0';
    // do a sanity check
    if (relayNumber < 1 || relayNumber > 4) return -1;
    else return relayNumber;
}

/*
command format r1,HIGH

POST /v1/devices/{DEVICE_ID}/relay

# EXAMPLE REQUEST
curl https://api.particle.io/v1/devices/0123456789abcdef/relay \
-d access_token=123412341234 -d params=r1,HIGH
*/
int relayControl(String command)
{
    int relayState = 0;
    // parse the relay number
    int relayNumber = getRelayNumber(command);
    if (relayNumber == -1) return -1;

    // find out the state of the relay
    if (command.substring(3,7) == "HIGH") relayState = 1;
    else if (command.substring(3,6) == "LOW") relayState = 0;
    else return -1;

    // write to the appropriate relay
    digitalWrite(relayNumber+2, relayState);

    // update states variable
    STATES[relayNumber-1] = relayState;

    return 1;
}

/*
command format r1

POST /v1/devices/{DEVICE_ID}/state

# EXAMPLE REQUEST
curl https://api.particle.io/v1/devices/0123456789abcdef/state \
-d access_token=123412341234 -d params=r1
*/
int relayState(String args)
{
    // parse the relay number
    int relayNumber = getRelayNumber(args);
    if (relayNumber == -1) return -1;

    return STATES[relayNumber-1];
}

void ready() {
    // code for displaying a notification that the system is ready
    // for example blinking each thing once
    Particle.publish("booted", "particle-relay-hub is ready.", ttl);
}

void setup()
{
    Particle.publish("booting", "particle-relay-hub is setting up.", ttl);

    // register the reset handler
    System.on(reset, reset_handler);

    //Initilize the relay control pins as output
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(RELAY3, OUTPUT);
    pinMode(RELAY4, OUTPUT);

    // Initialize all relays to an OFF state
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);
    digitalWrite(RELAY4, LOW);

    // register control function
    Particle.function("relay", relayControl);

    // register state function
    Particle.function("state", relayState);

    ready();
}

void loop() {
}