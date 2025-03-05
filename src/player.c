#include "player.h"
#include <cglm/vec2.h>

Position player_position = {450/2, 180/2};
static int movement_speed = 50;

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
      player_position.y -= movement_speed * delta_time;
    }
  
    // when scrolling the screen down, you need to ignore the UI. This will be interesting
    if (keyboard_state[SDL_SCANCODE_S] || mY > 980 && mY < 1080) {
      player_position.y += movement_speed  * delta_time;
    }
  
    if (keyboard_state[SDL_SCANCODE_A] || mX > 0 && mX < 100) {
      player_position.x -= movement_speed * delta_time;
    }
  
    if (keyboard_state[SDL_SCANCODE_D] || mX > 1820 && mX < 1920) {
      player_position.x += movement_speed * delta_time;
    }
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