#include "map.h"
#include "utils.h"
#include <math.h>

static int inside_map(int x, int y, int width, int height) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

static void free_grid(char **grid, int height) {
    if (!grid) return;
    for (int y = 0; y < height; y++) free(grid[y]);
    free(grid);
}

static GameMap* generate_floor(int depth) {
    GameMap* map = malloc(sizeof(GameMap));
    if (!map) return NULL;

    map->width = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    map->height = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    map->floors_completed = depth;

    map->grid = malloc((size_t)map->height * sizeof(char*));
    for (int y = 0; y < map->height; y++) {
        map->grid[y] = malloc((size_t)map->width * sizeof(char));
    }

    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            if (x == 0 || y == 0 || x == map->width - 1 || y == map->height - 1)
                map->grid[y][x] = '#';
            else
                map->grid[y][x] = '.';
        }
    }

    player->x = 1 + rand() % (map->width - 2);
    player->y = 1 + rand() % (map->height - 2);

    int up_x = -1, up_y = -1;
    if (depth > 1) {
        do {
            up_x = 1 + rand() % (map->width - 2);
            up_y = 1 + rand() % (map->height - 2);
        } while (map->grid[up_y][up_x] != '.' || (up_x == player->x && up_y == player->y));
        map->grid[up_y][up_x] = 'U';
    }

    int down_x, down_y;
    do {
        down_x = 1 + rand() % (map->width - 2);
        down_y = 1 + rand() % (map->height - 2);
    } while (map->grid[down_y][down_x] != '.' || 
             (down_x == player->x && down_y == player->y) ||
             (up_x != -1 && down_x == up_x && down_y == up_y));
    map->grid[down_y][down_x] = 'D';

    for (int y = 1; y < map->height - 1; y++) {
        for (int x = 1; x < map->width - 1; x++) {
            if (map->grid[y][x] == '.' && rand() % 100 < 15) {
                map->grid[y][x] = (rand() % 2 == 0) ? 'B' : 'C';
            }
        }
    }

    return map;
}

static void draw_map(GameMap* map) {
    printf("\nЭтаж глубины: %d\n", map->floors_completed);
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            int dx = x - player->x;
            int dy = y - player->y;
            
            int vision = VISION_RADIUS;
            if (global_weather.type == WEATHER_FOG) vision = 2;

            if (dx*dx + dy*dy > vision*vision) {
                putchar('?');
            } else if (x == player->x && y == player->y) {
                putchar('@');
            } else {
                putchar(map->grid[y][x]);
            }
        }
        putchar('\n');
    }
}

void end_adventure(GameMap* map, int emergency) {
    if (map) {
        free_grid(map->grid, map->height);
        free(map);
    }
    if (emergency) {
        printf("\n*** ОБМОРОК ***\n");
        printf("Вы упали без сил! Спасатели забрали половину золота.\n");
        if (player) {
            player->gold /= 2;
            player->stamina = player->max_stamina; // Восстановились дома
            player->health = player->max_health;
        }
    } else {
        printf("\nВы успешно вернулись домой.\n");
    }
}

void start_adventure(void) {
    if (player == NULL) {
        printf("Сначала создайте персонажа!\n");
        return;
    }
    if (player->stamina <= 0) {
        printf("Вы слишком устали. Отдохните.\n");
        return;
    }
    if (current_hour >= 2 && current_hour < 8) {
        printf("Сейчас ночь (02:00–08:00). Идите спать, утром отправитесь в приключение.\n");
        return;
    }

    printf("Начало приключения... Текущая погода: %s\n", get_weather_name(global_weather.type));
    
    GameMap* map = generate_floor(1);
    if (!map) {
        printf("Ошибка генерации карты.\n");
        return;
    }

    int running = 1;
    while (running) {
        if (player->stamina <= 0 || current_hour >= 2 && current_hour < 8) {
            end_adventure(map, 1);
            return;
        }

        draw_map(map);
        print_hud_status(player->name, player->health, player->max_health,
                         player->stamina, player->max_stamina, player->gold,
                         map->floors_completed, current_day, current_hour);

        char input[32];
        printf("Команда (W/A/S/D/Q): ");
        read_line(input, sizeof(input));
        char cmd = input[0];
        if (cmd >= 'a' && cmd <= 'z') cmd -= 32;

        if (cmd == 'Q') {
            if (map->grid[player->y][player->x] == 'U') {
                end_adventure(map, 0);
                return;
            } else {
                printf("Вы должны стоять на лестнице (U), чтобы уйти домой.\n");
                continue;
            }
        }

        int new_x = player->x;
        int new_y = player->y;
        int moved = 0;

        switch (cmd) {
            case 'W': new_y--; moved = 1; break;
            case 'A': new_x--; moved = 1; break;
            case 'S': new_y++; moved = 1; break;
            case 'D': new_x++; moved = 1; break;
            default: 
                printf("Неверная команда.\n");
                continue;
        }

        if (!moved) continue;

        int stamina_cost = 1;
        if (global_weather.type == WEATHER_RAIN) {
            stamina_cost = 2;
        }

        if (global_weather.type == WEATHER_STORM) {
            if (rand() % 100 < 20) {
                printf("Порыв ветра сдул вас!\n");
                int dirs[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
                int d = rand() % 4;
                new_x = player->x + dirs[d][0];
                new_y = player->y + dirs[d][1];
            }
        }

        if (!inside_map(new_x, new_y, map->width, map->height)) {
            printf("Стена/Граница.\n");
            continue;
        }

        char dest = map->grid[new_y][new_x];
        if (dest == '#' || dest == 'B' || dest == 'C') {
            printf("Препятствие.\n");
            continue;
        }

        player->x = new_x;
        player->y = new_y;
        
        player->stamina -= stamina_cost;
        update_time(0);
        
        if (player->stamina < 0) player->stamina = 0;

        if (dest == 'D') {
            printf("Вы спускаетесь глубже...\n");
            player->stamina -= 5;
            update_time(1); // 1 час на переход
            
            int reward = BASE_REWARD_MULTIPLIER * map->floors_completed;
            player->gold += reward;
            printf("Найдено сокровище: +%d золота!\n", reward);

            int old_depth = map->floors_completed;
            free_grid(map->grid, map->height);
            free(map);
            
            map = generate_floor(old_depth + 1);
            if (!map) {
                printf("Ошибка при переходе.\n");
                break;
            }
        }
    }
    
    end_adventure(map, 0);
}