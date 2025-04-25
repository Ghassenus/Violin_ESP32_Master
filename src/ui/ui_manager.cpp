#include "ui_manager.h"
#include "ui_status_bar.h"
#include "ui_home.h" // Nécessaire pour show_screen vers accueil

static lv_obj_t *status_bar;
static lv_obj_t *main_panel;

void ui_manager::init() {
    lv_obj_t *root = lv_scr_act();

    // Status bar via module
    status_bar = ui_status_bar_create();
    //lv_obj_set_parent(status_bar, root);
    //lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);

    // Zone principale
    main_panel = lv_obj_create(root);
    lv_obj_set_size(main_panel, LV_PCT(100), LV_PCT(90));
    lv_obj_align(main_panel, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_scrollbar_mode(main_panel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(main_panel, 0, 0);
    lv_obj_set_style_bg_color(main_panel, lv_color_hex(0x000000), 0);
}

void ui_manager::show_screen(lv_obj_t *screen, bool show_home_btn) {
    lv_obj_clean(main_panel);
    lv_obj_set_parent(screen, main_panel);
    lv_obj_center(screen);

    if (show_home_btn) {
        lv_obj_t *btn_home = lv_btn_create(main_panel);
        lv_obj_set_size(btn_home, 36, 36); // légèrement plus petit
        lv_obj_align(btn_home, LV_ALIGN_TOP_RIGHT, -8, 8); // mieux positionné

        // Style plus discret
        lv_obj_set_style_bg_color(btn_home, lv_color_hex(0x333333), LV_PART_MAIN); // gris foncé
        lv_obj_set_style_bg_opa(btn_home, LV_OPA_70, LV_PART_MAIN);                // semi-transparent
        lv_obj_set_style_radius(btn_home, LV_RADIUS_CIRCLE, LV_PART_MAIN);         // arrondi cercle
        lv_obj_set_style_border_width(btn_home, 0, LV_PART_MAIN);                  // sans bordure

        lv_obj_t *lbl = lv_label_create(btn_home);
        lv_label_set_text(lbl, LV_SYMBOL_HOME);
        lv_obj_center(lbl); // centre bien l'icône dans le bouton

        lv_obj_add_event_cb(btn_home, [](lv_event_t * e) {
            ui_manager::show_screen(ui_home_create(), false); // retour sans bouton
        }, LV_EVENT_CLICKED, NULL);
    }
}

