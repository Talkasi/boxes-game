#include "levels.h"
#include <string.h>
#include <stdio.h>

void get_level(int n, struct levelField *level_dst)
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

            int offsetW = (SCREEN_WIDTH - 9 * STEP) / 2;
            int offsetH = (SCREEN_HEIGHT - 9 * STEP) / 2;

            int x = offsetW;
            int y = offsetH;

            for (int i = 0; i < N_FIELDS_HEIGHT; ++i) {
                for (int j = 0; j < N_FIELDS_WIDTH; ++j) {
                    level_dst->field[i][j].x = x;
                    level_dst->field[i][j].y = y;
                    level_dst->field[i][j].type = level_01[i][j];

                    x += STEP;
                    if (x > SCREEN_WIDTH) {
                        x = offsetW;
                        y += STEP;
                    }
                }
            }

            level_dst->hero.x = offsetW + 1 * STEP;
            level_dst->hero.y = offsetH + 2 * STEP;
            level_dst->hero.i = 2;
            level_dst->hero.j = 1;

            level_dst->n_boxes = 3;

            level_dst->dst[0].x = offsetW + 7 * STEP;
            level_dst->dst[0].y = offsetH + 3 * STEP;

            level_dst->dst[1].x = offsetW + 7 * STEP;
            level_dst->dst[1].y = offsetH + 4 * STEP;

            level_dst->dst[2].x = offsetW + 7 * STEP;
            level_dst->dst[2].y = offsetH + 5 * STEP;
        } break;
            //        case 2: {
            //            level_field level_02 = {
            //                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            //                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            //                    {1, 0, 0, 2, 0, 0, 1, 0, 0, 1},
            //                    {1, 0, 0, 1, 2, 1, 1, 0, 1, 1},
            //                    {1, 0, 2, 0, 0, 0, 0, 0, 1, 0},
            //                    {1, 1, 1, 1, 1, 0, 1, 0, 1, 0},
            //                    {0, 0, 1, 0, 2, 0, 0, 0, 1, 0},
            //                    {0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
            //                    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
            //            };
            //
            //            level_dst->hero.x = 6;
            //            level_dst->hero.y = 6;
            //
            //            level_dst->w = 10;
            //            level_dst->h = 9;
            //
            //            level_dst->n_boxes = 4;
            //
            //            level_dst->dst[0].x = 1;
            //            level_dst->dst[0].y = 1;
            //
            //            level_dst->dst[1].x = 1;
            //            level_dst->dst[1].y = 2;
            //
            //            level_dst->dst[2].x = 2;
            //            level_dst->dst[2].y = 1;
            //
            //            level_dst->dst[3].x = 2;
            //            level_dst->dst[3].y = 2;
            //            memcpy(&level_dst->field, &level_02, sizeof(level_field));
            //        }
        default:
            break;
    }
}
