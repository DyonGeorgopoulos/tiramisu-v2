#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>
#include "entity.h"
#include "camera.h"

typedef struct {
  float x, y;
} Position;

extern Position player_position;

void init_player();

#endif