#include <stdio.h>
#include <string.h>
#include <time.h>

#define INVENTORY_SIZE 10
#define MAX_ITEMS 10
#define MAX_NAME_LEN 64
#define MAX_FARMER_NAME_LEN 33
#define MAX_RESOURCES 10

int current_day = 1;
int current_hour = 8;
int inventory[INVENTORY_SIZE] = {0};

char item_names[MAX_ITEMS][MAX_NAME_LEN];
char player_name[MAX_FARMER_NAME_LEN];

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
        printf("[7] Поиск предмета по названию\n")
        printf("[8] Записать состояние в дневник\n")
        printf("[9] Подсчёт ресурсов из файла\n")
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

