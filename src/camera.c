#include "camera.h"


Camera camera = {1920/2, 1080/2, 1920, 1080};

static int movement_speed = 100;

static void render(void) {
  igSliderInt("Camera movement speed",&movement_speed,0,1000,NULL,0);
}

static void update(float delta_time) {
    //camera.x =  player_position.x - camera.w / 2;
    //camera.y =  player_position.y - camera.h / 2;
  
    if (camera.x < 0) camera.x = 0;
    if (camera.y < 0) camera.y = 0;

    float mX, mY;
    const _Bool *keyboard_state = SDL_GetKeyboardState(NULL);
    SDL_GetMouseState(&mX, &mY);

    if (keyboard_state[SDL_SCANCODE_W] || mY > 0 && mY < 100) {
      camera.y -= movement_speed * player_position.scale * delta_time;
    }
  
    // when scrolling the screen down, you need to ignore the UI. This will be interesting
    if (keyboard_state[SDL_SCANCODE_S] || mY > 980 && mY < 1080) {
      camera.y += movement_speed * player_position.scale * delta_time;
    }
  
    if (keyboard_state[SDL_SCANCODE_A] || mX > 0 && mX < 100) {
      camera.x -= movement_speed * player_position.scale * delta_time;
    }
  
    if (keyboard_state[SDL_SCANCODE_D] || mX > 1820 && mX < 1920) {
      camera.x += movement_speed  * player_position.scale* delta_time;
    }
  
    // add update code for scale & map_edge here.
    //if (camera.x + camera.w > 450) camera.x = 450 - camera.w;
    //if (camera.y + camera.h > 450) camera.y = 450 - camera.h;
}

void init_camera() {
    Entity camera = {
        .name = "camera",
        .update = update,
        .render = render
    };

    create_entity(camera);
}