#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

#define MAX_NAME_LEN 64
#define INVENTORY_SIZE 10
#define MAX_ITEMS 10

typedef struct {
    char name[MAX_NAME_LEN];
    int health;
    int max_health;
    int stamina;
    int max_stamina;
    int gold;
    int x; 
    int y;
    int inventory[INVENTORY_SIZE]; 
} Player;

extern Player* player;

void create_player();
void delete_player();
void show_player_card();
void load_item_names();
void view_inventory();
void put_item();
void remove_item();
void clear_trash();
void search_item_by_name();
void write_diary();
void count_resources();


extern char item_names[MAX_ITEMS][MAX_NAME_LEN];

#endif