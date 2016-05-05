
#include "timer.h"
#include "range-checker.h"

// timer ledTimer(1000, toggleLed);

// OUTPUT
const int HEIZUNG = 11;
const int VENTIL = 10;
const int PUMPE = 9;

// CONST Schwellwerte
const int _val_fuellstand = 600;
const int _val_kessel_pressure = 334;

// LED
const int output_LED = 6;

// INPUT
const int SCHALTER = 0;
const int FUELLSTAND = 1;
const int DRUCK_KESSEL = 2;
const int DRUCK_KREISLAUF = 3;

// TEMPS
int temp_SCHALTER;
int temp_FUELLSTAND;
int temp_DRUCK_KESSEL;
int temp_DRUCK_KREISLAUF;

// TIME
int long loop_TIME = 0;
int long print_TIME = 0;
int long pump_TIME = 0;
int long heat_TIME = 0;

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
  digitalWrite(output_LED, state);
  digitalWrite(HEIZUNG, state);
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
  pinMode(HEIZUNG, OUTPUT);
  pinMode(VENTIL, OUTPUT);
  pinMode(PUMPE, OUTPUT);
  pinMode(output_LED, OUTPUT);

  // init serial
  Serial.begin(9600); //  setup serial

  // init TEMPS
  temp_SCHALTER = 100;
  temp_FUELLSTAND = 0;
  temp_DRUCK_KESSEL = 0;
  temp_DRUCK_KREISLAUF = 0;
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

bool is_switch_on() { return analogRead(SCHALTER) > 1000; }

void activate_pump(bool enable) {
  int state = enable ? HIGH : LOW;
  digitalWrite(PUMPE, state);
}

bool is_enough_water(void) { return analogRead(FUELLSTAND) < _val_fuellstand; }

timer water_fill_timer(1000, verify_enough_water);

timer switch_timer(10, check_switch);

void open_vent(bool enable) {
  int state = enable ? HIGH : LOW;
  digitalWrite(VENTIL, state);
}

void print_debug(void *) {
  Serial.print(analogRead(DRUCK_KESSEL));
  Serial.print(" ");
  Serial.print(analogRead(DRUCK_KREISLAUF));
  Serial.print(" ");
  Serial.print(analogRead(SCHALTER));
  Serial.print(" ");
  Serial.print(digitalRead(VENTIL));
  Serial.print(" ");
  Serial.print(digitalRead(PUMPE));
  Serial.print(" ");
  Serial.println(analogRead(FUELLSTAND));
}

timer debug_timer(500, print_debug);

void loop() {
  // spins ALL timer based ops.
  timer::spin_all();
}
