#ifndef Timers_h
#define Timers_h

#include "Arduino.h"
#include <functional>

typedef uint32_t Ticks_t;

enum class TimerType {
  OneShot = 0,
  Periodic = 1,
};

typedef void (*TimerCallback)(void *context);

struct Timer_t {
  Timer_t *next;
  // union
  // {
  //   struct {

  //   }
  // };
  std::function<void (void)> *lambda;
  TimerCallback callback;
  void *context;
  Ticks_t remainingTicks;
  Ticks_t initialTicks;
  bool running;
  bool reload;
  bool lambdaCallback;
};

class Timers {
private:
  Timer_t *start;
  Ticks_t lastTime;

public:
  Timers();
  void Run();
  Timer_t *Start(Ticks_t ticks, TimerCallback callback, void *context = NULL, TimerType type = TimerType::OneShot);
  Timer_t *Start(Ticks_t ticks, std::function<void(void)> lambda, TimerType type = TimerType::OneShot);
  void Stop(Timer_t *timer);
  void Pause(Timer_t *timer);
  void Resume(Timer_t *timer);
  Ticks_t ElapsedTime(Timer_t *timer);
};

#endif
