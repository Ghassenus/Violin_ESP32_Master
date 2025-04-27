#pragma once
#include <Arduino.h>

void telnet_manager_init();
void telnet_manager_handle();
void telnet_manager_print(const String& msg);
void telnet_manager_println(const String& msg);