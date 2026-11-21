#include <stdio.h>
#include "utils.h"
#include "player.h"
#include "map.h"

void run_menu(void) {
    int choice;
    
    load_item_names();

    generate_new_weather();

    while (1) {
        printf("\n================ МЕНЮ ================\n");
        if (player) {
            printf("Персонаж: %s (HP:%d, ST:%d, Gold:%d)\n", 
                   player->name, player->health, player->stamina, player->gold);
        } else {
            printf("Нет персонажа.\n");
        }
        printf("День: %d, время: %02d:00 | Погода: %s\n", 
               current_day, current_hour, get_weather_name(global_weather.type));
        
        printf("[0] Выход\n");
        printf("[1] Создать/Пересоздать персонажа\n");
        printf("[2] Карточка персонажа\n");
        printf("[3] Посмотреть на часы / Погоду\n");
        printf("[4] Промотать время\n");
        printf("[5] Отдохнуть\n");
        printf("[6] Обсмотреть инвентарь\n");
        printf("[7] Положить предмет\n");
        printf("[8] Выбросить предмет\n");
        printf("[9] Очистить мусор\n");
        printf("[10] Поиск предмета\n");
        printf("[11] Записать в дневник\n");
        printf("[12] Подсчёт ресурсов (input.txt)\n");
        printf("[13] Отправиться в приключение\n");
        printf("Выбор: ");

        if (scanf("%d", &choice) != 1) {
            clear_stdin();
            continue;
        }
        clear_stdin();

        switch (choice) {
            case 0:
                if (player) {
                    printf("Освобождение памяти игрока: %p\n", (void*)player);
                    free(player);
                }
                printf("До свидания!\n");
                return;
            case 1:
                create_player();
                break;
            case 2:
                show_player_card();
                break;
            case 3:
                printf("День: %d, Час: %02d\n", current_day, current_hour);
                printf("Погода: %s, Темп: %d C, Ветер: %d\n", 
                       get_weather_name(global_weather.type), 
                       global_weather.temperature, 
                       global_weather.wind_strength);
                break;
            case 4: {
                if (player == NULL) { printf("Нет персонажа.\n"); break; }
                int h;
                printf("Сколько часов ждать? (мин 1): ");
                if (scanf("%d", &h) == 1) {
                    if (h < 1) h = 1;
                    update_time(h);
                }
                clear_stdin();
                break;
            }
            case 5:
                if (player == NULL) { printf("Нет персонажа.\n"); break; }
                sleep_until_morning();
                player->health = player->max_health;
                player->stamina = player->max_stamina;
                printf("Вы полностью восстановлены.\n");
                break;
            case 6:
                view_inventory();
                break;
            case 7:
                put_item();
                break;
            case 8:
                remove_item();
                break;
            case 9:
                clear_trash();
                break;
            case 10:
                search_item_by_name();
                break;
            case 11:
                write_diary();
                break;
            case 12:
                count_resources();
                break;
            case 13:
                start_adventure();
                break;
            default:
                printf("Неверный пункт.\n");
        }
    }
}

int main(void) {
    srand((unsigned)time(NULL));
    run_menu();
    return 0;
}