
#include "map.h"


static cute_tiled_map_t* map;
static cute_tiled_layer_t* layer;
static cute_tiled_tileset_t* tileset;
static Texture* texture;

void render_map() {
    cute_tiled_layer_t* temp_layer = layer;

  while (temp_layer) {
    if (!temp_layer->data) {
      temp_layer = temp_layer->next;
      continue;
    }

    for (int i = 0; i < map->height; i++) {
      for (int j = 0; j < map->width; j++) {
        int tile_id = temp_layer->data[i * map->width + j];
        if (tile_id == 0) continue;

        Texture* temp_texture = texture;
        Texture* texture_to_use = NULL;

        while (temp_texture) {
          if (tile_id >= temp_texture->firstgid &&
          tile_id <= temp_texture->firstgid + temp_texture->tilecount - 1) {
            texture_to_use = temp_texture;
            break;
          }
          temp_texture = temp_texture->next;
        }

        if (!texture_to_use) {
          continue;
        }

        int tileset_columns = texture_to_use->tileset_width / map->tilewidth;

        sg_image_desc image_desc = sg_query_image_desc(texture_to_use->texture);
        sgp_set_pipeline(g_state.pip);
        sgp_set_image(IMG_sgp_iTexChannel0, texture_to_use->texture);
        sgp_set_sampler(SMP_sgp_iSmpChannel0, g_state.linear_sampler);

        sgp_rect src = {
          (tile_id - texture_to_use->firstgid) % tileset_columns * map->tilewidth,
          (tile_id - texture_to_use->firstgid) / tileset_columns * map->tileheight,
          map->tilewidth,
          map->tileheight
        };

        sgp_rect dst = {
          j * map->tilewidth,
          i * map->tileheight,
          map->tilewidth,
          map->tileheight
        };
        sgp_draw_textured_rect(1, dst, src);
        sgp_reset_image(IMG_sgp_iTexChannel0);

        sgp_reset_pipeline();
        sgp_reset_color();

      }
    }
  
    temp_layer = temp_layer->next;
  }
}

void init_map(const char* map_path) {
    map = cute_tiled_load_map_from_file(map_path, NULL);

    if (!map) {
        printf("we got problems loading the map");
    }

  
    layer = map->layers;
    tileset = map->tilesets;

    texture = malloc(sizeof(Texture));
    Texture* current_texture = texture;

     // create textgure linear sampler
          // create linear sampler
    sg_sampler_desc linear_sampler_desc = {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST
    };
    
    g_state.linear_sampler = sg_make_sampler(&linear_sampler_desc);
    if (sg_query_sampler_state(g_state.linear_sampler) != SG_RESOURCESTATE_VALID) {
        fprintf(stderr, "failed to create linear sampler");
        exit(-1);
    }
    // for each tileset used load them into the texture array
    while (tileset) {
        current_texture->texture = load_image(tileset->image.ptr);

        if (sg_query_image_state(current_texture->texture) != SG_RESOURCESTATE_VALID) {
            fprintf(stderr, "failed to load map texture for tileset");
            exit(1);
        } 

        current_texture->firstgid = tileset->firstgid;
        current_texture->tilecount = tileset->tilecount;
        current_texture->tileset_width = tileset->imagewidth;
        current_texture->tileset_height = tileset->imageheight;

        tileset = tileset->next;
        if (tileset) {
          current_texture->next = malloc(sizeof(Texture));
          current_texture = current_texture->next;
        } else {
          current_texture->next = NULL;
        }
    }
    // need to turn each tile into an entity, or a map entity like in tutorial.
}