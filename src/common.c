#include "common.h"
#include "camera.h"

void screen_to_world(float x, float y, vec2 dest)
{
    float screen_center_x = camera.w / 2;
    float screen_center_y = camera.h / 2;
    float new_x = ((x - screen_center_x) + camera.x) / camera.z;
    float new_y = ((y - screen_center_y) + camera.y) / camera.z;
    dest[0] = new_x;
    dest[1] = new_y;
}

void draw_sprite(float worldX, float worldY, Entity *entity)
{
    // call sgp_draw_textured_rect with the entity 
}