#ifndef BOXES_GAME_LEVELS_H
#define BOXES_GAME_LEVELS_H
#include <inttypes.h>
#define SCREEN_WIDTH 510
#define SCREEN_HEIGHT 510
#define STEP 51

#define N_FIELDS_WIDTH 10
#define N_FIELDS_HEIGHT 10

#define N_LEVELS 2

enum field {
    EMPTY,
    WALL,
    N_BOX,
    D_BOX,
    DST,
    N_FIELD_TYPES
};

typedef uint8_t level_field[N_FIELDS_HEIGHT][N_FIELDS_WIDTH];

struct point {
    int i;
    int j;
};

struct level {
    level_field field;
    struct point hero;
    int n_boxes;
    int OffsetW;
    int OffsetH;
};

void get_level(int n, struct level *level_dst);

#endif
