#include "ws_server.h"
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

static WebSocketsServer webSocket = WebSocketsServer(81);

void ws_server_init() {
    webSocket.begin();
    webSocket.onEvent([](uint8_t client_num, WStype_t type, uint8_t * payload, size_t length) {
        if (type == WStype_CONNECTED) {
            Serial.printf("[WS] Client %u connecté\n", client_num);
        } else if (type == WStype_DISCONNECTED) {
            Serial.printf("[WS] Client %u déconnecté\n", client_num);
        } else if (type == WStype_TEXT) {
            Serial.printf("[WS] Reçu : %s\n", payload);
        }
    });

    Serial.println("[WS] WebSocket serveur lancé sur le port 81");
}

void ws_server_loop() {
    webSocket.loop();
}

void ws_broadcast(const String& type, const String& payload) {
    StaticJsonDocument<256> doc;
    doc["type"] = type;
    doc["data"] = payload;

    String json;
    serializeJson(doc, json);

    webSocket.broadcastTXT(json);
}