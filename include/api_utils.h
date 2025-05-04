#pragma once
#include <WebServer.h>

// Injecte les en-têtes CORS (à appeler AVANT server.send)
inline void api_send_cors(WebServer& server) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// Répondre avec JSON + CORS (utilisé par /status et autres)
inline void api_send_json(WebServer& server, int code, const String& json) {
    api_send_cors(server);
    server.send(code, "application/json", json);
}

// Répondre avec un simple OK JSON
inline void api_send_ok(WebServer& server) {
    api_send_cors(server);
    server.send(200, "application/json", "{\"result\":\"ok\"}");
}

// Répondre avec une erreur JSON
inline void api_send_error(WebServer& server, const char* msg) {
    api_send_cors(server);
    server.send(400, "application/json", String("{\"error\":\"") + msg + "\"}");
}

// Gère les pré-requêtes OPTIONS (automatique pour le CORS)
inline void api_handle_options(WebServer& server, const char* path) {
    server.on(path, HTTP_OPTIONS, [&server]() {
        api_send_cors(server);
        server.send(204); // No Content
    });
}
