
#define LED_PIN 13

#include <ArduinoJetPeer.h>
#include <SerialClient.h>

JetPeer peer;

// wrap serial as "(Ethernet)Client"
SerialClient serialClient(Serial);

// analog ins state
JetState *ains;
JetState *webControlledState;

// reads all analog ins and converts
// them to a JSON Array
aJsonObject *read_ains() {
  aJsonObject *arr = aJson.createArray();
  for (int i = 0; i < 6; ++i) {
    aJson.addItemToArray(arr, aJson.createItem(analogRead(i)));
  }
  return arr;
}

aJsonObject *createClientsFetchRule() {
  aJsonObject *rule = aJson.createObject();
  aJsonObject *path = aJson.createObject();
  aJson.addItemToObject(rule, "path", path);
  aJson.addItemToObject(path, "startsWith", aJson.createItem("clients/"));
  return rule;
}

int clientCount = 0;

void watchClients(const char *path, const char *event, aJsonObject *val,
                  void *context) {
  String ev(event);
  if (ev.equals("add")) {
    if (clientCount == 0) {
      webControlledState->value(aJson.createItem((bool)true));
      digitalWrite(LED_PIN, HIGH);
    }
    ++clientCount;
  } else if (ev.equals("remove")) {
    --clientCount;
    if (clientCount == 0) {
      webControlledState->value(aJson.createItem((bool)false));
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void setup() {
  // init led
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);

  serialClient.waitHandshake();

  // handover serial to jet peer
  peer.init(serialClient);

  // create analog ins state
  // read-only
  webControlledState =
      peer.state("web-controled", aJson.createItem((bool)false));

  peer.fetch(createClientsFetchRule(), watchClients);
}

void loop() {
  // spin jet peer loop
  // eventually triggers function calls (state set)
  peer.loop();
}
