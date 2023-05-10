#include "levels.h"
#include <string.h>

struct level get_level(int n)
{
    struct level cur_level;

    switch (n) {
        case 1: {
            level_field level_01 = {
                    {1, 1, 1, 1, 1, 0, 0, 0, 0},
                    {1, 0, 0, 0, 1, 0, 0, 0, 0},
                    {1, 0, 2, 2, 1, 0, 1, 1, 1},
                    {1, 0, 2, 0, 1, 0, 1, 3, 1},
                    {1, 1, 1, 0, 1, 1, 1, 3, 1},
                    {0, 1, 1, 0, 0, 0, 0, 3, 1},
                    {0, 1, 0, 0, 0, 1, 0, 0, 1},
                    {0, 1, 0, 0, 0, 1, 1, 1, 1},
                    {0, 1, 1, 1, 1, 1, 0, 0, 0},
            };

            cur_level.hero.x = 1;
            cur_level.hero.y = 2;

            cur_level.n_boxes = 3;

            cur_level.dst[0].x = 7;
            cur_level.dst[0].y = 3;

            cur_level.dst[1].x = 7;
            cur_level.dst[1].y = 4;

            cur_level.dst[2].x = 7;
            cur_level.dst[2].y = 5;

            cur_level.box[0].x = 2;
            cur_level.box[0].y = 2;

            cur_level.box[1].x = 2;
            cur_level.box[1].y = 3;

            cur_level.box[2].x = 3;
            cur_level.box[2].y = 2;
            memcpy(&cur_level.field, &level_01, N_FIELDS_WIDTH * N_FIELDS_HEIGHT * sizeof(int));
        } break;
        case 2: {
            level_field level_02 = {
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                    {1, 3, 3, 0, 0, 0, 0, 0, 0, 1},
                    {1, 3, 3, 2, 0, 0, 1, 0, 0, 1},
                    {1, 0, 0, 1, 2, 1, 1, 0, 1, 1},
                    {1, 0, 2, 0, 0, 0, 0, 0, 1, 0},
                    {1, 1, 1, 1, 1, 0, 1, 0, 1, 0},
                    {0, 0, 1, 0, 2, 0, 0, 0, 1, 0},
                    {0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
            };

            cur_level.hero.x = 6;
            cur_level.hero.y = 6;

            cur_level.n_boxes = 4;

            cur_level.dst[0].x = 1;
            cur_level.dst[0].y = 1;

            cur_level.dst[1].x = 1;
            cur_level.dst[1].y = 2;

            cur_level.dst[2].x = 2;
            cur_level.dst[2].y = 1;

            cur_level.dst[3].x = 2;
            cur_level.dst[3].y = 2;
            memcpy(&cur_level.field, &level_02, N_FIELDS_WIDTH * N_FIELDS_HEIGHT * sizeof(int));
        }
        default:
            break;
    }
    return cur_level;
}
