// src/core/uart_dispatcher.cpp
#include "uart_dispatcher.h"
#include "logger.h"
#include <vector>
#include <api_server.h>
#include <ws_server.h>

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
            ws_broadcast("bt_scan_result", msg.data); // Envoi en temps réel au HTML via WebSocket
    
        }
    }
    else if (msg.type == "BT_SAVED") {
        // Appareil mémorisé dans l'historique
        log_info("[UART][BT] Appareil mémorisé : " + msg.data);
        ws_broadcast("bt_device_known", msg.data);  // ← vers WebSocket
    }
    
    else if (msg.type == "BT_CONNECT_OK") {
        log_info("[UART] Connexion réussie au périphérique !");
        ws_broadcast("bt_device_connected", msg.data);
    }
    else if (msg.type == "PIN_REQUIRED") {
        log_warn("[UART] Code PIN requis pour appairage !");
    }
    else if (msg.type == "BT_SCAN_END") {
        log_info("[UART] Fin du scan Bluetooth");
        ws_broadcast("bt_scan_end", msg.data);
    }
    else if (msg.type == "ERROR") {
        log_error("[UART] Erreur reçue : " + msg.data);
        ws_broadcast("error", msg.data);
    }
    else if (msg.type == "BT_SCAN_START") {
        log_info("[UART] Debut du scan Bluetooth : " + msg.data);
        ws_broadcast("bt_scan_start", msg.data);
    }
    else if (msg.type == "BT_PAIR_OK") {
        log_info("[UART] Appairage OK : " + msg.data);
        ws_broadcast("bt_paired", msg.data);
    }
    else if (msg.type == "BT_PAIR_KO") {
        log_error("[UART] Appairage KO : " + msg.data);
        ws_broadcast("bt_paired_ko", msg.data);
    }
    else if (msg.type == "BT_ALL_REMOVED") {
        log_info("[UART] Historique péripherique Bluetoth effacé : " + msg.data);
        ws_broadcast("bt_history_cleared", msg.data);
    }
    else if (msg.type == "BT_REMOVED") {
        log_info("[UART] péripherique Bluetoth supprimé : " + msg.data);
        ws_broadcast("bt_device_removed", msg.data);
    }
    else if (msg.type == "BT_REMOVED_KO") {
        log_error("[UART]  suppression du péripherique Bluetoth échoué : " + msg.data);
        ws_broadcast("bt_device_not_removed", msg.data);
    }      

    else {
        log_error("[UART] Message UART inconnu : " + msg.type + " / Data: " + msg.data);
        ws_broadcast("uart_message_unkown", msg.data);

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
