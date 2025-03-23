#include "render.h"
#include "global.h"
#include "player.h"
#include "camera.h"
#include "sokol_gp.h"
#define SOKOL_IMGUI_NO_SOKOL_APP
#include "sokol-sdl-graphics-backend.h"
#include "sokol_imgui.h"

vec2 dst = {0};

void app_render(void *appstate)
{
    AppState *state = (AppState *)appstate;

    int width, height;
    SDL_GetWindowSizeInPixels(state->window, &width, &height);
    sgp_begin(width, height);
    
    // IMGUI CODE
    ImGui_ImplSDL3_NewFrame();
    simgui_new_frame(&(simgui_frame_desc_t){
        .width = width,
        .height = height,
        .delta_time = 1,
        .dpi_scale = 1.0f});

    // VIEWPORT CODE
    sgp_viewport(0, 0, width, height);

    sgp_set_blend_mode(SGP_BLENDMODE_BLEND);
    sgp_clear();

    // draw background
    sgp_set_color(0.0f, 0.0f, 0.0f, 1.0f);
    sgp_clear();
    sgp_reset_color();
    sgp_set_pipeline(g_state.pip);

    // TRANSLATE WORLD
    
    // to accurately scale the world at the mouse position, we need screen to world. 
    // if i get mouseX & mouseY, i should need to maybe offset them by (camera.x - camera.w / 2)
    sgp_translate(-(camera.x - camera.w / 2), -(camera.y - camera.h / 2));
    sgp_scale(camera.z, camera.z);
    //sgp_scale(player_position.scale, player_position.scale);
    igBegin("Debug Menu", NULL, 0);

    // EVERY ENTITIES RENDER
    for (int i = 0; i < entities_count; i++)
    {
        if (!entities[i].render)
            continue;
        entities[i].render();
    }

    igText("Zoom levelL: %f", camera.z);

    // could add an entitites IMGUI section.
    // so each entity is in charge of writing it's own stuff, or have a global things can pull debug info out of. Like it all writes to the one, IG_BEGIN and IG_END. 
    igEnd();

    sg_begin_pass(&(sg_pass){.action = g_state.pass_action, .swapchain = d3d11_swapchain()});
    
    
    // Dispatch all draw commands to Sokol GFX.
    sgp_flush();
    // Finish a draw command queue, clearing it.
    sgp_end();

    simgui_render();
    // End render pass.
    sg_end_pass();
    // Commit Sokol render.
    sg_commit();
    

    se_present(state->window);
}