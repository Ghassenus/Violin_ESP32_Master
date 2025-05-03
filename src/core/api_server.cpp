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
#include <bluetooth_manager.h>
#include <uart_manager.h>
#include "ws_server.h"

static WebServer server(80);

void api_server_init() {
    log_info("[API] Initialisation serveur HTTP...");

    register_handler("/", HTTP_GET, []() {
        server.send(200, "text/plain", "Hello World");
    });

    register_handler("/api/status", HTTP_GET, []() {
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

    // Routes Bluetooth
    register_handler("/api/bluetooth/scan", HTTP_POST, []() {
        uart_clear_scanned_devices();
        uart_manager_send("BT_SCAN", "");
        api_send_ok(server);
    });

    register_handler("/api/bluetooth/devices", HTTP_GET, []() {
        StaticJsonDocument<1024> doc;
        JsonArray devices = doc.createNestedArray("devices");
        for (const BtDevice& dev : uart_get_scanned_devices()) {
            JsonObject obj = devices.createNestedObject();
            obj["mac"] = dev.mac;
            obj["name"] = dev.name;
        }
        String out;
        serializeJson(doc, out);
        server.send(200, "application/json", out);
    });

    register_handler("/api/bluetooth/connect", HTTP_POST, []() {
        StaticJsonDocument<200> doc;
        DeserializationError err = deserializeJson(doc, server.arg("plain"));
        if (err || !doc["mac"]) {
            api_send_error(server, "mac address required");
            return;
        }
        bluetooth_manager::connect(doc["mac"]);
        api_send_ok(server);
    });

    // Commandes audio (UART vers ESP2)
    register_handler("/api/bluetooth/play", HTTP_POST, []() {
        uart_manager_send("BT_PLAY", "");
        api_send_ok(server);
    });

    register_handler("/api/bluetooth/stop", HTTP_POST, []() {
        uart_manager_send("BT_STOP", "");
        api_send_ok(server);
    });

    register_handler("/api/bluetooth/forward", HTTP_POST, []() {
        uart_manager_send("BT_FORWARD", "");
        api_send_ok(server);
    });

    register_handler("/api/bluetooth/backward", HTTP_POST, []() {
        uart_manager_send("BT_BACKWARD", "");
        api_send_ok(server);
    });

    register_handler("/api/bluetooth/volup", HTTP_POST, []() {
        uart_manager_send("BT_VOL_UP", "");
        api_send_ok(server);
    });

    register_handler("/api/bluetooth/voldown", HTTP_POST, []() {
        uart_manager_send("BT_VOL_DOWN", "");
        api_send_ok(server);
    });

    // 404
    server.onNotFound([]() {
        api_send_cors(server);
        server.send(404, "text/plain", "Not found");
    });

    server.begin();
}

void on_uart_message(const UartMessage& msg) {
    if (msg.type == "ERROR") {
        log_error("[API/UART] Erreur UART : " + msg.data);
        ws_broadcast("uart_error", msg.data);
    } else if (msg.type == "CONNECT_OK") {
        ws_broadcast("bt_status", "connect_ok");
    } else if (msg.type == "DISCONNECTED") {
        ws_broadcast("bt_status", "disconnected");
    } else {
        log_warn("[API/UART] Message ignoré : " + msg.type + " / " + msg.data);
    }
}

void api_server_loop() {
    server.handleClient();
}

void register_handler(const String& path, http_method method, std::function<void()> handler) {
    server.on(path.c_str(), method, [handler]() {
        api_send_cors(server);
        handler();
    });
    api_handle_options(server, path.c_str());
}
