// src/core/uart_dispatcher.cpp
#include "uart_dispatcher.h"
#include "logger.h"
#include <vector>
#include <api_server.h>

static std::vector<BtDevice> scanned_devices;

void uart_dispatch_message(const UartMessage& msg) {
    if (msg.type == "SCAN_RESULT") {
        int sep = msg.data.indexOf('|');
        if (sep != -1) {
            BtDevice dev;
            dev.mac = msg.data.substring(0, sep);
            dev.name = msg.data.substring(sep + 1);
            scanned_devices.push_back(dev);
            log_info("[UART][BT] Appareil trouvé: " + dev.name + " [" + dev.mac + "]");
    
        }
    } 
    else if (msg.type == "CONNECT_OK") {
        log_info("[UART] Connexion réussie au périphérique !");
    }
    else if (msg.type == "PIN_REQUIRED") {
        log_warn("[UART] Code PIN requis pour appairage !");
    }
    else if (msg.type == "SCAN_END") {
        log_info("[UART] Fin du scan Bluetooth");
    }
    else if (msg.type == "ERROR") {
        log_error("[UART] Erreur reçue : " + msg.data);
    }
    else {
        log_error("[UART] Message UART inconnu : " + msg.type + " / Data: " + msg.data);
    }

      // <--- Relais global à l'API pour WebSocket et autres
      on_uart_message(msg);  // Appelle la fonction API ici
}

// ⚡️ Accès à la liste depuis l'API
const std::vector<BtDevice>& uart_get_scanned_devices() {
    return scanned_devices;
}

// Pour reset la liste avant un nouveau scan
void uart_clear_scanned_devices() {
    scanned_devices.clear();
}
