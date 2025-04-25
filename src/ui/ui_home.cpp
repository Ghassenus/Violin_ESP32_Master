#include "ui_home.h"
#include "ui_manager.h"
#include "ui_settings.h"
#include "ui_wifi.h"
#include "ui_status_bar.h"

#include <lvgl.h>
#include <WiFi.h>

static lv_obj_t* create_tile(lv_obj_t* parent, const char* icon, const char* label, lv_color_t color, lv_event_cb_t cb) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 100, 100);
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_set_style_pad_all(btn, 8, 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* icon_lbl = lv_label_create(btn);
    lv_label_set_text(icon_lbl, icon);
    lv_obj_set_style_text_font(icon_lbl, &lv_font_montserrat_28, 0);
    lv_obj_align(icon_lbl, LV_ALIGN_TOP_MID, 0, 4);

    lv_obj_t* text_lbl = lv_label_create(btn);
    lv_label_set_text(text_lbl, label);
    lv_obj_align(text_lbl, LV_ALIGN_BOTTOM_MID, 0, -4);

    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);

    return btn;
}

lv_obj_t* ui_home_create() {
    // Conteneur principal
    lv_obj_t *container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(container, lv_color_hex(0x101010), 0);
    lv_obj_set_style_bg_opa(container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_layout(container, LV_LAYOUT_GRID);

    // Grille 2x2
    static lv_coord_t col_dsc[] = {LV_PCT(50), LV_PCT(50), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_PCT(50), LV_PCT(50), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(container, col_dsc, row_dsc);

    // Reset titre
    ui_status_bar_set_title("");

    // === Boutons ===

    // Wi-Fi
    lv_obj_t* btn_wifi = create_tile(container, LV_SYMBOL_WIFI, "Wi-Fi", lv_palette_main(LV_PALETTE_BLUE), [](lv_event_t* e) {
        ui_manager::show_screen(ui_wifi_create());
    });
    lv_obj_set_grid_cell(btn_wifi, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    // Réglages
    lv_obj_t* btn_settings = create_tile(container, LV_SYMBOL_SETTINGS, "Settings", lv_palette_main(LV_PALETTE_ORANGE), [](lv_event_t* e) {
        ui_manager::show_screen(ui_settings_create());
    });
    lv_obj_set_grid_cell(btn_settings, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    // Redémarrer
    lv_obj_t* btn_restart = create_tile(container, LV_SYMBOL_REFRESH, "Redemarrer", lv_palette_main(LV_PALETTE_RED), [](lv_event_t* e) {
        ESP.restart();
    });
    lv_obj_set_grid_cell(btn_restart, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    // Bluetooth (placeholder)
    lv_obj_t* btn_bt = create_tile(container, LV_SYMBOL_BLUETOOTH, "Bluetooth", lv_palette_main(LV_PALETTE_CYAN), [](lv_event_t* e) {
        // TODO: écran Bluetooth
    });
    lv_obj_set_grid_cell(btn_bt, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    return container;
}
