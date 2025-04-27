#pragma once
#include<Arduino.h>

void uart_manager_init();
void uart_manager_loop();
void uart_manager_send(const String& msg);
