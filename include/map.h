#ifndef MAP_H
#define MAP_H

#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"
#include "sokol_gfx.h"
#include "sokol_gp.h"
#include "global.h"
#include "entity.h"
#include "shader.glsl.h"
#include <SDL3/SDL.h>



typedef struct Texture {
  //SDL_Texture* texture;
  sg_image texture;
  int firstgid;
  int tilecount;
  int tileset_width;
  int tileset_height;
  struct Texture* next;
} Texture;

static void render();
void init_map(const char* map_path);

#endif