#pragma once
#include <lvgl.h>

namespace ui_manager {
    void init();                         // à appeler dans setup()
    void show_screen(lv_obj_t *screen, bool show_home_btn = true);  // changer la zone principale
}