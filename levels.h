#ifndef BOXES_GAME_LEVELS_H
#define BOXES_GAME_LEVELS_H
#include <inttypes.h>

#define N_FIELDS_WIDTH 10
#define N_FIELDS_HEIGHT 10

#define N_LEVELS 5

enum field {
    EMPTY,
    WALL,
    N_BOX,
    D_BOX,
    DST,
    N_FIELD_TYPES
};

typedef uint8_t lvl_field[N_FIELDS_HEIGHT][N_FIELDS_WIDTH];
typedef uint8_t lvl_info_arr[N_LEVELS];

struct point {
    int i;
    int j;
};

struct lvl {
    lvl_field field;
    struct point hero;
    int n_boxes;
    int OffsetW;
    int OffsetH;
};

struct lvl_info {
    int cur_n;
    lvl_info_arr info;
};

void getLvl(int n, struct lvl *lмl_dst);

#endif
