// src/core/battery.h
#pragma once
#include <cstdint>
#define PIN_BAT_VOLT 34 // Entrée ADC pour la batterie
// broche ADC reliée à la mesure de batterie
void     battery_init();
uint32_t battery_get_voltage_mV();
uint8_t  battery_get_percent();
bool battery_is_charging();
