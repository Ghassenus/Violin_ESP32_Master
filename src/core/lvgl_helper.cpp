#include <Arduino.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "lvgl_helper.h"

// === Définition écran et tactile ===
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

#define BACKLIGHT_PIN 21   // Ajouté pour la gestion du rétroéclairage
#define BACKLIGHT_CHANNEL 0

TFT_eSPI tft = TFT_eSPI();

#define TOUCH_CS   33
#define TOUCH_IRQ  36
#define TOUCH_MISO 39
#define TOUCH_MOSI 32
#define TOUCH_SCK  25
SPIClass spiTouch(HSPI);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);
static int current_pwm_value = 255; // 100% lumineux au démarrage
static int target_pwm_value = 255;
static int _current_brightness_percent = 100; // Variable interne

// === LVGL buffers ===
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 5];

// === Fonction d'affichage (flush) ===
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1,
                    area->x2 - area->x1 + 1,
                    area->y2 - area->y1 + 1);
  tft.pushColors((uint16_t *)color_p,
                 (area->x2 - area->x1 + 1) *
                 (area->y2 - area->y1 + 1), true);
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

// === Lecture tactile calibrée ===
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (touch.touched()) {
    TS_Point p = touch.getPoint();

    int16_t x = map(p.x, 385, 3527, 0, SCREEN_WIDTH);
    int16_t y = map(p.y, 544, 3547, 0, SCREEN_HEIGHT);

    x = constrain(x, 0, SCREEN_WIDTH - 1);
    y = constrain(y, 0, SCREEN_HEIGHT - 1);

    data->state = LV_INDEV_STATE_PR;
    data->point.x = x;
    data->point.y = y;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

// === Initialisation globale ===
void lvgl_setup() {
  // Initialisation écran
  tft.begin();
  tft.setRotation(1);

  // === PWM pour le rétroéclairage ===
  ledcSetup(BACKLIGHT_CHANNEL, 5000, 8);       // canal 0, 5 kHz, 8 bits
  ledcAttachPin(BACKLIGHT_PIN, BACKLIGHT_CHANNEL);
  ledcWrite(BACKLIGHT_CHANNEL, 255);            // 100% lumineux au départ

  // Efface l'écran
  tft.fillScreen(TFT_BLACK);

  // Initialisation tactile
  spiTouch.begin(TOUCH_SCK, TOUCH_MISO, TOUCH_MOSI);
  touch.begin(spiTouch);
  touch.setRotation(1);

  // Initialisation LVGL
  lv_init();

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * 5);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
}

void screen_set_brightness_percent(int percent) {
    percent = constrain(percent, 0, 100);
    _current_brightness_percent = percent;
    target_pwm_value = map(percent, 0, 100, 0, 255);
}  

void screen_brightness_loop() {
    if (current_pwm_value == target_pwm_value) return;

    if (current_pwm_value < target_pwm_value) {
        current_pwm_value += 5;
        if (current_pwm_value > target_pwm_value) current_pwm_value = target_pwm_value;
    } else if (current_pwm_value > target_pwm_value) {
        current_pwm_value -= 5;
        if (current_pwm_value < target_pwm_value) current_pwm_value = target_pwm_value;
    }

    ledcWrite(0, current_pwm_value);
}


int screen_get_brightness_percent() {
    return _current_brightness_percent;
}

// === À appeler dans loop() ===
void lvgl_loop() {
  lv_timer_handler();
  delay(5);
}
