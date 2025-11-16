#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>
#include <esp_rmaker_core.h>

#define DEFAULT_POWER       true
#define DEFAULT_HUE         180
#define DEFAULT_SATURATION  100
#define DEFAULT_BRIGHTNESS  25

extern esp_rmaker_device_t *light_device;

void app_driver_init(void);
esp_err_t app_driver_set_state(bool state);
esp_err_t app_driver_set_brightness(uint16_t brightness);
esp_err_t app_driver_set_hue(uint16_t hue);
esp_err_t app_driver_set_saturation(uint16_t saturation);

bool app_driver_get_state(void);
uint16_t app_driver_get_brightness(void);
uint16_t app_driver_get_hue(void);
uint16_t app_driver_get_saturation(void);
