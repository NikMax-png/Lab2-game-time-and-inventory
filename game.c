#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define INVENTORY_SIZE 10
#define MAX_ITEMS 10
#define MAX_NAME_LEN 64
#define MAX_FARMER_NAME_LEN 33
#define MAX_RESOURCES 10
#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30
#define VISION_RADIUS 4

int current_day = 1;
int current_hour = 8;
int inventory[INVENTORY_SIZE] = {0};
int adventure_depth = 0;

char item_names[MAX_ITEMS][MAX_NAME_LEN];
char player_name[MAX_FARMER_NAME_LEN];

static int inside_map(int x, int y, int width, int height) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

static void free_map(char **map, int height) {
    if (map == NULL) return;
    for (int y = 0; y < height; y++) {
        free(map[y]);
    }
    free(map);
}

static void draw_map(
    char **map, int width, int height,
    int player_x, int player_y,
    int up_x, int up_y, int has_up
) {
    printf("\nЭтаж глубины: %d\n", adventure_depth);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int dx = x - player_x;
            int dy = y - player_y;

            if (dx * dx + dy * dy > VISION_RADIUS * VISION_RADIUS) {
                putchar('?');
            } else if (x == player_x && y == player_y) {
                putchar('@');
            } else if (has_up && x == up_x && y == up_y) {
                putchar('U');
            } else {
                putchar(map[y][x]); 
            }
        }
        putchar('\n');
    }
}

static int generate_floor(
    char ***map_out, int *width_out, int *height_out,
    int *player_x, int *player_y, int *up_x, int *up_y,
    int has_up
) {
    int width = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    int height = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);

    char **map = malloc((size_t)height * sizeof(char *)); 
    if (map == NULL) {
        printf("Не удалось выделить память для массива строк карты.\n");
        return 0;
    }

    for (int y = 0; y < height; y++) { 
        map[y] = malloc((size_t)width * sizeof(char));
        if (map[y] == NULL) {
            for (int i = 0; i < y; i++) {
                free(map[i]);
            }
            free(map);
            printf("Не удалось выделить память для строки %d.\n", y);
            return 0;
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == 0 || y == 0 || x == width - 1 || y == height - 1) {
                map[y][x] = '#';
            } else {
                map[y][x] = '.';
            }
        }
    }

    *player_x = 1 + rand() % (width - 2);
    *player_y = 1 + rand() % (height - 2);

    if (has_up) {
        int dirs[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
        int order[4] = {0, 1, 2, 3};

        for (int i = 3; i > 0; i--) {
            int j = rand() % (i + 1);
            int temp = order[i];
            order[i] = order[j];
            order[j] = temp;
        }

        *up_x = *player_x;
        *up_y = *player_y;

        for (int i = 0; i < 4; i++) {
            int dir = order[i];
            int x = *player_x + dirs[dir][0];
            int y = *player_y + dirs[dir][1];

            if (x > 0 && x < width - 1 && y > 0 && y < height - 1) {
                *up_x = x;
                *up_y = y;
                break;
            }
        }
    }

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if ((x == *player_x && y == *player_y) ||
                (has_up && x == *up_x && y == *up_y)) {
                continue;
            }

            if (rand() % 100 < 15) {
                map[y][x] = (rand() % 2 == 0) ? 'B' : 'C';
            }
        }
    }

    int down_x, down_y;
    do {
        down_x = 1 + rand() % (width - 2);
        down_y = 1 + rand() % (height - 2);
    } while ((down_x == *player_x && down_y == *player_y) ||
             (has_up && down_x == *up_x && down_y == *up_y));

    map[down_y][down_x] = 'D';

    *map_out = map;
    *width_out = width;
    *height_out = height;
    return 1;
}

void read_line(char *buffer, size_t size);
void load_item_names();
void see_time();
void work_hours();
void view_inventory();
void put_item();
void remove_item();
void clear_trash();
void search_item_by_name();
void write_diary();
void count_resources();
void adventure();
void initialize_game();
void run_program();

int main() {
    initialize_game();
    run_program();
    return 0;
}

void initialize_game() {
    load_item_names();

    printf("Введите имя фермера (до 32 символов): ");
    read_line(player_name, sizeof(player_name));
    if (player_name[0] == '\0') {
        strcpy(player_name, "Игрок");
    }
}

void run_program() {
    int choice;

    while (1) {
        printf("\n=== Добро пожаловать, %s! ===\n", player_name);
        printf("[0] Выход\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Промотать время (Поработать)\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Очистка от мусора\n");
        printf("[7] Поиск предмета по названию\n");
        printf("[8] Записать состояние в дневник фермера\n");
        printf("[9] Подсчёт ресурсов из input.txt\n");
        printf("[10] Отправиться в приключение\n");
        printf("Выберите пункт: ");

        if (scanf("%d", &choice) != 1) {
            printf("Некорректный ввод.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }

        switch (choice) {
            case 0:
                printf("Выход из программы.\n");
                return;
            case 1:
                see_time();
                break;
            case 2:
                work_hours();
                break;
            case 3:
                view_inventory();
                break;
            case 4:
                put_item();
                break;
            case 5:
                remove_item();
                break;
            case 6:
                clear_trash();
                break;
            case 7:
                search_item_by_name();
                break;
            case 8:
                write_diary();
                break;
            case 9:
                count_resources();
                break;
            case 10:
                adventure();
                break;
            default:
                printf("Некорректный выбор.\n");
        }
    }
}

void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
}

void load_item_names() {
    static const char *defaults[MAX_ITEMS] = {
        "Пусто", "Дерево", "Камень", "Семена", "Ягода",
        "Палка", "Гриб", "Яблоко", "Яйцо", "Ткань"
    };

    for (int i = 0; i < MAX_ITEMS; i++) {
        snprintf(item_names[i], MAX_NAME_LEN, "%s", defaults[i]);
    }

    FILE *file = fopen("items.txt", "r");
    if (file == NULL) {
        printf("Ошибка: items.txt не найден. Используются стандартные названия.\n");
        return;
    }

    int id;
    char name[MAX_NAME_LEN];
    while (fscanf(file, "%d %63s", &id, name) == 2) {
        if (id >= 0 && id < MAX_ITEMS) {
            snprintf(item_names[id], MAX_NAME_LEN, "%s", name);
        }
    }

    fclose(file);
}

void see_time() {
    printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
}

void work_hours() {
    int hours;
    printf("Введите количество часов для работы: ");
    scanf("%d", &hours);
    if (hours < 0) {
        printf("Некорректное значение часов.\n");
        return;
    }
    current_hour += hours;
    while (current_hour >= 24) {
        current_hour -= 24;
        current_day++;
    }
}

void view_inventory() {
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        int id = inventory[i];
        printf("Слот %d: [%d] - %s\n", i, id,
               (id >= 0 && id < MAX_ITEMS) ? item_names[id] : "Неизвестный предмет");
    }
}

void put_item() {
    int index, item_id;
    printf("Введите индекс слота (0-%d): ", 10 - 1);
    scanf("%d", &index);
    if (index < 0 || index >= 10) {
        printf("Некорректный индекс.\n");
        return;
    }
    printf("Введите ID предмета (0-9): ");
    scanf("%d", &item_id);
    if (item_id < 0 || item_id > 9) {
        printf("Некорректный ID предмета.\n");
        return;
    }
    inventory[index] = item_id;
    printf("Предмет %s добавлен в слот %d.\n", item_names[item_id], index);
}

void remove_item() {
    int index;
    printf("Введите индекс слота (0-%d): ", 10 - 1);
    scanf("%d", &index);
    if (index < 0 || index >= 10) {
        printf("Некорректный индекс.\n");
        return;
    }
    inventory[index] = 0;
}

void clear_trash() {
    int item_id, count = 0;
    printf("Введите ID предмета для удаления: ");
    scanf("%d", &item_id);
    for (int i = 0; i < 10; i++) {
        if (inventory[i] == item_id) {
            inventory[i] = 0;
            count++;
        }
    }
    printf("Удалено %d предметов.\n", count);
}

void search_item_by_name() {
    char name[MAX_NAME_LEN];   
    printf("Введите название предмета: ");
    fflush(stdout);
    if (scanf("%63s", name) != 1) { 
        while (getchar() != '\n'); 
        return; 
    }
    int found_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (item_names[i][0] != '\0' && strcmp(item_names[i], name) == 0) { 
            found_id = i; 
            break; 
        }
    }
    if (found_id < 0) {
        printf("Предмета с таким названием нет в каталоге.\n");
        return;
    }
    int found = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i] == found_id) {
            printf(" \"%s\" найден в слоте %d\n", name, i);
            found = 1;
        }
    }
    if (!found) {
        printf("В рюкзаке данного предмета нет.\n");
    }
}

void write_diary() {
    FILE *file = fopen("diary.txt", "a");
    if (file == NULL) {
        printf("Не удалось открыть diary.txt для записи.\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char timestamp[64] = "время неизвестно";

    if (local != NULL) {
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local);
    }

    printf("\n--- Дневник фермера ---\n");
    printf("Фермер: %s\n", player_name);
    printf("Игровое время: день %d, %02d:00\n", current_day, current_hour);
    printf("Дата и время записи: %s\n", timestamp);
    printf("Инвентарь:\n");

    fprintf(file, "--- Дневник фермера ---\n");
    fprintf(file, "Фермер: %s\n", player_name);
    fprintf(file, "Игровое время: день %d, %02d:00\n", current_day, current_hour);
    fprintf(file, "Дата и время записи: %s\n", timestamp);
    fprintf(file, "Инвентарь:\n");

    for (int i = 0; i < INVENTORY_SIZE; i++) {
        int id = inventory[i];
        const char *name = (id >= 0 && id < MAX_ITEMS)
                               ? item_names[id]
                               : "Неизвестный предмет";
        printf("Слот %d: [%d] - %s\n", i, id, name);
        fprintf(file, "Слот %d: [%d] - %s\n", i, id, name);
    }

    printf("-----------------------\n");
    fprintf(file, "-----------------------\n");
    fclose(file);
    printf("Запись добавлена в diary.txt.\n");
}

void count_resources() {
    char names[MAX_RESOURCES][MAX_NAME_LEN];
    int totals[MAX_RESOURCES] = {0};
    int count = 0;
    char name[MAX_NAME_LEN];
    int quantity;

    FILE *input = fopen("input.txt", "r");
    if (input == NULL) {
        printf("Не удалось открыть input.txt.\n");
        return;
    }

    while (fscanf(input, "%63s %d", name, &quantity) == 2) {
        int index;
        for (index = 0; index < count; index++) {
            if (strcmp(names[index], name) == 0) {
                break;
            }
        }

        if (index == count) {
            if (count >= MAX_RESOURCES) {
                printf("Слишком много разных ресурсов; оставшиеся записи пропущены.\n");
                break;
            }
            snprintf(names[count], MAX_NAME_LEN, "%s", name);
            totals[count] = 0;
            count++;
        }

        totals[index] += quantity;
    }
    fclose(input);

    FILE *output = fopen("output.txt", "w");
    if (output == NULL) {
        printf("Не удалось открыть output.txt для записи.\n");
        return;
    }

    printf("--- Сводная ведомость ---\n");
    for (int i = 0; i < count; i++) {
        printf("%s: %d шт.\n", names[i], totals[i]);
        fprintf(output, "%s: %d шт.\n", names[i], totals[i]);
    }

    fclose(output);
    printf("Ведомость записана в output.txt.\n");
}

void adventure() {
    char **map = NULL;
    int width, height;
    int player_x, player_y, up_x = -1, up_y = -1;
    int has_up = 0;

    adventure_depth = 1;

    if (!generate_floor(&map, &width, &height,
                        &player_x, &player_y, &up_x, &up_y, has_up)) {
        return;
    }

    printf("\nПриключение началось! Управление: W/A/S/D, Q — домой с лестницы U.\n");

    while (1) {
        draw_map(map, width, height, player_x, player_y, up_x, up_y, has_up);
        printf("Ваш ход (W/A/S/D/Q): ");

        char input[32];
        read_line(input, sizeof(input));
        char command = input[0];

        if (command >= 'a' && command <= 'z') {
            command = (char)(command - 'a' + 'A');
        }

        if (command == 'Q') {
            if (has_up && player_x == up_x && player_y == up_y) {
                free_map(map, height); 
                printf("Вы вернулись домой.\n");
                return;
            }
            printf("Вернуться домой можно только стоя на лестнице U.\n");
            continue;
        }

        int new_x = player_x;
        int new_y = player_y;

        switch (command) {
            case 'W': new_y--; break;
            case 'A': new_x--; break;
            case 'S': new_y++; break;
            case 'D': new_x++; break;
            default:
                printf("Неизвестная команда. Используйте W/A/S/D или Q.\n");
                continue;
        }

        if (!inside_map(new_x, new_y, width, height)) {
            continue;
        }

        char destination = map[new_y][new_x]; 
        if (destination == '#' || destination == 'B' || destination == 'C') {
            printf("Путь преграждает препятствие.\n");
            continue;
        }

        player_x = new_x;
        player_y = new_y;

        if (destination == 'D') {
            free_map(map, height); 
            adventure_depth++;
            has_up = 1;

            if (!generate_floor(&map, &width, &height,
                                &player_x, &player_y, &up_x, &up_y, has_up)) {
                printf("Приключение завершено из-за ошибки памяти.\n");
                return;
            }

            printf("Вы спустились на новый этаж.\n");
        }
    }
}
