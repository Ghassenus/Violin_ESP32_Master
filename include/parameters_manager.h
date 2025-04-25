#pragma once
#include <time.h>
void parameters_manager_init();
void parameters_manager_set_hour(int);
void parameters_manager_set_minute(int);
void parameters_manager_set_date(int y,int m,int d);
void parameters_manager_set_time_format(bool);
bool parameters_manager_is_24h_format();
void parameters_manager_get_date(int &year, int &month, int &day);
int parameters_manager_get_minute();
int parameters_manager_get_hour();
