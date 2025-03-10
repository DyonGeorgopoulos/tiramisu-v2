#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>
#include "entity.h"
#include "camera.h"


typedef struct {
  float x, y;
  float scale;
} Position;

extern Position player_position;
extern bool should_scale;

void init_player();
void mouse_wheel_event(SDL_MouseWheelEvent event);

#endif