
#include "map.h"
#include "camera.h"
#include <float.h>
#include <math.h>

static cute_tiled_map_t *map;
static cute_tiled_layer_t *layer;
static cute_tiled_tileset_t *tileset;
static Texture *texture;
static Game_Map *game_map;

static void cleanup(void)
{
    if (texture)
    {
        sg_image simg = {0};
        Texture *current_texture = texture;

        while (current_texture)
        {
            Texture *next_texture = current_texture->next;

            if (current_texture->texture.id != 0)
            {
                sg_destroy_image(current_texture->texture);
            }

            SDL_free(current_texture);
            current_texture = next_texture;
        }

        texture = NULL;
    }

    if (map)
    {
        cute_tiled_free_map(map);
        map = NULL;
    }

    if (game_map)
    {
        if (game_map->tiles)
        {
            SDL_free(game_map->tiles);
        }
        SDL_free(game_map);
        game_map = NULL;
    }
}

static void render(void)
{
    sgp_set_sampler(SMP_sgp_iSmpChannel0, g_state.linear_sampler);

    cute_tiled_layer_t *temp_layer = layer;
    Texture* tmp_texture = NULL;
    for (int i = 0; i < (game_map->map_width * game_map->map_height); i++)
    {
        Tile temp_tile = game_map->tiles[i];
        if (temp_tile.texture_to_use == NULL) {
            continue;
        }
        // set the texture if the texture is not the one we need to set.

        sgp_set_image(IMG_sgp_iTexChannel0, temp_tile.texture_to_use->texture);

        sgp_rect src = {
            temp_tile.texture_x, 
            temp_tile.texture_y,
            game_map->tile_width,
            game_map->tile_height};

        sgp_rect dst = {
            temp_tile.tile_world_y,
            temp_tile.tile_world_x,
            game_map->tile_width,
            game_map->tile_height};
        sgp_draw_textured_rect(1, dst, src);
        sgp_reset_image(IMG_sgp_iTexChannel0);

        // loop through each layer for each tile and if theres a texture to use
        // we have to render something
        for (int j = 0; j < game_map->layers; j++) {
            if (temp_tile.layers[j].texture_to_use) {
                sgp_set_image(IMG_sgp_iTexChannel0, temp_tile.layers[j].texture_to_use->texture);
            }
                sgp_rect src = {
                    temp_tile.layers[j].texture_x, 
                    temp_tile.layers[j].texture_y,
                    game_map->tile_width,
                    game_map->tile_height};
        
                sgp_rect dst = {
                    temp_tile.tile_world_y,
                    temp_tile.tile_world_x,
                    game_map->tile_width,
                    game_map->tile_height};
                sgp_draw_textured_rect(1, dst, src);
                sgp_reset_image(IMG_sgp_iTexChannel0);
        }
    }

    sgp_reset_sampler(SMP_sgp_iSmpChannel0);
}

void init_map(const char *map_path)
{
    map = cute_tiled_load_map_from_file(map_path, NULL);

    if (!map)
    {
        printf("we got problems loading the map");
    }

    layer = map->layers;
    tileset = map->tilesets;

    texture = malloc(sizeof(Texture));
    Texture *current_texture = texture;

    // create textgure linear sampler
    // create linear sampler
    sg_sampler_desc linear_sampler_desc = {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .mipmap_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_MIRRORED_REPEAT,
        .wrap_v = SG_WRAP_MIRRORED_REPEAT};

    g_state.linear_sampler = sg_make_sampler(&linear_sampler_desc);
    if (sg_query_sampler_state(g_state.linear_sampler) != SG_RESOURCESTATE_VALID)
    {
        fprintf(stderr, "failed to create linear sampler");
        exit(-1);
    }
    // for each tileset used load them into the texture array
    while (tileset)
    {
        current_texture->texture = load_image(tileset->image.ptr);

        if (sg_query_image_state(current_texture->texture) != SG_RESOURCESTATE_VALID)
        {
            fprintf(stderr, "FATAL: failed to load map texture for tileset");
            exit(1);
        }

        current_texture->firstgid = tileset->firstgid;
        current_texture->tilecount = tileset->tilecount;
        current_texture->tileset_width = tileset->imagewidth;
        current_texture->tileset_height = tileset->imageheight;

        tileset = tileset->next;
        if (tileset)
        {
            current_texture->next = malloc(sizeof(Texture));
            current_texture = current_texture->next;
        }
        else
        {
            current_texture->next = NULL;
        }
    }

    init_map_array(map);

    Entity map_e = {
        .name = "map",
        .render = render,
        .cleanup = cleanup};

    create_entity(map_e);
}

void init_map_array(cute_tiled_map_t* map)
{

    // Create map & tiles
    game_map = malloc(sizeof(Game_Map));
    game_map->map_height = map->height;
    game_map->map_width = map->width;
    game_map->tile_height = map->tileheight;
    game_map->tile_width = map->tilewidth;

    // allocate tiles in contigous memory. So for a 3x3 map. -> [0] = [0,0], [1] = [0, 1], [2] = [0,2]
    Tile *tiles = (Tile *)calloc((game_map->map_width * game_map->map_height), sizeof(Tile));
    
    // get the layer count and then alloc each dynamic layer
    int layer_count = 0;
    cute_tiled_layer_t *temp_layer = layer;

    while (temp_layer)
    {
        layer_count++;
        if (!temp_layer->data)
        {
            temp_layer = temp_layer->next;
            continue;
        }
        temp_layer = temp_layer->next;
    }

    for (int i =0; i < map->height * map->width; i++)
    {
        tiles[i].layers = (Layer*)calloc(layer_count, sizeof(Layer));
    }
    
    // 100% could do some optimisation here. 
    game_map->layers = layer_count;
    temp_layer = layer;
    int tmp_layer_count = 0;
    while (temp_layer)
    {
        if (!temp_layer->data)
        {
            //tmp_layer_count++;
            temp_layer = temp_layer->next;
            continue;
        }

        for (int i = 0; i < map->height; i++)
        {
            for (int j = 0; j < map->width; j++)
            {
                int tile_id = temp_layer->data[i * map->width + j];

                // if theres nothing in that spot, skip!
                if (tile_id == 0)
                    continue;

                Texture *temp_texture = texture;
                Texture *texture_to_use = NULL;

                // this code figures out which texture to use. We need to store a pointer to this texture for each tile. I do wonder if we can batch it for layer.
                // for now this brute forces and thats fine, but we should cache it to ensure we dont need to look it up and set_image constantly.
                while (temp_texture)
                {
                    if (tile_id >= temp_texture->firstgid &&
                        tile_id <= temp_texture->firstgid + temp_texture->tilecount - 1)
                    {
                        texture_to_use = temp_texture;
                        break;
                    }
                    temp_texture = temp_texture->next;
                }

                if (!texture_to_use)
                {
                    continue;
                }

                int tileset_columns = texture_to_use->tileset_width / map->tilewidth;
                int map_w = map->width - 1;
                if (i == 0) {
                    map_w = map->width;
                }
                int index = (i * map_w) + (i + j);

                // allocate the base tile
                if (tmp_layer_count == 0) {
                    tiles[index].texture_to_use = texture_to_use;
                    tiles[index].texture_x = (tile_id - texture_to_use->firstgid) % tileset_columns * map->tilewidth;
                    tiles[index].texture_y = (tile_id - texture_to_use->firstgid) / tileset_columns * map->tileheight;
                    tiles[index].tile_world_x = i * game_map->tile_width;
                    tiles[index].tile_world_y = j * game_map->tile_height;
                }
                else 
                {
                    // allocate the layer one by one.
                    tiles[index].layers[tmp_layer_count - 1].texture_to_use = texture_to_use;
                    tiles[index].layers[tmp_layer_count - 1].texture_x = (tile_id - texture_to_use->firstgid) % tileset_columns * map->tilewidth;
                    tiles[index].layers[tmp_layer_count - 1].texture_y = (tile_id - texture_to_use->firstgid) / tileset_columns * map->tileheight;
                }
 
            }
        }
        tmp_layer_count++;
        temp_layer = temp_layer->next;
    }
    game_map->tiles = tiles;
}