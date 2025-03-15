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

typedef struct Layer {
  Texture* texture_to_use;
  float texture_x;
  float texture_y;
} Layer;

typedef struct Tile {
  char* tile_name; // name of the tile
  float texture_x;
  float texture_y;
  // texture info for the ground tile itself.
  float tile_world_x, tile_world_y; // tile x & y position in world space
  Texture* texture_to_use; // texture to use when rendering this tile
  int texture_id; // the id of the texture at the tile space
  Entity entity; // the entity placed on the tile
  Layer* layers;
} Tile;

typedef struct Game_Map {
  char* map_name;
  int map_width;
  int map_height;
  int tile_width;
  int tile_height;
  int layers;
  Tile* tiles;
  // what does a map have?
} Game_Map;

static void render();
void init_map(const char* map_path);
void init_map_array(cute_tiled_map_t* map);

#endif