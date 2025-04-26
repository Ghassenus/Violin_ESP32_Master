// src/ui/ui_settings.cpp
#include "ui_settings.h"
#include "ui_settings_pages.h"
#include "ui_manager.h"
#include <ui_status_bar.h>
#include <lvgl.h>

static lv_obj_t* panel_left;
static lv_obj_t* panel_right;
static lv_obj_t* current_selected_btn = nullptr;

// Fonction utilitaire pour charger dynamiquement une page
static void load_screen(lv_obj_t* (*create_func)(lv_obj_t*)) {
    lv_obj_clean(panel_right);
    lv_obj_t* screen = create_func(panel_right);
    lv_obj_center(screen);
}

// === Gestion du clic sur un bouton du menu ===
static void menu_btn_event_cb(lv_event_t* e) {
    lv_obj_t* btn = lv_event_get_target(e);
    auto func = (lv_obj_t* (*)(lv_obj_t*)) lv_event_get_user_data(e);

    if (func) load_screen(func);

    // Reset tous les boutons
    uint32_t count = lv_obj_get_child_cnt(panel_left);
    for (uint32_t i = 0; i < count; ++i) {
        lv_obj_t* child = lv_obj_get_child(panel_left, i);
        lv_obj_set_style_bg_color(child, lv_color_hex(0x202020), 0); // couleur normale
    }

    // Activer la surbrillance sur le bouton cliqué
    lv_obj_set_style_bg_color(btn, lv_palette_lighten(LV_PALETTE_BLUE, 2), 0);

    current_selected_btn = btn;
}

lv_obj_t* ui_settings_create() {
    ui_status_bar_set_title("Reglages");

    lv_obj_t* root = lv_obj_create(lv_scr_act());
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_style_bg_color(root, lv_color_hex(0x101010), 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_ROW);

    // Panel gauche (menu)
    panel_left = lv_obj_create(root);
    lv_obj_set_size(panel_left, LV_PCT(30), LV_PCT(100));
    lv_obj_set_style_pad_all(panel_left, 5, 0);
    lv_obj_set_style_bg_color(panel_left, lv_color_hex(0x202020), 0);
    lv_obj_set_flex_flow(panel_left, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(panel_left, LV_SCROLLBAR_MODE_OFF);

    // Panel droit (contenu)
    panel_right = lv_obj_create(root);
    lv_obj_set_size(panel_right, LV_PCT(70), LV_PCT(100));
    lv_obj_set_style_pad_all(panel_right, 5, 0);
    lv_obj_set_style_bg_color(panel_right, lv_color_hex(0x303030), 0);
    lv_obj_set_scrollbar_mode(panel_right, LV_SCROLLBAR_MODE_OFF);

    // === Liste des options ===
    static const struct {
        const char* name;
        lv_obj_t* (*create_func)(lv_obj_t*);
    } options[] = {
        {"Date & Heure", ui_settings_datetime_create},
        {"Affichage",    ui_settings_display_create},
        {"Audio",        ui_settings_audio_create},
        {"Infos",        ui_settings_inf_system_create},
        {"Fonction 2",   ui_settings_func2_create},
        {"Fonction 3",   ui_settings_func3_create},
    };

    for (uint8_t i = 0; i < sizeof(options) / sizeof(options[0]); ++i) {
        lv_obj_t* btn = lv_btn_create(panel_left);
        lv_obj_set_size(btn, LV_PCT(100), 40);
        lv_obj_set_style_radius(btn, 5, 0);

        lv_obj_t* lbl = lv_label_create(btn);
        lv_label_set_text(lbl, options[i].name);
        lv_obj_center(lbl);

        lv_obj_add_event_cb(btn, menu_btn_event_cb, LV_EVENT_CLICKED, (void*)options[i].create_func);
    }

    // Charger une première page par défaut
    load_screen(ui_settings_datetime_create);

    // ➔ Et mettre la surbrillance sur le premier bouton directement
    if (lv_obj_get_child_cnt(panel_left) > 0) {
        current_selected_btn = lv_obj_get_child(panel_left, 0);
        lv_obj_set_style_bg_color(current_selected_btn, lv_palette_lighten(LV_PALETTE_BLUE, 2), 0);
    }

    return root;
}