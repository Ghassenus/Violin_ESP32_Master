#include "ui_settings_pages.h"

// === Page Date/Heure ===
lv_obj_t* ui_settings_datetime_create() {
    lv_obj_t* cont = lv_obj_create(NULL);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Réglages Date & Heure");
    lv_obj_center(label);

    return cont;
}

// === Page Affichage ===
lv_obj_t* ui_settings_display_create() {
    lv_obj_t* cont = lv_obj_create(NULL);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Réglages Affichage");
    lv_obj_center(label);

    return cont;
}

// === Page Audio ===
lv_obj_t* ui_settings_audio_create() {
    lv_obj_t* cont = lv_obj_create(NULL);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Réglages Audio");
    lv_obj_center(label);

    return cont;
}

// === Page Fonction 1 ===
lv_obj_t* ui_settings_func1_create() {
    lv_obj_t* cont = lv_obj_create(NULL);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Fonction 1");
    lv_obj_center(label);

    return cont;
}

// === Page Fonction 2 ===
lv_obj_t* ui_settings_func2_create() {
    lv_obj_t* cont = lv_obj_create(NULL);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Fonction 2");
    lv_obj_center(label);

    return cont;
}

// === Page Fonction 3 ===
lv_obj_t* ui_settings_func3_create() {
    lv_obj_t* cont = lv_obj_create(NULL);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Fonction 3");
    lv_obj_center(label);

    return cont;
}
