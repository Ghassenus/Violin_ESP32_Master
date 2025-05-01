 #include "uart_manager.h"
#include "uart_parser.h" 
#include "uart_dispatcher.h" 
#include <logger.h>

#define UART_ESP2 Serial2

#define UART_RX_PIN 35
#define UART_TX_PIN 23

#define UART_BAUD 115200

static String uart_buffer;
static unsigned long last_simulation = 0; // for simulation

void uart_manager_init() {
    UART_ESP2.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
    log_info("[UART] ESP2 initialisé sur Serial2 (TX=23, RX=35)");
}

void uart_manager_loop() {
    while (UART_ESP2.available()) {
        char c = UART_ESP2.read();
        if (c == '\n') {
            uart_buffer.trim();
            if (uart_buffer.length() > 0) {
                UartMessage msg;
                if (uart_parse_message(uart_buffer, msg)) {
                    uart_dispatch_message(msg);
                } else {
                    log_error("[UART] ⚡ Erreur parsing : " + uart_buffer);
                }
            }
            uart_buffer = ""; // Réinitialiser pour le prochain message
        } else {
            uart_buffer += c;
        }
    }
}

void uart_manager_send(const String& type, const String& data) {
    String message = "{" + type + ":" + data + "}";
    UART_ESP2.println(message);
    LOGI("[UART] Envoyé : %s\n", message.c_str());
}