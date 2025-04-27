#include "api_server.h"
#include "api_utils.h"
#include <WebServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "battery.h"
#include "parameters_manager.h"
#include "wifi_manager.h"
#include "lvgl_helper.h"
#include <logger.h>

static WebServer server(80);

void api_server_init() {
    log_info("[API] Initialisation serveur HTTP...");

    // Accueil
    server.on("/", HTTP_GET, []() {
        api_send_cors(server);
        server.send(200, "text/plain", "Hello World");
    });

    // GET /api/status
    server.on("/api/status", HTTP_GET, []() {
        StaticJsonDocument<512> doc;
        doc["wifi"]   = WiFi.isConnected();
        doc["ssid"]   = WiFi.SSID();
        doc["ip"]     = WiFi.localIP().toString();
        doc["rssi"]   = WiFi.RSSI();
        doc["batt"]   = battery_get_percent();
        doc["batt_voltage"] = battery_get_voltage_mV();
        doc["fmt24h"] = parameters_manager_is_24h_format();
        doc["brightness"] = screen_get_brightness_percent(); 

        time_t now = time(nullptr);
        struct tm tm;
        localtime_r(&now, &tm);

        doc["hour"] = tm.tm_hour;
        doc["minute"] = tm.tm_min;
        doc["year"] = tm.tm_year + 1900;
        doc["month"] = tm.tm_mon + 1;
        doc["day"] = tm.tm_mday;

        doc["heap_free"] = ESP.getFreeHeap();
        doc["heap_total"] = ESP.getHeapSize();
        size_t sketch_total = ESP.getSketchSize() + ESP.getFreeSketchSpace();
        doc["flash_used"] = ESP.getSketchSize();
        doc["flash_total"] = sketch_total;
        doc["ram_usage"] = (ESP.getHeapSize() - ESP.getFreeHeap()) * 100 / ESP.getHeapSize();
        doc["flash_usage"] = (ESP.getSketchSize() * 100) / sketch_total;

        String out;
        serializeJson(doc, out);
        api_send_json(server, 200, out);
    });
    api_handle_options(server, "/api/status");

    // GET /api/wifi/scan
    server.on("/api/wifi/scan", HTTP_GET, []() {
        int n = WiFi.scanNetworks();
        StaticJsonDocument<1024> doc;
        JsonArray arr = doc.createNestedArray("networks");

        for (int i = 0; i < n; i++) {
            JsonObject net = arr.createNestedObject();
            net["ssid"] = WiFi.SSID(i);
            net["rssi"] = WiFi.RSSI(i);
            net["enc"] = WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "secured";
        }
        WiFi.scanDelete(); // Libère la mémoire

        String out;
        serializeJson(doc, out);
        api_send_json(server, 200, out);
    });
    api_handle_options(server, "/api/wifi/scan");

    // POST /api/wifi/connect
    server.on("/api/wifi/connect", HTTP_POST, []() {
        StaticJsonDocument<200> doc;
        DeserializationError err = deserializeJson(doc, server.arg("plain"));
        if (err || !doc["ssid"] || !doc["pass"]) {
            api_send_error(server, "ssid & pass required");
            return;
        }
        Preferences p; p.begin("wifi", false);
        p.putString("ssid", doc["ssid"].as<const char*>());
        p.putString("pass", doc["pass"].as<const char*>());
        p.end();

        wifi_manager_connect();
        api_send_ok(server);
    });
    api_handle_options(server, "/api/wifi/connect");

    // POST /api/params/time
    server.on("/api/params/time", HTTP_POST, []() {
        StaticJsonDocument<128> doc;
        DeserializationError err = deserializeJson(doc, server.arg("plain"));
        if (err || !doc["hour"] || !doc["minute"]) {
            api_send_error(server, "hour & minute required");
            return;
        }
        parameters_manager_set_hour(doc["hour"]);
        parameters_manager_set_minute(doc["minute"]);
        api_send_ok(server);
    });
    api_handle_options(server, "/api/params/time");

    // POST /api/params/date
    server.on("/api/params/date", HTTP_POST, []() {
        StaticJsonDocument<128> doc;
        DeserializationError err = deserializeJson(doc, server.arg("plain"));
        if (err || !doc["year"] || !doc["month"] || !doc["day"]) {
            api_send_error(server, "year, month & day required");
            return;
        }
        parameters_manager_set_date(doc["year"], doc["month"], doc["day"]);
        api_send_ok(server);
    });
    api_handle_options(server, "/api/params/date");

    // POST /api/params/format
    server.on("/api/params/format", HTTP_POST, []() {
        StaticJsonDocument<64> doc;
        DeserializationError err = deserializeJson(doc, server.arg("plain"));
        if (err || !doc["format24"].is<bool>()) {
            api_send_error(server, "format24 required");
            return;
        }
        parameters_manager_set_time_format(doc["format24"]);
        api_send_ok(server);
    });
    api_handle_options(server, "/api/params/format");

    // POST /api/display/brightness
    server.on("/api/display/brightness", HTTP_POST, []() {
        StaticJsonDocument<64> doc;
        DeserializationError err = deserializeJson(doc, server.arg("plain"));
        if (err || !doc["brightness"].is<int>()) {
            api_send_error(server, "brightness (0-100) required");
            return;
        }
        screen_set_brightness_percent(doc["brightness"]);
        api_send_ok(server);
    });
    api_handle_options(server, "/api/display/brightness");

    // POST /api/system/reboot
    server.on("/api/system/reboot", HTTP_POST, []() {
        api_send_json(server, 200, "{\"result\":\"rebooting\"}");
        delay(100);
        ESP.restart();
    });
    api_handle_options(server, "/api/system/reboot");

    // 404
    server.onNotFound([]() {
        api_send_cors(server);
        server.send(404, "text/plain", "Not found");
    });

    server.begin();
}

void api_server_loop() {
    server.handleClient();
}