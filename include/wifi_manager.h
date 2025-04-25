#pragma once
#include <Arduino.h>
void wifi_manager_scan_start();
bool wifi_manager_scan_in_progress();
void wifi_manager_scan_handle();
int  wifi_manager_get_scan_count();
const char* wifi_manager_get_ssid(int idx);
void wifi_manager_connect();
void wifi_manager_start_ap(); // Axtiver le mode AP
