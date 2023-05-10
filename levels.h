#ifndef BOXES_GAME_LEVELS_H
#define BOXES_GAME_LEVELS_H

#define N_FIELDS_WIDTH 10
#define N_FIELDS_HEIGHT 10

typedef int level_field[N_FIELDS_HEIGHT][N_FIELDS_WIDTH];

struct point {
    int x;
    int y;
};

struct level {
    struct point hero;
    level_field field;
    int n_boxes;
    struct point box[N_FIELDS_HEIGHT * N_FIELDS_WIDTH];
    struct point dst[N_FIELDS_HEIGHT * N_FIELDS_WIDTH];
};

struct level get_level(int n);

#endif
