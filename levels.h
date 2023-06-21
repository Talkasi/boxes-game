#ifndef BOXES_GAME_LEVELS_H
#define BOXES_GAME_LEVELS_H

#define SCREEN_WIDTH 510
#define SCREEN_HEIGHT 510
#define STEP 51

#define N_FIELDS_WIDTH 10
#define N_FIELDS_HEIGHT 10

#define N_SUBFIELDS 100
#define N_LEVELS 2

#define DST 0
enum field {
    EMPTY,
    WALL,
    BOX,
    N_FIELD_TYPES
};

typedef int level_field[N_FIELDS_HEIGHT][N_FIELDS_WIDTH];

struct heroInfo {
    int x;
    int y;
    int i;
    int j;
};

struct subField {
    int x;
    int y;
    int type;
};

struct level {
    struct subField field[N_FIELDS_HEIGHT][N_FIELDS_WIDTH];
    struct heroInfo hero;
    int n_boxes;
    struct subField dst[N_SUBFIELDS];
};

void get_level(int n, struct level *level_dst);

#endif
