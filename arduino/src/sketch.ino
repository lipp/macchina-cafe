
#define LED_PIN 13

#include <ArduinoJetPeer.h>
#include <SerialClient.h>

JetPeer peer;

// wrap serial as "(Ethernet)Client"
SerialClient serialClient(Serial);

// analog ins state
JetState* ains;

// reads all analog ins and converts
// them to a JSON Array
aJsonObject* read_ains() {
	aJsonObject* arr = aJson.createArray();
	for(int i = 0; i < 6; ++i) {
		aJson.addItemToArray(arr, aJson.createItem(analogRead(i)));
	}
	return arr;
};

void serial_handshake() {
/*	while(1) {
		if (Serial.available()) {
			Serial.read();
			digitalWrite(LED_PIN, HIGH);
			return;
		} else {
			digitalWrite(LED_PIN, LOW);
			delay(1000);
			digitalWrite(LED_PIN, HIGH);
			delay(1000);
		}	
	} */
	delay(5000);
}

void setup() {
	// init led
	pinMode(LED_PIN, OUTPUT);

	Serial.begin(115200);

	serial_handshake();

	// handover serial to jet peer	
	peer.init(serialClient);

	// create analog ins state
	// read-only
	ains = peer.state("ains", read_ains());
}

void loop() {
	digitalWrite(LED_PIN, HIGH);

	// spin jet peer loop 
	// eventually triggers function calls (state set)
	peer.loop();

	// post new analog in values
//	ains->value(read_ains());

	// dont make too much pressure on serial 
	// interface. 50ms seems max possible.
	digitalWrite(LED_PIN, LOW);
	delay(50);
}
