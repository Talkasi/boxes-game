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
                    {1, 0, 2, 0, 1, 0, 1, 0, 1},
                    {1, 1, 1, 0, 1, 1, 1, 0, 1},
                    {0, 1, 1, 0, 0, 0, 0, 0, 1},
                    {0, 1, 0, 0, 0, 1, 0, 0, 1},
                    {0, 1, 0, 0, 0, 1, 1, 1, 1},
                    {0, 1, 1, 1, 1, 1, 0, 0, 0},
            };

            cur_level.hero.x = 1;
            cur_level.hero.y = 2;

            cur_level.w = 9;
            cur_level.h = 9;

            cur_level.n_boxes = 3;

            cur_level.dst[0].x = 7;
            cur_level.dst[0].y = 3;

            cur_level.dst[1].x = 7;
            cur_level.dst[1].y = 4;

            cur_level.dst[2].x = 7;
            cur_level.dst[2].y = 5;
            memcpy(&cur_level.field, &level_01, sizeof (level_field));
        } break;
        case 2: {
            level_field level_02 = {
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 2, 0, 0, 1, 0, 0, 1},
                    {1, 0, 0, 1, 2, 1, 1, 0, 1, 1},
                    {1, 0, 2, 0, 0, 0, 0, 0, 1, 0},
                    {1, 1, 1, 1, 1, 0, 1, 0, 1, 0},
                    {0, 0, 1, 0, 2, 0, 0, 0, 1, 0},
                    {0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
            };

            cur_level.hero.x = 6;
            cur_level.hero.y = 6;

            cur_level.w = 10;
            cur_level.h = 9;

            cur_level.n_boxes = 4;

            cur_level.dst[0].x = 1;
            cur_level.dst[0].y = 1;

            cur_level.dst[1].x = 1;
            cur_level.dst[1].y = 2;

            cur_level.dst[2].x = 2;
            cur_level.dst[2].y = 1;

            cur_level.dst[3].x = 2;
            cur_level.dst[3].y = 2;
            memcpy(&cur_level.field, &level_02, sizeof(level_field));
        }
        default:
            break;
    }
    return cur_level;
}
