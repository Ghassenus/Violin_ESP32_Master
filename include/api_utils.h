#pragma once
#include <WebServer.h>

// Injecte en-têtes CORS dans la réponse
inline void api_send_cors(WebServer& server) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// Répondre proprement JSON + CORS
inline void api_send_json(WebServer& server, int code, const String& json) {
    api_send_cors(server);
    server.send(code, "application/json", json);
}

// Répondre OK simple
inline void api_send_ok(WebServer& server) {
    api_send_cors(server);
    server.send(200, "application/json", "{\"result\":\"ok\"}");
}

// Répondre erreur simple
inline void api_send_error(WebServer& server, const char* msg) {
    api_send_cors(server);
    server.send(400, "application/json", String("{\"error\":\"") + msg + "\"}");
}

// Gérer une route OPTIONS automatiquement
inline void api_handle_options(WebServer& server, const char* path) {
    server.on(path, HTTP_OPTIONS, [&server]() {
        api_send_cors(server);
        server.send(204); // 204 = No Content
    });
}
