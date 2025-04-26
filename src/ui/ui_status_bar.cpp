#include "ui_status_bar.h"
#include <lvgl.h>
#include"battery.h"
#include"ota_manager.h"
#include <time.h>
#include <WiFi.h>
#include <parameters_manager.h>
#include "violin_colors.h"

static lv_obj_t* lbl_time;
static lv_obj_t* icon_wifi;
static lv_obj_t* icon_batt;
static lv_obj_t* icon_ota;
static lv_obj_t* lbl_title;

static void _status_bar_timer_cb(lv_timer_t*) {
    ui_status_bar_update_time();
    ui_status_bar_set_battery(battery_get_percent());
    ui_status_bar_set_wifi(WiFi.status() == WL_CONNECTED);
    ui_status_bar_notify_ota(ota_manager_is_running());
}

lv_obj_t* ui_status_bar_create() {
    // Crée un conteneur de barre
    lv_obj_t *bar = lv_obj_create(lv_scr_act()); // ATTENTION : parent = écran actif !
    lv_obj_set_size(bar, LV_PCT(100), LV_PCT(10));
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_set_style_bg_color(bar, lv_color_hex(0x000000), 0); // fond noir
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);

    // Supprime le scroll
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    // Heure à gauche
lbl_time = lv_label_create(bar);
lv_label_set_text(lbl_time, "--:--");
lv_obj_set_style_text_color(lbl_time, lv_color_white(), 0);
lv_obj_align(lbl_time, LV_ALIGN_LEFT_MID, 5, 0);

// Titre centré
lbl_title = lv_label_create(bar);
lv_label_set_text(lbl_title, "");
lv_obj_set_style_text_color(lbl_title, lv_color_white(), 0);
lv_obj_align(lbl_title, LV_ALIGN_CENTER, 0, 0);

// Icône OTA (droite)
icon_ota = lv_label_create(bar);
lv_label_set_text(icon_ota, LV_SYMBOL_REFRESH);
lv_obj_add_flag(icon_ota, LV_OBJ_FLAG_HIDDEN);
lv_obj_set_style_text_color(icon_ota, lv_color_hex(0xFF0000), 0); // rouge
lv_obj_align(icon_ota, LV_ALIGN_RIGHT_MID, -5, 0);

// Batterie ← OTA
icon_batt = lv_label_create(bar);
lv_label_set_text(icon_batt, LV_SYMBOL_BATTERY_FULL);
lv_obj_align_to(icon_batt, icon_ota, LV_ALIGN_OUT_LEFT_MID, -5, 0);

// WiFi ← Batterie
icon_wifi = lv_label_create(bar);
lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);
lv_obj_align_to(icon_wifi, icon_batt, LV_ALIGN_OUT_LEFT_MID, -5, 0);

// Démarre le timer
lv_timer_create(_status_bar_timer_cb, 5000, nullptr);

    return bar;
}



void ui_status_bar_set_battery(int level_percent) {
    const char* sym = LV_SYMBOL_BATTERY_EMPTY;
    if      (level_percent > 80) { sym = LV_SYMBOL_BATTERY_FULL; lv_obj_set_style_text_color(icon_batt, col_green, 0);}
    else if (level_percent > 60) { sym = LV_SYMBOL_BATTERY_3; lv_obj_set_style_text_color(icon_batt, col_green, 0);} 
    else if (level_percent > 40) { sym = LV_SYMBOL_BATTERY_2; lv_obj_set_style_text_color(icon_batt, col_yellow, 0);}
    else if (level_percent > 20) { sym = LV_SYMBOL_BATTERY_1; lv_obj_set_style_text_color(icon_batt, col_red, 0);}
    else { sym = LV_SYMBOL_BATTERY_EMPTY; lv_obj_set_style_text_color(icon_batt, col_red, 0);}   
    lv_label_set_text(icon_batt, sym);
}


void ui_status_bar_set_wifi(bool connected) {
    lv_label_set_text(icon_wifi, connected ? LV_SYMBOL_WIFI : LV_SYMBOL_CLOSE);
    lv_obj_set_style_text_color(icon_wifi, connected ? col_green : col_yellow, 0);
}

void ui_status_bar_update_time() {
    time_t now = time(nullptr);
    struct tm tm;
    localtime_r(&now, &tm);
    char buf[8];

     if (parameters_manager_is_24h_format()) {
        snprintf(buf, sizeof(buf), "%02d:%02d", tm.tm_hour, tm.tm_min);
    } else {
        int hour = tm.tm_hour % 12;
        if (hour == 0) hour = 12;
        snprintf(buf, sizeof(buf), "%2d:%02d%s", hour, tm.tm_min, tm.tm_hour >= 12 ? "pm" : "am");
    }

    lv_label_set_text(lbl_time, buf);
}

void ui_status_bar_notify_ota(bool running) {
    if (running)
        lv_obj_clear_flag(icon_ota, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(icon_ota, LV_OBJ_FLAG_HIDDEN);
}

void ui_status_bar_set_title(const char* txt) {
    lv_label_set_text(lbl_title, txt);
}

//surcharge pour les String si besoin
void ui_status_bar_set_title(const String& txt) {
    lv_label_set_text(lbl_title, txt.c_str());
}