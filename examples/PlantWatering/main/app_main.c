#include <stdio.h>
#include <stdbool.h>

#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_node.h>
#include <app_reset.h>

#include "app_priv.h"

static const char *TAG = "app_main";

esp_rmaker_device_t *light_device;

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    /* RainMaker node */
    esp_rmaker_node_t *node = esp_rmaker_node_init("PlantWateringDevice", "Light");
    if (!node) {
        ESP_LOGE(TAG, "Node init failed");
        return;
    }

    /* Device: Light with Brightness + Hue + Saturation */
    light_device = esp_rmaker_device_create("PWM Light", ESP_RMAKER_DEVICE_LIGHT, NULL);

    esp_rmaker_device_add_param(light_device,
        esp_rmaker_power_param_create(ESP_RMAKER_PARAM_POWER, DEFAULT_POWER));

    esp_rmaker_device_add_param(light_device,
        esp_rmaker_brightness_param_create(ESP_RMAKER_PARAM_BRIGHTNESS, DEFAULT_BRIGHTNESS));

    esp_rmaker_device_add_param(light_device,
        esp_rmaker_hue_param_create(ESP_RMAKER_PARAM_HUE, DEFAULT_HUE));

    esp_rmaker_device_add_param(light_device,
        esp_rmaker_saturation_param_create(ESP_RMAKER_PARAM_SATURATION, DEFAULT_SATURATION));

    esp_rmaker_node_add_device(node, light_device);

    /* Driver */
    app_driver_init();

    /* Start RainMaker */
    esp_rmaker_start();

    /* Enable Reset (long-press) */
    app_reset_button_register(CONFIG_EXAMPLE_BOARD_BUTTON_GPIO, 0);

    ESP_LOGI(TAG, "PlantWatering Matter/PWM device started!");
}
