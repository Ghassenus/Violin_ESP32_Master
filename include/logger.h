#pragma once
#include <Arduino.h>
#include "telnet_manager.h"

// === Couleurs ANSI pour Telnet ===
#define ANSI_RESET  "\033[0m"
#define ANSI_RED    "\033[31m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_GREEN  "\033[32m"

// === Fonctions principales ===
inline void log_info(const String& msg) {
    Serial.println("[INFO] " + msg);
    telnet_manager_println(String(ANSI_GREEN) + "[INFO] " + msg + ANSI_RESET);
}

inline void log_warn(const String& msg) {
    Serial.println("[WARN] " + msg);
    telnet_manager_println(String(ANSI_YELLOW) + "[WARN] " + msg + ANSI_RESET);
}

inline void log_error(const String& msg) {
    Serial.println("[ERROR] " + msg);
    telnet_manager_println(String(ANSI_RED) + "[ERROR] " + msg + ANSI_RESET);
}

// === Version printf() style ===
inline void logf(const char* format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    Serial.println(buf);
    telnet_manager_println(buf);
}

// === Macros ultra rapides ===
#define LOGI(...) logf("[INFO] " __VA_ARGS__)
#define LOGW(...) logf("[WARN] " __VA_ARGS__)
#define LOGE(...) logf("[ERROR] " __VA_ARGS__)
