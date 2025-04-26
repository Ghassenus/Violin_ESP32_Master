#include "ui_settings_pages.h"
#include "parameters_manager.h"
#include <Arduino.h>

// === Pointeurs globaux pour les rollers ===
static lv_obj_t* roller_day;
static lv_obj_t* roller_month;
static lv_obj_t* roller_year;
static lv_obj_t* roller_hour;
static lv_obj_t* roller_minute;
static lv_obj_t* switch_format;

// === Callback pour sauver la date + heure + format ===
static void save_datetime_cb(lv_event_t* e) {
    int day = lv_roller_get_selected(roller_day) + 1;
    int month = lv_roller_get_selected(roller_month) + 1;
    int year = 2025 + lv_roller_get_selected(roller_year);

    int hour = lv_roller_get_selected(roller_hour);
    int minute = lv_roller_get_selected(roller_minute);
    bool fmt24 = lv_obj_has_state(switch_format, LV_STATE_CHECKED);

    parameters_manager_set_date(year, month, day);
    parameters_manager_set_hour(hour);
    parameters_manager_set_minute(minute);
    parameters_manager_set_time_format(fmt24);

    Serial.printf("[DateTime] %02d/%02d/%d %02d:%02d - format %s\n",
                  day, month, year, hour, minute, fmt24 ? "24h" : "12h");
}

// === Page Date/Heure ===
lv_obj_t* ui_settings_datetime_create(lv_obj_t* parent) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);
    lv_obj_set_style_pad_all(cont, 10, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    // ===== Ligne 1 : Date =====
    lv_obj_t* row_date = lv_obj_create(cont);
    lv_obj_set_size(row_date, LV_PCT(100), 70); // un peu plus haut
    lv_obj_set_style_pad_all(row_date, 5, 0);
    lv_obj_set_style_pad_row(row_date, 10, 0); // petit espace vertical
    lv_obj_set_style_bg_opa(row_date, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(row_date, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row_date, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(row_date, LV_SCROLLBAR_MODE_OFF);

    roller_day = lv_roller_create(row_date);
    lv_roller_set_options(roller_day,
        "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n"
        "11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n"
        "21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31",
        LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_day, 3);
    lv_obj_set_width(roller_day, LV_PCT(30));
    lv_obj_set_height(roller_day, 45);

    roller_month = lv_roller_create(row_date);
    lv_roller_set_options(roller_month,
        "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12",
        LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_month, 3);
    lv_obj_set_width(roller_month, LV_PCT(30));
    lv_obj_set_height(roller_month, 45);

    String years;
    for (int i = 0; i < 100; ++i) {
        years += String(2025 + i);
        if (i != 99) years += "\n";
    }
    roller_year = lv_roller_create(row_date);
    lv_roller_set_options(roller_year, years.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_year, 3);
    lv_obj_set_width(roller_year, LV_PCT(40));
    lv_obj_set_height(roller_year, 45);

    // ===== Ligne 2 : Heure =====
    lv_obj_t* row_time = lv_obj_create(cont);
    lv_obj_set_size(row_time, LV_PCT(100), 70);
    lv_obj_set_style_pad_all(row_time, 5, 0);
    lv_obj_set_style_pad_row(row_time, 10, 0);
    lv_obj_set_style_bg_opa(row_time, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(row_time, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row_time, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(row_time, LV_SCROLLBAR_MODE_OFF);

    roller_hour = lv_roller_create(row_time);
    lv_roller_set_options(roller_hour,
        "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23",
        LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_hour, 3);
    lv_obj_set_width(roller_hour, LV_PCT(30));
    lv_obj_set_height(roller_hour, 45);

    roller_minute = lv_roller_create(row_time);
    lv_roller_set_options(roller_minute,
        "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59",
        LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_minute, 3);
    lv_obj_set_width(roller_minute, LV_PCT(30));
    lv_obj_set_height(roller_minute, 45);

    switch_format = lv_switch_create(row_time);
    lv_obj_add_state(switch_format, parameters_manager_is_24h_format() ? LV_STATE_CHECKED : 0);
    lv_obj_set_width(switch_format, 50);

    // ===== Bouton Enregistrer =====
    lv_obj_t* btn_save = lv_btn_create(cont);
    lv_obj_set_size(btn_save, 120, 40);
    lv_obj_center(btn_save);

    lv_obj_t* lbl_save = lv_label_create(btn_save);
    lv_label_set_text(lbl_save, "Enregistrer");

    lv_obj_add_event_cb(btn_save, save_datetime_cb, LV_EVENT_CLICKED, NULL);

    // === Initialiser les rollers avec la date/heure actuelle ===
    int year_now, month_now, day_now;
    parameters_manager_get_date(year_now, month_now, day_now);

    lv_roller_set_selected(roller_day, day_now - 1, LV_ANIM_OFF);
    lv_roller_set_selected(roller_month, month_now - 1, LV_ANIM_OFF);
    lv_roller_set_selected(roller_year, year_now - 2025, LV_ANIM_OFF);

    lv_roller_set_selected(roller_hour, parameters_manager_get_hour(), LV_ANIM_OFF);
    lv_roller_set_selected(roller_minute, parameters_manager_get_minute(), LV_ANIM_OFF);

    if (parameters_manager_is_24h_format()) {
        lv_obj_add_state(switch_format, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(switch_format, LV_STATE_CHECKED);
    }

    return cont;
}

// === Page Affichage ===
lv_obj_t* ui_settings_display_create(lv_obj_t* parent) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Réglages Affichage");
    lv_obj_center(label);

    return cont;
}

// === Page Audio ===
lv_obj_t* ui_settings_audio_create(lv_obj_t* parent) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Réglages Audio");
    lv_obj_center(label);

    return cont;
}

// === Page Fonction 1 ===
lv_obj_t* ui_settings_func1_create(lv_obj_t* parent) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Fonction 1");
    lv_obj_center(label);

    return cont;
}

// === Page Fonction 2 ===
lv_obj_t* ui_settings_func2_create(lv_obj_t* parent) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Fonction 2");
    lv_obj_center(label);

    return cont;
}

// === Page Fonction 3 ===
lv_obj_t* ui_settings_func3_create(lv_obj_t* parent) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x303030), 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Fonction 3");
    lv_obj_center(label);

    return cont;
}
