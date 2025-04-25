#pragma once
#include <Arduino.h>
#include <lvgl.h>

// Crée et retourne la status bar (à placer en haut)
lv_obj_t* ui_status_bar_create();

void ui_status_bar_set_battery(int level_percent); // Afficher statut batterie
void ui_status_bar_set_wifi(bool connected); // Afficher Statut WIFI
void ui_status_bar_update_time(); // Afficher et mettre à jour el temps
void ui_status_bar_notify_ota(bool running); // Afficher si une OTA est en cours
void ui_status_bar_set_title(const char* txt);// updater le teitre du status bar ( texte au milieu)
void ui_status_bar_set_title(const String& txt); // surcharge pour les strings