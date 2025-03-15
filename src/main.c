#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
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

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
//#include "cimgui.h"
//#include "cimgui_impl.h"
#define SOKOL_IMGUI_NO_SOKOL_APP
#define SOKOL_IMGUI_IMPL
#include "cimgui.h"
#include "cimgui_impl.h"
#include "sokol_imgui.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "shader.glsl.h"
#include "entity.h"
#include "sokol-sdl-graphics-backend.h"
#include "global.h"
#include "common.h"


/* TODO:
- LOAD_TEXTURE FUNC
- SOME STRUCT TO INITIALISE ENTITIES
- SOME FUNCTION TO CREATE AN ENTITY AND PASS IT TO GLOBAL ENTITY ARRAY
- ENTITY ARRAY SHOULD HAVE SUB ARRAYS. FOR EXAMPLE THE MAP, SHOULD BE IN THE OVERARCHING ENTITY ARRAY + HAVE ITS OWN ENTITY SLOT FOR EACH ELEMENT IN THE TILESET OR SOME OTHER STRUCT


*/
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    AppState* state = SDL_malloc(sizeof(AppState));
    *appstate = state;
    
    if (SDL_Init(SDL_INIT_VIDEO) == false)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), NULL);
        return SDL_APP_FAILURE;
    }

    // 800x450 is 16:9
    state->window = SDL_CreateWindow(
        "TIRAMISU",       // window title
        1920,             // width, in pixels
        1080,             // height, in pixels
        SDL_WINDOW_OPENGL // flags - see below
    );
    if (state->window == NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't create window/renderer!", SDL_GetError(), NULL);
        return SDL_APP_FAILURE;
    }


    // initialise d3d11 or opengl backend
    se_init_backend(state->window);

    // create sokol_gfx backend
    const sg_desc desc = se_create_desc();
    sg_setup(&desc);

    // Initialize Sokol GP.
    sgp_desc sgpdesc = {0};
    sgp_setup(&sgpdesc);
    if (!sgp_is_valid())
    {
        fprintf(stderr, "Failed to create Sokol GP context: %s\n", sgp_get_error_message(sgp_get_last_error()));
        exit(-1);
    }

    // INITIALISE SIMGUI -> PROBABLY SHOULD ALTER THIS DESC
    simgui_setup(&(simgui_desc_t){.ini_filename = "imgui.ini"});
    igCreateContext(NULL);
    se_init_imgui(state->window);

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

    // initialise the map
    init_map("../../res/map.json");
    init_player();
    init_camera();

    // return success!
    return SDL_APP_CONTINUE;
}

// This function runs when a new event occurs
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppState* state = (AppState*) appstate;

    for (int i = 0; i < entities_count; i++) {
        if (!entities[i].handle_events) continue;
        entities[i].handle_events(state->delta_time, event);
    }

    ImGui_ImplSDL3_ProcessEvent(event);
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
    case SDL_EVENT_MOUSE_WHEEL:
        mouse_wheel_event(event->wheel);
        break;
    default:
        break;
    }

    // return continue to continue
    return SDL_APP_CONTINUE;
}

// This function runs once at shutdown
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    se_deinit_backend();
    // SDL will clean up the window/renderer for us.
}