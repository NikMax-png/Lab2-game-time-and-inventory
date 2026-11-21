#ifndef MAP_H
#define MAP_H

#include "player.h"

#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30
#define VISION_RADIUS 4
#define BASE_REWARD_MULTIPLIER 10

typedef struct {
    char **grid;
    int width;
    int height;
    int floors_completed;
} GameMap;

void start_adventure(void);
void end_adventure(GameMap* map, int emergency);

#endif