#include "utils.h"

int current_day = 1;
int current_hour = 8;
Weather global_weather = {WEATHER_CLEAR, 20, 0};

void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        clear_stdin();
    }
}

void clear_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void update_time(int hours_to_skip) {
    if (hours_to_skip <= 0) return;
    
    if (current_hour + hours_to_skip >= 26) { 
         
         int target_total_hours = current_hour + hours_to_skip;
         int target_day_offset = target_total_hours / 24;
         int target_hour_mod = target_total_hours % 24;
         
         if (target_day_offset > 0 && target_hour_mod >= 2) {
             printf("Нельзя бодрствовать так долго! Нужно лечь спать.\n");
             return;
         }
    }
    
    current_hour += hours_to_skip;
    while (current_hour >= 24) {
        current_hour -= 24;
        current_day++;
        generate_new_weather(); 
    }
}

void sleep_until_morning(void) {
    current_day++;
    current_hour = 8;
    generate_new_weather();
    printf("Вы хорошо выспались. Наступил День %d, утро 08:00.\n", current_day);
}

void generate_new_weather(void) {
    srand((unsigned)time(NULL) ^ (current_day * 100));
    global_weather.type = (WeatherType)(rand() % 4);
    global_weather.temperature = 10 + rand() % 20;
    global_weather.wind_strength = (global_weather.type == WEATHER_STORM) ? (5 + rand() % 6) : (rand() % 3);
}

const char* get_weather_name(WeatherType type) {
    switch(type) {
        case WEATHER_CLEAR: return "Ясно";
        case WEATHER_RAIN: return "Ливень";
        case WEATHER_FOG: return "Туман";
        case WEATHER_STORM: return "Ураган";
        default: return "Неизвестно";
    }
}

void print_hud_status(const char* player_name, int hp, int max_hp, int stam, int max_stam, 
                      int gold, int floor, int day, int hour) {
    printf("\n[HUD] Игрок: %s | HP: %d/%d | Выносливость: %d/%d | Золото: %d\n",
           player_name, hp, max_hp, stam, max_stam, gold);
    printf("[HUD] Этаж: %d | Время: День %d, %02d:00 | Погода: %s (%d°C)\n",
           floor, day, hour, get_weather_name(global_weather.type), global_weather.temperature);
    printf("--------------------------------------------------\n");
}