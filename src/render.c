#include "render.h"
#include "global.h"
#include "player.h"
#include "camera.h"
#include "sokol_gp.h"
#include "sokol-sdl-graphics-backend.h"

void app_render(void *appstate)
{
    AppState *state = (AppState *)appstate;

    int width, height;
    SDL_GetWindowSizeInPixels(state->window, &width, &height);
    sgp_begin(width, height);
    sgp_viewport(0, 0, width, height);
    //sgp_project(-width/2, width/2, -height/2, height/2);
    sgp_set_blend_mode(SGP_BLENDMODE_BLEND);
    sgp_clear();

    // draw background
    sgp_set_color(0.0f, 0.0f, 0.0f, 1.0f);
    sgp_clear();
    sgp_reset_color();
    sgp_set_pipeline(g_state.pip);
    //sgp_scale_at(1, 1, (int)player_position.x - camera.w / 2, (int)player_position.y - camera.h / 2);
    sgp_scale(6, 6);
    sgp_translate(-(camera.x - camera.w / 2), -(camera.y - camera.h / 2));
    //sgp_project
    for (int i = 0; i < entities_count; i++)
    {
        if (!entities[i].render)
            continue;
        entities[i].render();
    }

    sg_begin_pass(&(sg_pass){.action = g_state.pass_action, .swapchain = d3d11_swapchain()});
    
    // Dispatch all draw commands to Sokol GFX.
    sgp_flush();
    // Finish a draw command queue, clearing it.
    sgp_end();
    // End render pass.
    sg_end_pass();
    // Commit Sokol render.
    sg_commit();
    se_present(state->window);
}