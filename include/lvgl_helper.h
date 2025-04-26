#pragma once
#include <lvgl.h>

void lvgl_setup();       // À appeler dans setup()
void lvgl_loop();        // À appeler dans loop()
void screen_set_brightness_percent(int percent); // Ajuster la luminosité de l'écran
