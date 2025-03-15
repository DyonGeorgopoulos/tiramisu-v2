#include "player.h"
#include <cglm/vec2.h>
#define SOKOL_IMGUI_NO_SOKOL_APP
#include "sokol_gfx.h"
#include "sokol_imgui.h"
#include "sokol_gp.h"

Position player_position = {0, 0, 6};

static void cleanup(void) {

}

static void handle_events(float delta_time, SDL_Event* event) {
    switch (event->type)
    {
        default:
            break;
    }

}

static void update(float delta_time) {

  }

  void init_player() {
  
    Entity player = {
      .name = "player",
      .cleanup = cleanup,
      .handle_events = handle_events,
      .update = update
    };
  
    create_entity(player);
}