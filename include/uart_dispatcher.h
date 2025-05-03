// src/core/uart_dispatcher.h
#pragma once
#include "uart_parser.h"
#include <vector> 

struct BtDevice{
    String mac;
    String name;
}; 

// Fonction à appeler depuis uart_manager
void uart_dispatch_message(const UartMessage& msg);
const std::vector<BtDevice>& uart_get_scanned_devices();
void uart_clear_scanned_devices();