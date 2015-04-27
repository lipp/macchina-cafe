#ifndef TIMER_H
#define TIMER_H

struct timer {
  typedef void (*callback_t)(void *);
  timer(int freq, callback_t cb, void *context = NULL)
      : _freq(freq), _last(millis()), _cb(cb), _context(context){};

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
};

#endif
