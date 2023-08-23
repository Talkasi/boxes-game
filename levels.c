#include "levels.h"
#include <string.h>

void getLvl(int n, struct lvl *lvl_dst)
{
    switch (n) {
        case 1: {
            lvl_field field_01 = {
                    {1, 1, 1, 1, 1, 0, 0, 0, 0},
                    {1, 0, 0, 0, 1, 0, 0, 0, 0},
                    {1, 0, 2, 2, 1, 0, 1, 1, 1},
                    {1, 0, 2, 0, 1, 0, 1, 4, 1},
                    {1, 1, 1, 0, 1, 1, 1, 4, 1},
                    {0, 1, 1, 0, 0, 0, 0, 4, 1},
                    {0, 1, 0, 0, 0, 1, 0, 0, 1},
                    {0, 1, 0, 0, 0, 1, 1, 1, 1},
                    {0, 1, 1, 1, 1, 1, 0, 0, 0},
            };

            memcpy(lvl_dst->field, field_01, sizeof(lvl_field));

            lvl_dst->hero.i = 2;
            lvl_dst->hero.j = 1;

            lvl_dst->OffsetW = (SCREEN_WIDTH - 9 * TILE_SIZE) / 2;
            lvl_dst->OffsetH = (SCREEN_HEIGHT - 9 * TILE_SIZE) / 2;
            lvl_dst->n_boxes = 3;
        } break;
        case 2: {
            lvl_field field_02 = {
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                    {1, 4, 4, 0, 0, 0, 0, 0, 0, 1},
                    {1, 4, 4, 2, 0, 0, 1, 0, 0, 1},
                    {1, 0, 0, 1, 2, 1, 1, 0, 1, 1},
                    {1, 0, 2, 0, 0, 0, 0, 0, 1, 0},
                    {1, 1, 1, 1, 1, 0, 1, 0, 1, 0},
                    {0, 0, 1, 0, 2, 0, 0, 0, 1, 0},
                    {0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0}};

            memcpy(lvl_dst->field, field_02, sizeof(lvl_field));

            lvl_dst->hero.i = 6;
            lvl_dst->hero.j = 6;

            lvl_dst->OffsetW = (SCREEN_WIDTH - 10 * TILE_SIZE) / 2;
            lvl_dst->OffsetH = (SCREEN_HEIGHT - 9 * TILE_SIZE) / 2;
            lvl_dst->n_boxes = 4;
        } break;
        case 3: {
            lvl_field field_03 = {
                    {0, 1, 1, 1, 1, 0},
                    {1, 1, 0, 0, 1, 0},
                    {1, 0, 2, 0, 1, 0},
                    {1, 1, 2, 0, 1, 1},
                    {1, 1, 0, 2, 0, 1},
                    {1, 4, 2, 0, 0, 1},
                    {1, 4, 4, 3, 4, 1},
                    {1, 1, 1, 1, 1, 1}};

            memcpy(lvl_dst->field, field_03, sizeof(lvl_field));

            lvl_dst->hero.i = 2;
            lvl_dst->hero.j = 1;

            lvl_dst->OffsetW = (SCREEN_WIDTH - 6 * TILE_SIZE) / 2;
            lvl_dst->OffsetH = (SCREEN_HEIGHT - 8 * TILE_SIZE) / 2;
            lvl_dst->n_boxes = 5;
        } break;
        case 4: {
            lvl_field field_04 = {
                    {0, 1, 1, 1, 1, 1, 0, 0},
                    {0, 1, 0, 0, 1, 1, 1, 0},
                    {0, 1, 0, 2, 0, 0, 1, 0},
                    {1, 1, 1, 0, 1, 0, 1, 1},
                    {1, 4, 1, 0, 1, 0, 0, 1},
                    {1, 4, 2, 0, 0, 1, 0, 1},
                    {1, 4, 0, 0, 0, 2, 0, 1},
                    {1, 1, 1, 1, 1, 1, 1, 1}};

            memcpy(lvl_dst->field, field_04, sizeof(lvl_field));

            lvl_dst->hero.i = 1;
            lvl_dst->hero.j = 2;

            lvl_dst->OffsetW = (SCREEN_WIDTH - 8 * STEP) / 2;
            lvl_dst->OffsetH = (SCREEN_HEIGHT - 8 * STEP) / 2;
            lvl_dst->n_boxes = 3;
        } break;
        case 5: {
            lvl_field field_05 = {
                    {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
                    {0, 1, 0, 0, 0, 0, 0, 1, 1, 1},
                    {1, 1, 2, 1, 1, 1, 0, 0, 0, 1},
                    {1, 0, 0, 0, 2, 0, 0, 2, 0, 1},
                    {1, 0, 4, 4, 1, 0, 2, 0, 1, 1},
                    {1, 1, 4, 4, 1, 0, 0, 0, 1, 0},
                    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0}};

            memcpy(lvl_dst->field, field_05, sizeof(lvl_field));

            lvl_dst->hero.i = 3;
            lvl_dst->hero.j = 2;

            lvl_dst->OffsetW = (SCREEN_WIDTH - 10 * TILE_SIZE) / 2;
            lvl_dst->OffsetH = (SCREEN_HEIGHT - 7 * TILE_SIZE) / 2;
            lvl_dst->n_boxes = 4;
        } break;
        case 6: {
            lvl_field field_05 = {
                    {0, 0, 1, 1, 1, 1, 1, 1},
                    {0, 0, 1, 0, 0, 0, 0, 1},
                    {1, 1, 1, 2, 2, 2, 0, 1},
                    {1, 0, 0, 2, 4, 4, 0, 1},
                    {1, 0, 2, 4, 4, 4, 1, 1},
                    {1, 1, 1, 1, 0, 0, 1, 0},
                    {0, 0, 0, 1, 1, 1, 1, 0}};

            memcpy(lvl_dst->field, field_05, sizeof(lvl_field));

            lvl_dst->hero.i = 3;
            lvl_dst->hero.j = 2;

            lvl_dst->OffsetW = (SCREEN_WIDTH - 8 * TILE_SIZE) / 2;
            lvl_dst->OffsetH = (SCREEN_HEIGHT - 7 * TILE_SIZE) / 2;
            lvl_dst->n_boxes = 5;
        } break;
        default:
            break;
    }
}
