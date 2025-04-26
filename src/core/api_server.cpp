#include "api_server.h"
#include <WebServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "battery.h"
#include "parameters_manager.h"
#include "wifi_manager.h"
#include "lvgl_helper.h"
static WebServer server(80);

void api_server_init() {

  Serial.println("[API] Initialisation serveur HTTP...");
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "Hello World");
  });

  // GET /api/status
  server.on("/api/status", HTTP_GET, []() {
    StaticJsonDocument<256> doc;
    doc["wifi"]   = WiFi.isConnected();
    doc["ssid"]   = WiFi.SSID();
    doc["ip"]     = WiFi.localIP().toString();
    doc["rssi"]   = WiFi.RSSI();
    doc["batt"]   = battery_get_percent();
    doc["fmt24h"] = parameters_manager_is_24h_format();

    String out;
    serializeJson(doc, out);
    server.send(200, "application/json", out);
  });

  // POST /api/wifi/connect
  server.on("/api/wifi/connect", HTTP_POST, []() {
    StaticJsonDocument<200> doc;
    DeserializationError err = deserializeJson(doc, server.arg("plain"));

    if (err || !doc["ssid"] || !doc["pass"]) {
      server.send(400, "application/json", "{\"error\":\"ssid & pass required\"}");
      return;
    }

    Preferences p; p.begin("wifi", false);
    const char* ssid = doc["ssid"];
    const char* pass = doc["pass"];
    p.putString("ssid", ssid);
    p.putString("pass", pass);
    p.end();

    wifi_manager_connect();

    server.send(200, "application/json", "{\"result\":\"ok\"}");
  });

  // POST /api/params/time
  server.on("/api/params/time", HTTP_POST, []() {
    StaticJsonDocument<128> doc;
    DeserializationError err = deserializeJson(doc, server.arg("plain"));

    if (err || !doc["hour"] || !doc["minute"]) {
      server.send(400, "application/json", "{\"error\":\"hour & minute required\"}");
      return;
    }

    parameters_manager_set_hour(doc["hour"]);
    parameters_manager_set_minute(doc["minute"]);
    server.send(200, "application/json", "{\"result\":\"ok\"}");
  });

  // POST /api/params/date
  server.on("/api/params/date", HTTP_POST, []() {
    StaticJsonDocument<128> doc;
    DeserializationError err = deserializeJson(doc, server.arg("plain"));

    if (err || !doc["year"] || !doc["month"] || !doc["day"]) {
      server.send(400, "application/json", "{\"error\":\"year, month & day required\"}");
      return;
    }

    parameters_manager_set_date(doc["year"], doc["month"], doc["day"]);
    server.send(200, "application/json", "{\"result\":\"ok\"}");
  });

  // POST /api/params/format
  server.on("/api/params/format", HTTP_POST, []() {
    StaticJsonDocument<64> doc;
    DeserializationError err = deserializeJson(doc, server.arg("plain"));

    if (err || !doc["format24"].is<bool>()) {
      server.send(400, "application/json", "{\"error\":\"format24 required\"}");
      return;
    }

    parameters_manager_set_time_format(doc["format24"]);
    server.send(200, "application/json", "{\"result\":\"ok\"}");
  });

  // POST /api/display/brightness
  server.on("/api/display/brightness", HTTP_POST, []() {
    StaticJsonDocument<64> doc;
    DeserializationError err = deserializeJson(doc, server.arg("plain"));

    if (err || !doc["brightness"].is<int>()) {
        server.send(400, "application/json", "{\"error\":\"brightness (0-100) required\"}");
        return;
    }

    int brightness = doc["brightness"];
    screen_set_brightness_percent(brightness);

    server.send(200, "application/json", "{\"result\":\"ok\"}");
  });

  // 404
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
}

void api_server_loop() {
  server.handleClient(); // loop-safe, non bloquant si handler rapide
}