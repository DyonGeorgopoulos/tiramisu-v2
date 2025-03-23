#ifndef ENTITY_H
#define ENTITY_H

#include <SDL3/SDL.h>
#include "sokol_gfx.h"
#include "sokol_gp.h"

#define MAX_ENTITIES 100
#define MAX_NAME_LENGTH 64

typedef struct {
    char name[MAX_NAME_LENGTH];
    void (*cleanup)(void);
    void (*handle_events)(float, SDL_Event*);
    void (*update)(float);
    void (*render)(void);
  } Entity;
  

  typedef struct Game_Entity_Texture {
    sg_image texture; // the loaded texture file
    int tile_size; // the size of each individual tile
    int object_size[2]; // the number of tiles the object consumes on the image x * y
    int texture_len; // the size in pixels the texture length is
    int texture_1;
  } Game_Entity_Texture;

  typedef struct GameEntity{
    // these live on tiles. Can only be placed on tiles
    char name[MAX_NAME_LENGTH];

  } GameEntity; 

  extern Entity entities[MAX_ENTITIES];
  extern int entities_count;
  
  void create_entity(Entity entity);
  void delete_entity(int index);
  void swap_entities(int index1, int index2);
  int find_entity(const char* name);

#endif