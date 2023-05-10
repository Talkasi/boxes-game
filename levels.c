#include "levels.h"
#include <string.h>

void get_level(int n, struct level *level_dst)
{
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

            level_dst->hero.x = 1;
            level_dst->hero.y = 2;

            level_dst->w = 9;
            level_dst->h = 9;

            level_dst->n_boxes = 3;

            level_dst->dst[0].x = 7;
            level_dst->dst[0].y = 3;

            level_dst->dst[1].x = 7;
            level_dst->dst[1].y = 4;

            level_dst->dst[2].x = 7;
            level_dst->dst[2].y = 5;
            memcpy(&level_dst->field, &level_01, sizeof (level_field));
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

            level_dst->hero.x = 6;
            level_dst->hero.y = 6;

            level_dst->w = 10;
            level_dst->h = 9;

            level_dst->n_boxes = 4;

            level_dst->dst[0].x = 1;
            level_dst->dst[0].y = 1;

            level_dst->dst[1].x = 1;
            level_dst->dst[1].y = 2;

            level_dst->dst[2].x = 2;
            level_dst->dst[2].y = 1;

            level_dst->dst[3].x = 2;
            level_dst->dst[3].y = 2;
            memcpy(&level_dst->field, &level_02, sizeof(level_field));
        }
        default:
            break;
    }
}
