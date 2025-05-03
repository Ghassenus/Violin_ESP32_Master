#pragma once
#include <Arduino.h>

void ws_server_init();
void ws_server_loop();

// Diffuse un événement (type + contenu JSON brut)
void ws_broadcast(const String& type, const String& payload);
