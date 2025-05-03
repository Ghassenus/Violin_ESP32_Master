#pragma once
#include <uart_dispatcher.h>
#include <http_parser.h>

void api_server_init();
void api_server_loop();
void on_uart_message(const UartMessage& msg);
void register_handler(const String& path, http_method method, std::function<void()> handler);
