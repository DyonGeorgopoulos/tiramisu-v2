#include "player.h"
#include <cglm/vec2.h>
#define SOKOL_IMGUI_NO_SOKOL_APP
#include "sokol_gfx.h"
#include "sokol_imgui.h"
#include "sokol_gp.h"

Position player_position = {0, 0, 6};
bool should_scale = false;
static int movement_speed = 100;

static void render() {
  igSliderInt("Camera movement speed",&movement_speed,0,1000,NULL,0);
}
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
    float mX, mY;
    const _Bool *keyboard_state = SDL_GetKeyboardState(NULL);
    SDL_GetMouseState(&mX, &mY);

    if (keyboard_state[SDL_SCANCODE_W] || mY > 0 && mY < 100) {
      camera.y -= movement_speed * delta_time;
    }
  
    // when scrolling the screen down, you need to ignore the UI. This will be interesting
    if (keyboard_state[SDL_SCANCODE_S] || mY > 980 && mY < 1080) {
      camera.y += movement_speed  * delta_time;
    }
  
    if (keyboard_state[SDL_SCANCODE_A] || mX > 0 && mX < 100) {
      camera.x -= movement_speed * delta_time;
    }
  
    if (keyboard_state[SDL_SCANCODE_D] || mX > 1820 && mX < 1920) {
      camera.x += movement_speed * delta_time;
    }
  }

  void init_player() {
  
    Entity player = {
      .name = "player",
      .cleanup = cleanup,
      .handle_events = handle_events,
      .update = update,
      .render = render
    };
  
    create_entity(player);
}

void mouse_wheel_event(SDL_MouseWheelEvent event) {
  player_position.scale += event.y * 0.2;

  if (player_position.scale < 1) {
    player_position.scale = 1;
  }
  // we should only sgp_scale on the mousehweel event tbh
  should_scale = true;
}