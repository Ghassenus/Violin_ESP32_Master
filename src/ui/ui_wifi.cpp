#include "ui_wifi.h"
#include "ui_status_bar.h"
#include "wifi_manager.h"
#include "parameters_manager.h"
#include "ui_manager.h"
#include "ui_home.h"

#include <lvgl.h>
#include <WiFi.h>
#include <Preferences.h>

static lv_obj_t* cont;
static lv_obj_t* dropdown;
static lv_obj_t* pwd_field;
static lv_obj_t* show_pwd_btn;
static lv_obj_t* connect_btn;
static lv_obj_t* spinner;
static lv_obj_t* keyboard;
// Stocker la position initiale
static lv_coord_t cont_y_initial = 0;

static void refresh_wifi_list() {
    lv_dropdown_clear_options(dropdown);
    int n = wifi_manager_get_scan_count();
    if (n == 0) {
        lv_dropdown_set_options(dropdown, "Aucun réseau trouvé");
        return;
    }

    String opts;
    for (int i = 0; i < n; ++i) {
        opts += wifi_manager_get_ssid(i);
        if (i < n - 1) opts += "\n";
    }
    lv_dropdown_set_options(dropdown, opts.c_str());
}

static void scan_wifi_cb(lv_event_t* e) {
    wifi_manager_scan_start();
    lv_obj_add_flag(dropdown, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(spinner, LV_OBJ_FLAG_HIDDEN);

    lv_timer_create([](lv_timer_t* t) {
        wifi_manager_scan_handle();

        if (!wifi_manager_scan_in_progress()) {
            lv_obj_clear_flag(dropdown, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(spinner, LV_OBJ_FLAG_HIDDEN);
            refresh_wifi_list();
            lv_timer_del(t);
        }
    }, 500, nullptr);
}

static void toggle_pwd_cb(lv_event_t* e) {
    static bool visible = false;
    visible = !visible;
    lv_textarea_set_password_mode(pwd_field, !visible);

    lv_obj_t* lbl = lv_obj_get_child(show_pwd_btn, 0);
    lv_label_set_text(lbl, visible ? LV_SYMBOL_EYE_OPEN : LV_SYMBOL_EYE_CLOSE);
}

static void on_focus(lv_event_t* e) {
    lv_keyboard_set_textarea(keyboard, pwd_field);
    lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

    // Première fois on enregistre la position d'origine
    if (cont_y_initial == 0) {
        lv_area_t coords;
        lv_obj_get_coords(cont, &coords);
        cont_y_initial = coords.y1;
    }

    // Décale vers le haut (par ex: 80 pixels)
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, -60);
}

static void keyboard_event_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_keyboard_set_textarea(keyboard, NULL);
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

        // Remet la position d'origine
        lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);
    }
}

static void connect_cb(lv_event_t* e) {
    char ssid[64];
    lv_dropdown_get_selected_str(dropdown, ssid, sizeof(ssid));
    const char* pass = lv_textarea_get_text(pwd_field);

    WiFi.begin(ssid, pass);

    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries++ < 20) {
        delay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Preferences p; p.begin("wifi", false);
        p.putString("ssid", ssid);
        p.putString("pass", pass);
        p.end();

        ui_status_bar_set_title((String)"Wi-Fi: " + WiFi.SSID());
        ui_manager::show_screen(ui_home_create());
    } else {
        wifi_manager_start_ap(); // fallback mode
        String msg = WiFi.isConnected() ? WiFi.SSID() : "mode AP";
        ui_status_bar_set_title((String)"Wi-Fi: " + msg);

        msg = "Connexion impossible. " + msg;
        lv_obj_t* alert = lv_msgbox_create(NULL, "Échec", msg.c_str(), NULL, false);
        lv_obj_center(alert);
    }
}

lv_obj_t* ui_wifi_create() {
    ui_status_bar_set_title(WiFi.isConnected() ? ("Wi-Fi: " + WiFi.SSID()).c_str() : "Wi-Fi: AP");

    cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(cont, 12, 0);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x202020), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    // --- Scan Wi-Fi ---
    lv_obj_t* scan_btn = lv_btn_create(cont);
    lv_obj_set_style_radius(scan_btn, 6, 0);
    lv_obj_set_width(scan_btn, LV_PCT(100));
    lv_obj_t* lbl_scan = lv_label_create(scan_btn);
    lv_label_set_text(lbl_scan, "Scan Wi-Fi");
    lv_obj_set_style_text_color(lbl_scan, lv_color_white(), 0);
    lv_obj_add_event_cb(scan_btn, scan_wifi_cb, LV_EVENT_CLICKED, NULL);

    // --- Spinner ---
    spinner = lv_spinner_create(cont, 1000, 60);
    lv_obj_add_flag(spinner, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_width(spinner, LV_PCT(100));

    // --- Dropdown SSID ---
    dropdown = lv_dropdown_create(cont);
    lv_obj_set_width(dropdown, LV_PCT(100));
    lv_obj_set_style_text_color(dropdown, lv_color_black(), 0);

    // --- Mot de passe avec œil ---
    lv_obj_t* pwd_container = lv_obj_create(cont);
    lv_obj_set_size(pwd_container, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(pwd_container, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(pwd_container, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag(pwd_container, LV_OBJ_FLAG_SCROLLABLE);

    pwd_field = lv_textarea_create(pwd_container);
    lv_textarea_set_one_line(pwd_field, true);
    lv_textarea_set_password_mode(pwd_field, true);
    lv_textarea_set_placeholder_text(pwd_field, "Mot de passe");
    lv_obj_set_style_text_color(pwd_field, lv_color_black(), 0);
    lv_obj_set_width(pwd_field, LV_PCT(85));
    lv_obj_set_height(pwd_field, 40);
    lv_obj_add_event_cb(pwd_field, on_focus, LV_EVENT_FOCUSED, NULL);

    show_pwd_btn = lv_btn_create(pwd_container);
    lv_obj_set_size(show_pwd_btn, 40, 40);
    lv_obj_set_style_radius(show_pwd_btn, 5, 0);
    lv_obj_t* eye_lbl = lv_label_create(show_pwd_btn);
    lv_label_set_text(eye_lbl, LV_SYMBOL_EYE_CLOSE);
    lv_obj_set_style_text_color(eye_lbl, lv_color_white(), 0);
    lv_obj_add_event_cb(show_pwd_btn, toggle_pwd_cb, LV_EVENT_CLICKED, NULL);

    // --- Bouton de connexion ---
    connect_btn = lv_btn_create(cont);
    lv_obj_set_style_radius(connect_btn, 6, 0);
    lv_obj_set_width(connect_btn, LV_PCT(100));
    lv_obj_t* lbl_conn = lv_label_create(connect_btn);
    lv_label_set_text(lbl_conn, "Se connecter");
    lv_obj_set_style_text_color(lbl_conn, lv_color_white(), 0);
    lv_obj_add_event_cb(connect_btn, connect_cb, LV_EVENT_CLICKED, NULL);

    // --- Clavier virtuel flottant ---
    keyboard = lv_keyboard_create(lv_scr_act());
    lv_obj_set_size(keyboard, LV_HOR_RES, 120);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_textarea(keyboard, NULL);
    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x303030), 0);
    lv_obj_set_style_text_color(keyboard, lv_color_white(), 0);
    lv_obj_add_event_cb(keyboard, keyboard_event_cb, LV_EVENT_ALL, NULL);

    return cont;
}
