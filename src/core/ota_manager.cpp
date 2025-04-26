// src/core/ota_manager.cpp

#include "ota_manager.h"
#include <ArduinoOTA.h>
#include "ui_status_bar.h" // pour notifier le header
#include "ui_ota_popup.h" // afficher un popup de mise à jour

// Drapeau interne indiquant si une OTA est en cours
static bool _ota_running = false;

#define OTA_PASSWORD "OtaViolin2025"

void ota_manager_init() {
    // Nom du device sur le réseau
    ArduinoOTA.setHostname("esp32-violin");

    // Authentification sécurisée via hash SHA1
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA
        .onStart([]() {
            Serial.println("[OTA] Démarrage");
            _ota_running = true;
            ui_status_bar_notify_ota(true); // Affiche l'icône OTA
            ui_ota_popup_create(); // <<< ouvrir le popup OTA
            lv_timer_handler(); // forcer rafraichissement
        })
        .onEnd([]() {
            Serial.println("[OTA] Terminé");
            _ota_running = false;
            ui_status_bar_notify_ota(false); // Cache l'icône OTA
            ui_ota_popup_close(); // <<< fermer proprement
            lv_timer_handler(); // <<< Ajouté ici pour forcer rafraichissement
        })
        .onProgress([](unsigned int progress, unsigned int total) {
          uint8_t percent = (progress * 100) / total;
            Serial.printf("[OTA] Progression: %u%%\r", percent);
            ui_ota_popup_update_progress(percent); // <<< mettre à jour la barre
            lv_timer_handler(); // <<< Ajouté ici pour forcer rafraichissement
        })
        .onError([](ota_error_t error) {
            Serial.printf("[OTA] Erreur [%u]: ", error);
            switch (error) {
                case OTA_AUTH_ERROR:    Serial.println("Échec authentification"); break;
                case OTA_BEGIN_ERROR:   Serial.println("Échec begin"); break;
                case OTA_CONNECT_ERROR: Serial.println("Échec connexion"); break;
                case OTA_RECEIVE_ERROR: Serial.println("Échec réception"); break;
                case OTA_END_ERROR:     Serial.println("Échec end"); break;
                default:                Serial.println("Erreur inconnue");
            }
            _ota_running = false;
            ui_status_bar_notify_ota(false);
            ui_ota_popup_close(); // <<< fermer en cas d'erreur aussi
            lv_timer_handler(); // <<< Ajouté ici pour forcer rafraichissement
        });

    ArduinoOTA.begin();
    Serial.println("[OTA] Prêt à recevoir une mise à jour OTA !");
}

void ota_manager_handle() {
    ArduinoOTA.handle();
}

bool ota_manager_is_running() {
    return _ota_running;
}
