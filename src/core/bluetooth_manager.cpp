#include "bluetooth_manager.h"
#include "uart_manager.h"
#include "logger.h"

namespace bluetooth_manager {

void init() {
    // Rien à faire pour l'instant
}

void loop() {
    // Rien à faire pour l'instant
}

void scan() {
    log_info("[BT] Demande de scan envoyée à ESP2");
    uart_manager_send("BT_SCAN", "");
}

void connect(const String& mac_address) {
    log_info("[BT] Demande de connexion à " + mac_address);
    uart_manager_send("BT_CONNECT", mac_address);
}

}
