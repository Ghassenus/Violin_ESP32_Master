#include <Arduino.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "lvgl_helper.h"

// === Définition écran et tactile ===
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

TFT_eSPI tft = TFT_eSPI();

#define TOUCH_CS   33
#define TOUCH_IRQ  36
#define TOUCH_MISO 39
#define TOUCH_MOSI 32
#define TOUCH_SCK  25
SPIClass spiTouch(HSPI);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

// === LVGL buffers ===
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 10];

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

    // Pas de rotation
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
  tft.begin();
  tft.setRotation(1);
  pinMode(21, OUTPUT); digitalWrite(21, HIGH);
  tft.fillScreen(TFT_BLACK);

  spiTouch.begin(TOUCH_SCK, TOUCH_MISO, TOUCH_MOSI);
  touch.begin(spiTouch);
  touch.setRotation(1);

  lv_init();

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * 10);

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

// === À appeler dans loop() ===
void lvgl_loop() {
  lv_timer_handler();
  delay(5);
}
