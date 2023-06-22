#include "levels.h"
#include <string.h>

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
        }
        default:
            break;
    }
}
