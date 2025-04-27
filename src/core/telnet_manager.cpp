#include "telnet_manager.h"
#include <WiFi.h>
#include <logger.h>

// Serveur Telnet sur port 23
static WiFiServer telnetServer(23);
static WiFiClient telnetClient;

void telnet_manager_init() {
    telnetServer.begin();
    telnetServer.setNoDelay(true);
    log_info("[Telnet] Serveur Telnet démarré sur port 23");
}

void telnet_manager_handle() {
    if (telnetServer.hasClient()) {
        if (!telnetClient || !telnetClient.connected()) {
            if (telnetClient) telnetClient.stop();
            telnetClient = telnetServer.available();
            Serial.println("[Telnet] Client connecté !");
            telnet_manager_println("[Telnet] Bienvenue sur ESP32 Violin !");
        } else {
            WiFiClient newClient = telnetServer.available();
            newClient.stop(); // Refuser clients multiples
        }
    }
}

void telnet_manager_print(const String& msg) {
    if (telnetClient && telnetClient.connected()) {
        telnetClient.print(msg);
    }
}

void telnet_manager_println(const String& msg) {
    if (telnetClient && telnetClient.connected()) {
        telnetClient.println(msg);
    }
}
