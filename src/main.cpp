#include <Arduino.h>
#include "lvgl_helper.h"
#include "ui_manager.h"
#include "ui_home.h"
#include <api_server.h>
#include <wifi_manager.h>
#include <WiFi.h>
#include <ota_manager.h>
#include "uart_manager.h"
#include "telnet_manager.h"
#include <esp_adc_cal.h>
#include <battery.h>
#include <logger.h>
#include <ws_server.h>

// === Batterie charge debug ===
static uint32_t last_batt_debug = 0;
static bool last_charging_status = false;
unsigned long last_ping = 0; // test uart
const unsigned long ping_interval = 5000; // 5 secondes test uart


void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println(F("[Boot] Démarrage..."));
  

  // Initialisation des couches de base
  lvgl_setup();         // LVGL + écran + tactile
  
   // ➔ Tenter de se connecter au Wi-Fi
   wifi_manager_connect();

   // ➔ Si non connecté, basculer automatiquement en AP
   if (WiFi.status() != WL_CONNECTED) {
       wifi_manager_start_ap();
       Serial.println("Connexion mode AP!");
       Serial.print("SSID : ");
       Serial.println(WiFi.SSID());
       Serial.print("Adresse IP : ");
       Serial.println(WiFi.localIP());
   }
   // ➔ Maintenant que WiFi est OK : ON PEUT démarrer Telnet
   telnet_manager_init();

   // ➔ Ensuite tu peux démarrer ton serveur API, OTA, etc.
   api_server_init(); // initialiser le serveur API
   ota_manager_init(); // <=== Activer OTA
   ws_server_init();
   uart_manager_init(); // Activer communication UART
  ui_manager::init();   // Status bar + main_panel + gestionnaire UI
 
  

  // Afficher la première page
  ui_manager::show_screen(ui_home_create(),false);

 
}

void loop() {
  lvgl_loop(); // à appeler en permanence 
  api_server_loop();
  ws_server_loop();
  ota_manager_handle();
  uart_manager_loop();
  telnet_manager_handle();
  screen_brightness_loop();
  
 }
