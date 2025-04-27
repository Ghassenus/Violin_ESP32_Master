// src/core/battery.cpp
#include "battery.h"
#include <esp_adc_cal.h>
#include <Arduino.h>

#define PIN_BAT_VOLT 34 // ADC pin
static constexpr uint32_t DEFAULT_VREF = 1100; // mV
static constexpr uint16_t MIN_VOLTAGE = 2500; 
static constexpr uint16_t MAX_VOLTAGE = 4200; 

static esp_adc_cal_characteristics_t adc_chars;
static float voltage_smoothed = 0;
static float voltage_trend = 0;

void battery_init() {
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);

    esp_adc_cal_characterize(
      ADC_UNIT_1,
      ADC_ATTEN_DB_11,
      ADC_WIDTH_BIT_12,
      DEFAULT_VREF,
      &adc_chars
    );
}

uint32_t battery_get_voltage_mV() {
    uint32_t raw = analogRead(PIN_BAT_VOLT);
    uint32_t mv = (raw * 3300UL / 4095UL) * 2;

    if (voltage_smoothed == 0) {
        voltage_smoothed = mv;  // Première lecture
        voltage_trend = mv;
    } else {
        voltage_smoothed = voltage_smoothed * 0.90f + mv * 0.10f;
        voltage_trend    = voltage_trend * 0.98f + mv * 0.02f; // encore plus lent pour la tendance
    }
    return (uint32_t) voltage_smoothed;
}

uint8_t battery_get_percent() {
    uint32_t mv = battery_get_voltage_mV();
    if (mv <= MIN_VOLTAGE)  return 0;
    if (mv >= MAX_VOLTAGE)  return 100;
    return (uint8_t)((mv - MIN_VOLTAGE) * 100 / (MAX_VOLTAGE - MIN_VOLTAGE));
}

bool battery_is_charging() {
    // Si la tension "rapide" est au-dessus de la tendance "lente" + 20mV
    // --> alors on considère qu'on est en charge
    return (voltage_smoothed > voltage_trend + 20);
}
