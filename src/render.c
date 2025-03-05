#include "render.h"
#include "global.h"
#include "sokol_gp.h"
#include "sokol-sdl-graphics-backend.h"

void app_render(void *appstate)
{
    AppState *state = (AppState *)appstate;

    int width, height;
    SDL_GetWindowSizeInPixels(state->window, &width, &height);
    sgp_begin(width, height);
    sgp_viewport(0, 0, width, height);
    sgp_set_blend_mode(SGP_BLENDMODE_BLEND);
    sgp_clear();

    // draw background
    sgp_set_color(0.0f, 0.0f, 0.0f, 1.0f);
    sgp_clear();
    sgp_reset_color();

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