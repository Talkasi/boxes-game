#ifndef BOXES_GAME_LEVELS_H
#define BOXES_GAME_LEVELS_H

#define N_FIELDS_WIDTH 10
#define N_FIELDS_HEIGHT 10

#define N_LEVELS 2
#define N_DST 10

typedef int level_field[N_FIELDS_HEIGHT][N_FIELDS_WIDTH];

struct point {
    int x;
    int y;
};

struct level {
    struct point hero;
    int w;
    int h;
    level_field field;
    int n_boxes;
    struct point dst[N_DST];
};

void get_level(int n, struct level *level_dst);

#endif
