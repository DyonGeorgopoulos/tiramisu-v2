#include <stdio.h>

#define SOKOL_IMPL

#ifdef SOKOL_EXPERIMENT_GL
#define SOKOL_GLCORE33
#include <glad/gl.h>
#elif SOKOL_EXPERIMENT_D3D
#define SOKOL_D3D11
#endif

#define SOKOL_EXTERNAL_GL_LOADER
#define SOKOL_NO_DEPRECATED
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_gp.h"

// #include "../imgui/imgui_impl_sdl3.h"

// #define SOKOL_IMGUI_NO_SOKOL_APP
// #define SOKOL_IMGUI_IMPL
// #include <util/sokol_imgui.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "shader.glsl.h"
#include "sokol-sdl-graphics-backend.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

struct State
{
    sg_pass_action pass_action;
    sg_image image;
    sg_pipeline pip;
    sg_shader shd;
    sg_sampler linear_sampler;
    struct Entity *entities;
};

struct State g_state = {0};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) == false)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), NULL);
        return SDL_APP_FAILURE;
    }

    // 800x450 is 16:9
    window = SDL_CreateWindow(
        "TIRAMISU",                              // window title
        1920,                                     // width, in pixels
        1080,                                     // height, in pixels
        SDL_WINDOW_OPENGL // flags - see below
    );
    if (window == NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't create window/renderer!", SDL_GetError(), NULL);
        return SDL_APP_FAILURE;
    }

    // initialise d3d11 or opengl backend
    se_init_backend(window);

    // create sokol_gfx backend
    const sg_desc desc = se_create_desc();
    sg_setup(&desc);

    // Initialize Sokol GP, adjust the size of command buffers for your own use.
    sgp_desc sgpdesc = {0};
    sgp_setup(&sgpdesc);
    if (!sgp_is_valid())
    {
        fprintf(stderr, "Failed to create Sokol GP context: %s\n", sgp_get_error_message(sgp_get_last_error()));
        exit(-1);
    }

    // initialize shader
    g_state.shd = sg_make_shader(sgp_program_shader_desc(sg_query_backend()));
    if (sg_query_shader_state(g_state.shd) != SG_RESOURCESTATE_VALID)
    {
        fprintf(stderr, "failed to make custom pipeline shader\n");
        exit(-1);
    }

    sgp_pipeline_desc pip_desc = {0};
    pip_desc.shader = g_state.shd;
    pip_desc.has_vs_color = true;
    pip_desc.blend_mode = SGP_BLENDMODE_BLEND;
    g_state.pip = sgp_make_pipeline(&pip_desc);
    if (sg_query_pipeline_state(g_state.pip) != SG_RESOURCESTATE_VALID)
    {
        fprintf(stderr, "failed to make custom pipeline\n");
        exit(-1);
    }

    g_state.pass_action = (sg_pass_action){
        .colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {1.0f, 1.0f, 1.0f, 1.0f}}};

    // return success!
    return SDL_APP_CONTINUE;
}

// This function runs when a new event occurs
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        // end the program, reporting success to the OS
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
        if (event->key.key == SDLK_ESCAPE)
        {
            // end the program on ESC key,
            // returning success to the OS
            return SDL_APP_SUCCESS;
        }
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
    // this needs to be done to handle resizes. I think context needs to be nuked and swapchains created again
        break;
    default:
        break;
    }

    // return continue to continue
    return SDL_APP_CONTINUE;
}

// This function runs once per frame, and is the heart of the program
SDL_AppResult SDL_AppIterate(void *appstate)
{
    int width, height;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    sgp_begin(width, height);
    sgp_viewport(0, 0, width, height);
    sgp_set_blend_mode(SGP_BLENDMODE_BLEND);
    sgp_clear();

    // draw background
    sgp_set_color(0.0f, 0.0f, 0.0f, 1.0f);
    sgp_clear();
    sgp_reset_color();

    sgp_set_color(1, 1, 0.3f, 1.0f);
    sgp_draw_filled_rect(width/2, height/2, 50.0f, 50.0f);

    sg_begin_pass(&(sg_pass){.action = g_state.pass_action, .swapchain = d3d11_swapchain()});
    
    // Dispatch all draw commands to Sokol GFX.
    sgp_flush();
    // Finish a draw command queue, clearing it.
    sgp_end();
    // End render pass.
    sg_end_pass();
    // Commit Sokol render.
    sg_commit();
    se_present(window);

    // return continue to continue
    return SDL_APP_CONTINUE;
}

// This function runs once at shutdown
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    se_deinit_backend();
    // SDL will clean up the window/renderer for us.
}