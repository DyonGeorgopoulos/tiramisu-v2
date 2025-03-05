#ifndef COMMON_H
#define COMMON_H

#include <SDL3/SDL.h>
#include "entity.h"


typedef struct AppState {
    SDL_Window* window;
    float last_tick;
    float current_tick;
    float delta_time;
  } AppState;

#endif