#ifndef BOXES_GAME_LEVELS_H
#define BOXES_GAME_LEVELS_H

#define SCREEN_WIDTH 510
#define SCREEN_HEIGHT 510
#define STEP 51

#define N_FIELDS_WIDTH 10
#define N_FIELDS_HEIGHT 10

#define N_SUBFIELDS 100

#define N_LEVELS 2
#define N_DST 10

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

struct levelField {
    struct subField field[N_FIELDS_HEIGHT][N_FIELDS_WIDTH];
    struct heroInfo hero;
    int n_boxes;
    struct subField dst[N_SUBFIELDS];
};

void get_level(int n, struct levelField *level_dst);

#endif
