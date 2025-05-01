// src/core/uart_dispatcher.cpp
#include "uart_dispatcher.h"
#include "logger.h"
#include <vector>

static std::vector<BtDevice> scanned_devices;

void uart_dispatch_message(const UartMessage& msg) {
    if (msg.type == "BT_DEVICE") {
        int sep = msg.data.indexOf('|');
        if (sep != -1) {
            BtDevice dev;
            dev.mac = msg.data.substring(0, sep);
            dev.name = msg.data.substring(sep + 1);
            scanned_devices.push_back(dev);
            log_info("[UART][BT] Appareil trouvé: " + dev.name + " [" + dev.mac + "]");
        } else {
            log_error("[UART][BT] Format invalide reçu pour BT_DEVICE: " + msg.data);
        }
    } 
    else if (msg.type == "SCAN_RESULT") {
        log_info("[UART] Périphérique trouvé : " + msg.data);
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
}

// ⚡️ Accès à la liste depuis l'API
const std::vector<BtDevice>& get_scanned_devices() {
    return scanned_devices;
}

// Pour reset la liste avant un nouveau scan
void clear_scanned_devices() {
    scanned_devices.clear();
}
