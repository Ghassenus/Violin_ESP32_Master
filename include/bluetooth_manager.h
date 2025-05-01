#pragma once
#include <Arduino.h>
namespace bluetooth_manager {

void init();
void loop();

void scan(); // Demande un scan Bluetooth à ESP2
void connect(const String& mac_address); // Demande une connexion Bluetooth à ESP2

}
