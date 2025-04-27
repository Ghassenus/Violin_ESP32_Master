 #include "uart_manager.h"
#include "uart_parser.h" 
#include <logger.h>

#define UART_ESP2 Serial2

#define UART_RX_PIN 35
#define UART_TX_PIN 26

#define UART_BAUD 115200

static String uart_buffer;
static unsigned long last_simulation = 0; // for simulation

void uart_manager_init() {
    UART_ESP2.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
    log_info("[UART] ESP2 initialisé sur Serial2 (TX=26, RX=35)");
}

void uart_manager_loop() {
    while (UART_ESP2.available()) {
        char c = UART_ESP2.read();
        if (c == '}') {
            uart_buffer += c;
            log_info("[UART] Reçu : ");
            log_info(uart_buffer);

            UartMessage msg;
            if (uart_parse_message(uart_buffer, msg)) {
                LOGI("[UART] Type: %s | Data: %s\n", msg.type.c_str(), msg.data.c_str());

                // Dispatcher ici selon le type
                if (msg.type == "LIST") {
                    log_info("[UART] => Afficher liste BT !"); // simulation
                    // TODO: Afficher liste BT
                }
                else if (msg.type == "OK") {
                    log_info("[UART] => Succès !"); //Simulation
                    // TODO: Afficher succès
                }
                else if (msg.type == "FAIL") {
                    // TODO: Afficher erreur
                    log_error("[UART] => Erreur !"); // Simulation
                }
            }
            uart_buffer = "";
        }
        else {
            uart_buffer += c;
        }        
    }

    // === Simulateur toutes les 10 secondes ===
    if (millis() - last_simulation > 10000) {
        uart_buffer = "{LIST:ESP_Speaker_1,ESP_Speaker_2}";
        last_simulation = millis();
    }
}

void uart_manager_send(const String& type, const String& data) {
    String message = "{" + type + ":" + data + "}";
    UART_ESP2.println(message);
    LOGI("[UART] Envoyé : %s\n", message.c_str());
}