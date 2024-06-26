#ifndef BOXES_GAME_LEVELS_H
#define BOXES_GAME_LEVELS_H
#include "config.h"
#include <inttypes.h>

enum field
{
    EMPTY,
    WALL,
    N_BOX,
    D_BOX,
    DST,
    N_FIELD_TYPES
};

typedef uint8_t lvl_field[NTILES_HEIGHT][NTILES_WIDTH];

struct point
{
    int i;
    int j;
};

struct lvl
{
    lvl_field field;
    struct point hero;
    int n_boxes;
    int OffsetW;
    int OffsetH;
};

void getLvl(int n, struct lvl *lvl_dst);
#endif
