#ifndef ENTITY_H
#define ENTITY_H

#include <SDL3/SDL.h>

#define MAX_ENTITIES 100
#define MAX_NAME_LENGTH 64

typedef struct {
    char name[MAX_NAME_LENGTH];
    void (*cleanup)(void);
    void (*handle_events)(float, SDL_Event*);
    void (*update)(float);
    void (*render)(void);

    // entity data based on tile
  } Entity;
  
  extern Entity entities[MAX_ENTITIES];
  extern int entities_count;
  
  void create_entity(Entity entity);
  void delete_entity(int index);
  void swap_entities(int index1, int index2);
  int find_entity(const char* name);

#endif