#include "player.h"
#include <cglm/vec2.h>
#define SOKOL_IMGUI_NO_SOKOL_APP
#include "sokol_gfx.h"
#include "sokol_imgui.h"
#include "sokol_gp.h"

Position player_position = {0, 0, 6};
bool should_scale = false;

static void cleanup() {

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

void mouse_wheel_event(SDL_MouseWheelEvent event) {
  player_position.scale += event.y * 0.5;

  if (player_position.scale < 1) {
    player_position.scale = 1;
  }
  // we should only sgp_scale on the mousehweel event tbh
  should_scale = true;
}