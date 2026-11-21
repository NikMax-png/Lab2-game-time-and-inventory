#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
    WEATHER_CLEAR = 0,   
    WEATHER_RAIN,        
    WEATHER_FOG,         
    WEATHER_STORM        
} WeatherType;

typedef struct {
    WeatherType type;
    int temperature;     
    int wind_strength;   
} Weather;

extern int current_day;
extern int current_hour;
extern Weather global_weather;

void read_line(char *buffer, size_t size);
void clear_stdin(void);
void update_time(int hours_to_skip);
void sleep_until_morning(void);
void generate_new_weather(void);
const char* get_weather_name(WeatherType type);
void print_hud_status(const char* player_name, int hp, int max_hp, int stam, int max_stam, 
                      int gold, int floor, int day, int hour);

#endif