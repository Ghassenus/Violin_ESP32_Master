#include "ui_settings.h"
#include "ui_settings_pages.h"
#include "ui_manager.h"
#include <lvgl.h>
#include <ui_status_bar.h>

static lv_obj_t* panel_left;
static lv_obj_t* panel_right;

// Fonction utilitaire pour charger dynamiquement une page
static void load_screen(lv_obj_t* (*create_func)()) {
    lv_obj_clean(panel_right);
    lv_obj_t* screen = create_func();
    lv_obj_set_parent(screen, panel_right);
    lv_obj_center(screen);
}

lv_obj_t* ui_settings_create() {
    ui_status_bar_set_title("Settings");

    lv_obj_t* root = lv_obj_create(lv_scr_act());
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_style_bg_color(root, lv_color_hex(0x101010), 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_ROW); // Flex layout horizontal

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

    // Liste des options
    static const struct {
        const char* name;
        lv_obj_t* (*create_func)();
    } options[] = {
        {"Date & Heure", ui_settings_datetime_create},
        {"Affichage",    ui_settings_display_create},
        {"Audio",        ui_settings_audio_create},
        {"Fonction 1",   ui_settings_func1_create},
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

        lv_obj_add_event_cb(btn, [](lv_event_t* e){
            auto btn = lv_event_get_target(e);
            auto func = (lv_obj_t* (*)())lv_obj_get_user_data(btn);
            if (func) load_screen(func);
        }, LV_EVENT_CLICKED, (void*)options[i].create_func);
    }

    // Affiche la première page par défaut
    load_screen(ui_settings_datetime_create);

    return root;
}
