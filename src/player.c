#include "player.h"

Player* player = NULL;
char item_names[MAX_ITEMS][MAX_NAME_LEN];

void load_item_names() {
    static const char *defaults[MAX_ITEMS] = {
        "Пусто", "Дерево", "Камень", "Семена", "Ягода",
        "Палка", "Гриб", "Яблоко", "Яйцо", "Ткань"
    };
    for (int i = 0; i < MAX_ITEMS; i++) {
        snprintf(item_names[i], MAX_NAME_LEN, "%s", defaults[i]);
    }

    FILE *file = fopen("items.txt", "r");
    if (file == NULL) return; // Используем дефолт

    int id;
    char name[MAX_NAME_LEN];
    while (fscanf(file, "%d %63s", &id, name) == 2) {
        if (id >= 0 && id < MAX_ITEMS) {
            snprintf(item_names[id], MAX_NAME_LEN, "%s", name);
        }
    }
    fclose(file);
}

void create_player() {
    if (player != NULL) {
        char confirm[10];
        printf("Персонаж уже существует. Вы уверены, что хотите удалить его и создать нового? (y/n): ");
        read_line(confirm, sizeof(confirm));
        if (confirm[0] != 'y' && confirm[0] != 'Y') {
            printf("Отменено.\n");
            return;
        }
        printf("Освобождение памяти по адресу: %p\n", (void*)player);
        free(player);
        player = NULL;
    }

    player = malloc(sizeof(Player));
    if (!player) {
        printf("Ошибка выделения памяти!\n");
        return;
    }

    printf("Введите имя героя: ");
    read_line(player->name, MAX_NAME_LEN);
    if (strlen(player->name) == 0) strcpy(player->name, "Герой");

    player->health = 100;
    player->max_health = 100;
    player->stamina = 50;
    player->max_stamina = 50;
    player->gold = 10;
    player->x = 0;
    player->y = 0;
    
    for(int i=0; i<INVENTORY_SIZE; i++) player->inventory[i] = 0;

    printf("Персонаж %s создан.\n", player->name);
}

void delete_player() {
    if (player != NULL) {
        printf("Удаление персонажа %s...\n", player->name);
        printf("Освобождение памяти по адресу: %p\n", (void*)player);
        free(player);
        player = NULL;
    } else {
        printf("Нет персонажа для удаления.\n");
    }
}

void show_player_card() {
    if (player == NULL) {
        printf("Ошибка: Персонаж не создан.\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char timestamp[64] = "N/A";
    if (local) strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local);

    printf("\n=== КАРТОЧКА ПЕРСОНАЖА ===\n");
    printf("Имя: %s\n", player->name);
    printf("Здоровье: %d/%d\n", player->health, player->max_health);
    printf("Выносливость: %d/%d\n", player->stamina, player->max_stamina);
    printf("Золото: %d\n", player->gold);
    printf("Дата и время системы: %s\n", timestamp);
    printf("Игровое время: День %d, %02d:00\n", current_day, current_hour);
    printf("Инвентарь:\n");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        int id = player->inventory[i];
        printf("  Слот %d: [%d] - %s\n", i, id, 
               (id >= 0 && id < MAX_ITEMS) ? item_names[id] : "Неизвестный предмет");
    }
    printf("===========================\n");
}

void view_inventory() {
    if (player == NULL) { printf("Нет персонажа.\n"); return; }
    show_player_card(); // Просто выводим карточку, она содержит инвентарь
}

void put_item() {
    if (player == NULL) { printf("Нет персонажа.\n"); return; }
    int index, item_id;
    printf("Введите индекс слота (0-%d): ", INVENTORY_SIZE - 1);
    scanf("%d", &index);
    clear_stdin();
    if (index < 0 || index >= INVENTORY_SIZE) { printf("Некорректный индекс.\n"); return; }
    
    printf("Введите ID предмета (0-%d): ", MAX_ITEMS - 1);
    scanf("%d", &item_id);
    clear_stdin();
    if (item_id < 0 || item_id >= MAX_ITEMS) { printf("Некорректный ID.\n"); return; }

    player->inventory[index] = item_id;
    printf("Предмет %s добавлен.\n", item_names[item_id]);
}

void remove_item() {
    if (player == NULL) { printf("Нет персонажа.\n"); return; }
    int index;
    printf("Введите индекс слота для очистки: ");
    scanf("%d", &index);
    clear_stdin();
    if (index >= 0 && index < INVENTORY_SIZE) {
        player->inventory[index] = 0;
        printf("Слот очищен.\n");
    }
}

void clear_trash() {
    if (player == NULL) { printf("Нет персонажа.\n"); return; }
    int item_id, count = 0;
    printf("Введите ID предмета для массового удаления: ");
    scanf("%d", &item_id);
    clear_stdin();
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (player->inventory[i] == item_id) {
            player->inventory[i] = 0;
            count++;
        }
    }
    printf("Удалено %d предметов.\n", count);
}

void search_item_by_name() {
    if (player == NULL) { printf("Нет персонажа.\n"); return; }
    char name[MAX_NAME_LEN];
    printf("Введите название предмета: ");
    read_line(name, MAX_NAME_LEN);
    
    int found_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(item_names[i], name) == 0) { found_id = i; break; }
    }
    if (found_id < 0) { printf("Такого предмета нет в базе.\n"); return; }

    int found = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (player->inventory[i] == found_id) {
            printf("\"%s\" найден в слоте %d\n", name, i);
            found = 1;
        }
    }
    if (!found) printf("В рюкзаке данного предмета нет.\n");
}

void write_diary() {
    if (player == NULL) { printf("Нет персонажа.\n"); return; }
    FILE *file = fopen("diary.txt", "a");
    if (!file) { printf("Ошибка файла.\n"); return; }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char timestamp[64] = "unknown";
    if(local) strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local);

    fprintf(file, "--- Diary Entry ---\n");
    fprintf(file, "Hero: %s\n", player->name);
    fprintf(file, "Game Time: Day %d, %02d:00\n", current_day, current_hour);
    fprintf(file, "Real Time: %s\n", timestamp);
    fprintf(file, "Gold: %d, HP: %d/%d\n", player->gold, player->health, player->max_health);
    fprintf(file, "Inventory:\n");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        int id = player->inventory[i];
        if(id != 0) fprintf(file, "  Slot %d: %s\n", i, item_names[id]);
    }
    fprintf(file, "-------------------\n");
    fclose(file);
    printf("Запись в дневник сохранена.\n");
}

void count_resources() {
    // Логика подсчета из input.txt остается прежней, но требует проверки на существование файлов
    FILE *input = fopen("input.txt", "r");
    if (!input) { printf("input.txt не найден.\n"); return; }
    
    char names[MAX_ITEMS][MAX_NAME_LEN]; // Используем лимиты из player.h
    int totals[MAX_ITEMS] = {0};
    int count = 0;
    char name[MAX_NAME_LEN];
    int quantity;

    while (fscanf(input, "%63s %d", name, &quantity) == 2) {
        int index;
        for (index = 0; index < count; index++) {
            if (strcmp(names[index], name) == 0) break;
        }
        if (index == count) {
            if (count >= MAX_ITEMS) break;
            strcpy(names[count], name);
            totals[count] = 0;
            count++;
        }
        totals[index] += quantity;
    }
    fclose(input);

    FILE *output = fopen("output.txt", "w");
    if (!output) { printf("Ошибка вывода.\n"); return; }
    
    printf("--- Ресурсы ---\n");
    for (int i = 0; i < count; i++) {
        printf("%s: %d шт.\n", names[i], totals[i]);
        fprintf(output, "%s: %d шт.\n", names[i], totals[i]);
    }
    fclose(output);
    printf("Сохранено в output.txt.\n");
}