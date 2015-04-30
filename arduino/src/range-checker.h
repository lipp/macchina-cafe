#ifndef RANGE_CHECKER_H
#define RANGE_CHECKER_H

#include "timer.h"

struct range_checker {
  static void check_range(void *rcv) {
    range_checker *rc = reinterpret_cast<range_checker *>(rcv);
    rc->check();
  }

  enum event { IN_RANGE, UNDER_RANGE, OVER_RANGE, UNKNOWN_RANGE };

  typedef void (*eventcb_t)(event ev, void *context);

  range_checker(int ain, int min, int max, int freq, eventcb_t cb,
                void *context = NULL)
      : _ain(ain), _min(min), _max(max),
        _timer(freq, range_checker::check_range, this), _state(UNKNOWN_RANGE),
        _cb(cb), _context(context) {
    check();
  }

  void spin() { _timer.spin(); };

private:
  void check() {
    int value = analogRead(_ain);
    if ((value > _max) && (_state != OVER_RANGE)) {
      _cb(OVER_RANGE, _context);
      _state = OVER_RANGE;
    } else if ((value < _min) && (_state != UNDER_RANGE)) {
      _cb(UNDER_RANGE, _context);
      _state = UNDER_RANGE;
    } else if (((value >= _min) && (value <= _max)) && (_state != IN_RANGE)) {
      _cb(IN_RANGE, _context);
      _state = IN_RANGE;
    }
  }
  int _ain;
  int _min;
  int _max;
  event _state;

  timer _timer;
  eventcb_t _cb;
  void *_context;
};

#endif
