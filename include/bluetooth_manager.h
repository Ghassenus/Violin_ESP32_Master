#pragma once
#include <Arduino.h>
#include <vector>
#include "uart_dispatcher.h"
namespace bluetooth_manager {
   

void init();
void loop();

void scan(); // Demande un scan Bluetooth à ESP2
void connect(const String& mac_address); // Demande une connexion Bluetooth à ESP2
const std::vector<BtDevice>& get_scanned_devices(); // list all scande devices
void clear_scanned_devices(); // clear all scaned devices

}
