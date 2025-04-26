// src/ui/ui_ota_popup.cpp

#include <lvgl.h>
#include <Arduino.h>

static lv_obj_t* popup = nullptr;
static lv_obj_t* bar = nullptr;
static lv_obj_t* label_percent = nullptr;

// === Créer le popup OTA ===
void ui_ota_popup_create() {
    if (popup) return; // déjà créé

    popup = lv_obj_create(lv_scr_act()); // parent = écran actif
    lv_obj_set_size(popup, 200, 120);
    lv_obj_center(popup);
    lv_obj_set_style_radius(popup, 10, 0);
    lv_obj_set_style_bg_color(popup, lv_color_hex(0x202020), 0);
    lv_obj_set_style_bg_opa(popup, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(popup, 2, 0);
    lv_obj_set_style_border_color(popup, lv_color_hex(0x0077FF), 0);

    lv_obj_t* label = lv_label_create(popup);
    lv_label_set_text(label, "Mise à jour OTA...");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);

    bar = lv_bar_create(popup);
    lv_obj_set_size(bar, 160, 20);
    lv_obj_align(bar, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_range(bar, 0, 100);
    lv_bar_set_value(bar, 0, LV_ANIM_OFF);

    label_percent = lv_label_create(popup);
    lv_label_set_text(label_percent, "0%");
    lv_obj_align(label_percent, LV_ALIGN_BOTTOM_MID, 0, -10);
}

// === Mettre à jour la progression ===
void ui_ota_popup_update_progress(uint8_t percent) {
    if (!popup) return;
    lv_bar_set_value(bar, percent, LV_ANIM_OFF);

    char buf[8];
    snprintf(buf, sizeof(buf), "%u%%", percent);
    lv_label_set_text(label_percent, buf);
}

// === Fermer le popup ===
void ui_ota_popup_close() {
    if (popup) {
        lv_obj_del(popup);
        popup = nullptr;
        bar = nullptr;
        label_percent = nullptr;
    }
}
