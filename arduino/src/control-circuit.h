#ifndef CONTROL_CIRCUIT_H
#define CONTROL_CIRCUIT_H

#include "range-checker.h"

struct control_circuit {
  typedef void (*callback_t)();
  static void regulate_s(range_checker::event ev, void *context) {
    control_circuit *cc = reinterpret_cast<control_circuit *>(context);
    cc->regulate(ev);
  };

  control_circuit(int ain, int min, int max, int freq, callback_t cb)
      : _checker(ain, min, max, freq, control_circuit::regulate_s, this),
        _cb(cb) {}

  void init(void) { pinMode(_dout, OUTPUT); }

  void regulate(range_checker::event ev) {
    if (ev == range_checker::UNDER_RANGE) {
	_cb(range_checker::UNDER_RANGE)
    } else if (ev == range_checker::OVER_RANGE) {
	_cb(range_checker::OVER_RANGE)
    }
  }

  range_checker _checker;
  callback_t _cb;
};

#endif
