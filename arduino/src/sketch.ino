
#include "timer.h"
#include "range-checker.h"

// OUTPUT
const int HEATER_PIN = 11;
const int VENT_PIN = 10;
const int PUMP_PIN = 9;

// CONST LIMITS
const int FILL_LIMIT = 600;
const int PRESSURE_LIMIT = 334;

// LED
const int LED_PIN = 6;

// INPUT
const int SWITCH_PIN = 0;
const int FILL_PIN = 1;
const int PRESSURE_BOILER_PIN = 2;
const int PRESSURE_CIRCUIT_PIN = 3;


static const int boiler_ain = 2;
static const int boiler_dout = 11;
static const int boiler_dt = 10;
static const int boiler_min = 339;
static const int boiler_max = 340;
enum event { IN_RANGE, UNDER_RANGE, OVER_RANGE, UNKNOWN_RANGE };

void enable_heater(bool enable) {
  int state;
  if (enable) {
    state = HIGH;
  } else {
    state = LOW;
  }
  digitalWrite(LED_PIN, state);
  digitalWrite(HEATER_PIN, state);
}

void boiler_event_handler(range_checker::event ev, void *) {
  if (ev == range_checker::UNDER_RANGE) {
    enable_heater(true);
  } else if (ev == range_checker::OVER_RANGE) {
    enable_heater(false);
  }
}

range_checker boiler_circuit(boiler_ain, boiler_min, boiler_max, boiler_dt,
                             boiler_event_handler);

// Initialize Values
void setup() {
  // init IO
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(VENT_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // init serial
  Serial.begin(9600); //  setup serial

}

void verify_enough_water(void *) {
  if (!is_enough_water()) {
    activate_pump(true);
    open_vent(true);
  } else {
    activate_pump(false);
    open_vent(false);
  }
}

void check_switch(void *) {
  if (!is_enough_water()) {
    return;
  }
  if (is_switch_on()) {
    activate_pump(true);
    open_vent(false);
  } else {
    activate_pump(false);
    open_vent(false);
  }
}

bool is_switch_on() { return analogRead(SWITCH_PIN) > 1000; }

void activate_pump(bool enable) {
  int state = enable ? HIGH : LOW;
  digitalWrite(PUMP_PIN, state);
}

bool is_enough_water(void) { return analogRead(FILL_PIN) < FILL_LIMIT; }

timer water_fill_timer(1000, verify_enough_water);

timer switch_timer(10, check_switch);

void open_vent(bool enable) {
  int state = enable ? HIGH : LOW;
  digitalWrite(VENT_PIN, state);
}

void print_debug(void *) {
  Serial.print(analogRead(PRESSURE_BOILER_PIN));
  Serial.print(" ");
  Serial.print(analogRead(PRESSURE_CIRCUIT_PIN));
  Serial.print(" ");
  Serial.print(analogRead(SWITCH_PIN));
  Serial.print(" ");
  Serial.print(digitalRead(VENT_PIN));
  Serial.print(" ");
  Serial.print(digitalRead(PUMP_PIN));
  Serial.print(" ");
  Serial.println(analogRead(FILL_PIN));
}

timer debug_timer(500, print_debug);

void loop() {
  // spins ALL timer based ops.
  timer::spin_all();
}
