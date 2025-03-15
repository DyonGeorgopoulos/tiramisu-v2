#ifndef CAMERA_H
#define CAMERA_H

#include <SDL3/SDL.h>
#include "entity.h"
#include "player.h"
#include "cglm/vec2.h"

typedef struct
{
  float x, y, z, w, h;
} Camera;

extern Camera camera;
void init_camera();
void mouse_wheel_event(SDL_MouseWheelEvent event);

#endif