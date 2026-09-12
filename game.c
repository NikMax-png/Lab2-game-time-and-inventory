#include <stdio.h>
#define INVENTORY_SIZE 10

int current_day = 1;
int current_hour = 8;
int inventory[10] = {0};

int main() {
    int choice;
    while (1) {
        printf("\nМеню:\n");
        printf("[0] Выход\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Промотать время (Поработать)\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Очистка от мусора\n");
        printf("Выберите пункт: ");
        scanf("%d", &choice);

        switch (choice) {
            case 0:
                printf("Выход из программы.\n");
                return 0;
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
    return 0;
}