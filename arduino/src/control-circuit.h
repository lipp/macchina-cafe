#ifndef CONTROL_CIRCUIT_H
#define CONTROL_CIRCUIT_H

#include "range-checker.h"

struct control_circuit {
  static void regulate_s(range_checker::event ev, void *context) {
    control_circuit *cc = reinterpret_cast<control_circuit *>(context);
    cc->regulate(ev);
  };

  control_circuit(int ain, int min, int max, int freq, int dout)
      : _checker(ain, min, max, freq, control_circuit::regulate_s, this),
        _dout(dout) {}

  void init(void) { pinMode(_dout, OUTPUT); }

  void regulate(range_checker::event ev) {
    if (ev == range_checker::UNDER_RANGE) {
      digitalWrite(_dout, HIGH);
    } else if (ev == range_checker::OVER_RANGE) {
      digitalWrite(_dout, LOW);
    }
  }

  range_checker _checker;
  int _dout;
};

#endif
