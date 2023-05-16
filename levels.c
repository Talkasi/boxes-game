#include "levels.h"
#include <stdio.h>

void fill_field(struct level *level_dst, level_field field, int offsetW, int offsetH);

void get_level(int n, struct level *level_dst)
{
    switch (n) {
        case 1: {
            level_field field_01 = {
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

            fill_field(level_dst, field_01, offsetW, offsetH);

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
        case 2: {
            level_field field_02 = {
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                    {1, 0, 0, 2, 0, 0, 1, 0, 0, 1},
                    {1, 0, 0, 1, 2, 1, 1, 0, 1, 1},
                    {1, 0, 2, 0, 0, 0, 0, 0, 1, 0},
                    {1, 1, 1, 1, 1, 0, 1, 0, 1, 0},
                    {0, 0, 1, 0, 2, 0, 0, 0, 1, 0},
                    {0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0}};

            int offsetW = (SCREEN_WIDTH - 10 * STEP) / 2;
            int offsetH = (SCREEN_HEIGHT - 9 * STEP) / 2;

            fill_field(level_dst, field_02, offsetW, offsetH);

            level_dst->hero.x = 6 * STEP + offsetW;
            level_dst->hero.y = 6 * STEP + offsetH;
            level_dst->hero.i = 6;
            level_dst->hero.j = 6;

            level_dst->n_boxes = 4;

            level_dst->dst[0].x = 1 * STEP + offsetW;
            level_dst->dst[0].y = 1 * STEP + offsetH;

            level_dst->dst[1].x = 1 * STEP + offsetW;
            level_dst->dst[1].y = 2 * STEP + offsetH;

            level_dst->dst[2].x = 2 * STEP + offsetW;
            level_dst->dst[2].y = 1 * STEP + offsetH;

            level_dst->dst[3].x = 2 * STEP + offsetW;
            level_dst->dst[3].y = 2 * STEP + offsetH;
        }
        default:
            break;
    }
}

void fill_field(struct level *level_dst, level_field field, int offsetW, int offsetH)
{
    int x = offsetW;
    int y = offsetH;

    for (int i = 0; i < N_FIELDS_HEIGHT; ++i)
        for (int j = 0; j < N_FIELDS_WIDTH; ++j) {
            level_dst->field[i][j].x = x;
            level_dst->field[i][j].y = y;
            level_dst->field[i][j].type = field[i][j];

            x += STEP;
            if (x >= SCREEN_WIDTH) {
                x = offsetW;
                y += STEP;
            }
        }
}
