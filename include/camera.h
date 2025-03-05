#ifndef CAMERA_H
#define CAMERA_H

#include "entity.h"
#include "player.h"
#include "cglm/vec2.h"

typedef struct
{
  float x, y, w, h;
} Camera;

extern Camera camera;
void init_camera();

#endif