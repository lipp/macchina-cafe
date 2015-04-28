
#define LED_PIN 13

#include <ArduinoJetPeer.h>
#include <SerialClient.h>
#include "timer.h"
#include "range-checker.h"

JetPeer peer;

// wrap serial as "(Ethernet)Client"
SerialClient serialClient(Serial);

// analog ins state
JetState *ains;
JetState *webControlledState;

// reads all analog ins and converts
// them to a JSON Array
void update_ains(void *) {
  aJsonObject *arr = aJson.createArray();
  for (int i = 0; i < 6; ++i) {
    aJson.addItemToArray(arr, aJson.createItem(analogRead(i)));
  }
  ains->value(arr);
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
      // digitalWrite(LED_PIN, HIGH);
    }
    ++clientCount;
  } else if (ev.equals("remove")) {
    --clientCount;
    if (clientCount == 0) {
      webControlledState->value(aJson.createItem((bool)false));
      // digitalWrite(LED_PIN, LOW);
    }
  }
}

int led = 0;

void toggleLed(void *) {
  if (led) {
    led = 0;
    digitalWrite(LED_PIN, LOW);
  } else {
    led = 1;
    digitalWrite(LED_PIN, HIGH);
  }
}

timer ledTimer(1000, toggleLed);
timer ainTimer(200, update_ains);

void regulate_pressure(range_checker::event ev) {
  if (ev == range_checker::UNDER_RANGE) {

  } else {
  }
}

range_checker pressure_checker(1,   // ain 1
                               230, // ok range min
                               400, // ok range max
                               200, // 200ms delta
                               regulate_pressure);

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
  timer::spin_all();
}
