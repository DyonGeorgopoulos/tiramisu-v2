#ifndef GLOBAL_H
#define GLOBAL_H

#include "stb_image.h"
#include "sokol_gfx.h"
#include "sokol_gp.h"
#define TILE_SIZE 32


struct Entity {
    int x; // world pos x
    int y; // world pos y
    int width; // tile size
    int height; // tile size
    sg_image* image;
    int img_x; // texture location
    int img_y; // texture location
};

struct State {
    sg_pass_action pass_action;
    sg_image image;
    sg_pipeline pip;
    sg_shader shd;
    sg_sampler linear_sampler;
    struct Entity* entities;
};

sg_image load_image(const char *filename);

#endif

extern struct State g_state;