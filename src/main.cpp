#include <Arduino.h>
#include "lvgl_helper.h"
#include "ui_manager.h"
#include "ui_home.h"
#include <api_server.h>
#include <wifi_manager.h>
#include <WiFi.h>
#include <ota_manager.h>

void setup() {
  Serial.begin(115200);

  // Initialisation des couches de base
  lvgl_setup();         // LVGL + écran + tactile
  
   // ➔ Tenter de se connecter au Wi-Fi
   wifi_manager_connect();

   // [DEBUG] Afficher infos réseau
if (WiFi.isConnected()) {
  Serial.println("✅ Connexion Wi-Fi établie !");
  Serial.print("SSID : ");
  Serial.println(WiFi.SSID());
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
} else {
  Serial.println("⚠️  Connexion Wi-Fi échouée. Démarrage en mode AP ");
}

   // ➔ Si non connecté, basculer automatiquement en AP
   if (WiFi.status() != WL_CONNECTED) {
       wifi_manager_start_ap();
       Serial.println("Connexion mode AP!");
       Serial.print("SSID : ");
       Serial.println(WiFi.SSID());
       Serial.print("Adresse IP : ");
       Serial.println(WiFi.localIP());
   }

   // ➔ Ensuite tu peux démarrer ton serveur API, OTA, etc.
   api_server_init();
   ota_manager_init(); // <=== Activer OTA

  ui_manager::init();   // Status bar + main_panel + gestionnaire UI
  

  // Afficher la première page
  ui_manager::show_screen(ui_home_create(),false);
 
}

void loop() {
  lvgl_loop(); // à appeler en permanence 
  api_server_loop();
  ota_manager_handle();
  
}
