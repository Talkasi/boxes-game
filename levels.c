#include "levels.h"
#include <string.h>
#include "config.h"

void get_level(int n, struct level *level_dst)
{
    switch (n) {
        case 1: {
            level_field field_01 = {
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

            memcpy(level_dst->field, field_01, sizeof(level_field));

            level_dst->hero.i = 2;
            level_dst->hero.j = 1;

            level_dst->OffsetW = (SCREEN_WIDTH - 9 * STEP) / 2;
            level_dst->OffsetH = (SCREEN_HEIGHT - 9 * STEP) / 2;
            level_dst->n_boxes = 3;
        } break;
        case 2: {
            level_field field_02 = {
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                    {1, 4, 4, 0, 0, 0, 0, 0, 0, 1},
                    {1, 4, 4, 2, 0, 0, 1, 0, 0, 1},
                    {1, 0, 0, 1, 2, 1, 1, 0, 1, 1},
                    {1, 0, 2, 0, 0, 0, 0, 0, 1, 0},
                    {1, 1, 1, 1, 1, 0, 1, 0, 1, 0},
                    {0, 0, 1, 0, 2, 0, 0, 0, 1, 0},
                    {0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0}};

            memcpy(level_dst->field, field_02, sizeof(level_field));

            level_dst->hero.i = 6;
            level_dst->hero.j = 6;

            level_dst->OffsetW = (SCREEN_WIDTH - 10 * STEP) / 2;
            level_dst->OffsetH = (SCREEN_HEIGHT - 9 * STEP) / 2;
            level_dst->n_boxes = 4;
        } break;
        case 3: {
            level_field field_03 = {
                    {0, 1, 1, 1, 1, 0},
                    {1, 1, 0, 0, 1, 0},
                    {1, 0, 2, 0, 1, 0},
                    {1, 1, 2, 0, 1, 1},
                    {1, 1, 0, 2, 0, 1},
                    {1, 4, 2, 0, 0, 1},
                    {1, 4, 4, 3, 4, 1},
                    {1, 1, 1, 1, 1, 1}};

            memcpy(level_dst->field, field_03, sizeof(level_field));

            level_dst->hero.i = 2;
            level_dst->hero.j = 1;

            level_dst->OffsetW = (SCREEN_WIDTH - 6 * STEP) / 2;
            level_dst->OffsetH = (SCREEN_HEIGHT - 8 * STEP) / 2;
            level_dst->n_boxes = 5;
        } break;
        case 4: {
            level_field field_04 = {
                    {0, 1, 1, 1, 1, 1, 0, 0},
                    {0, 1, 0, 0, 1, 1, 1, 0},
                    {0, 1, 0, 2, 0, 0, 1, 0},
                    {1, 1, 1, 0, 1, 0, 1, 1},
                    {1, 4, 1, 0, 1, 0, 0, 1},
                    {1, 4, 2, 0, 0, 1, 0, 1},
                    {1, 4, 0, 0, 0, 2, 0, 1},
                    {1, 1, 1, 1, 1, 1, 1, 1}};

            memcpy(level_dst->field, field_04, sizeof(level_field));

            level_dst->hero.i = 1;
            level_dst->hero.j = 2;

            level_dst->OffsetW = (SCREEN_WIDTH - 8 * STEP) / 2;
            level_dst->OffsetH = (SCREEN_HEIGHT - 8 * STEP) / 2;
            level_dst->n_boxes = 3;
        } break;
        case 5: {
            level_field field_05 = {
                    {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
                    {0, 1, 0, 0, 0, 0, 0, 1, 1, 1},
                    {1, 1, 2, 1, 1, 1, 0, 0, 0, 1},
                    {1, 0, 0, 0, 2, 0, 0, 2, 0, 1},
                    {1, 0, 4, 4, 1, 0, 2, 0, 1, 1},
                    {1, 1, 4, 4, 1, 0, 0, 0, 1, 0},
                    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0}};

            memcpy(level_dst->field, field_05, sizeof(level_field));

            level_dst->hero.i = 3;
            level_dst->hero.j = 2;

            level_dst->OffsetW = (SCREEN_WIDTH - 10 * STEP) / 2;
            level_dst->OffsetH = (SCREEN_HEIGHT - 7 * STEP) / 2;
            level_dst->n_boxes = 4;
        } break;
        default:
            break;
    }
}
