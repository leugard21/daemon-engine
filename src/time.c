#include "time.h"

#include "SDL2/SDL.h"

void time_init(void) { SDL_InitSubSystem(SDL_INIT_TIMER); }

double time_now_seconds(void) {
  static uint64_t freq = 0;
  if (freq == 0)
    freq = (uint64_t)SDL_GetPerformanceFrequency();

  uint64_t t = (uint64_t)SDL_GetPerformanceCounter();
  return (double)t / (double)freq;
}
