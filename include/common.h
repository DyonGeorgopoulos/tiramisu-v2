#ifndef COMMON_H
#define COMMON_H

#include <SDL3/SDL.h>
#include "entity.h"
#include <cglm/vec2.h>

extern bool should_draw;
typedef struct AppState {
    SDL_Window* window;
    float last_tick;
    float current_tick;
    float delta_time;
  } AppState;

void screen_to_world(float x, float y, vec2 dest);
void draw_sprite(float worldX, float worldY, Entity* entity);
#endif