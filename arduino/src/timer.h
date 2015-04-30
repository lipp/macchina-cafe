#ifndef TIMER_H
#define TIMER_H

#include "Arduino.h"

#define MAX_TIMERS 5

struct timer {
  typedef void (*callback_t)(void *);
  timer(int freq, callback_t cb, void *context = NULL)
      : _freq(freq), _last(millis() + timer::_count), _cb(cb),
        _context(context) {
    _timers[_count] = this;
    _count++;
  };

  static void spin_all() {
    for (int i = 0; i < _count; ++i) {
      _timers[i]->spin();
    }
  };

  void spin() {
    unsigned long now = millis();
    if ((now - _last) > _freq) {
      _cb(_context);
      _last = now;
    }
  };

private:
  const unsigned long _freq;
  unsigned long _last;
  callback_t _cb;
  void *_context;
  static int _count;
  static timer *_timers[MAX_TIMERS];
};

#endif
