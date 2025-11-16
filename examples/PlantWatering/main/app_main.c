/* ESP RainMaker GPIO10 Voltage Control
 * 
 * This application allows control of GPIO10 voltage output via ESP RainMaker app.
 * Features:
 * - ON/OFF control
 * - Brightness control (0-100%) which controls output voltage via PWM
 * - Cloud connectivity via ESP RainMaker
 * - Local control via ESP RainMaker app
 */

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_standard_devices.h>
#include <esp_rmaker_schedule.h>
#include <esp_rmaker_scenes.h>
#include <esp_rmaker_console.h>
#include <esp_rmaker_ota.h>

#include <esp_rmaker_common_events.h>

#include <app_wifi.h>
#include <app_insights.h>

#include "app_priv.h"

static const char *TAG = "app_main";

/* Callback for power state changes from RainMaker */
static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
            const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    if (ctx) {
        ESP_LOGI(TAG, "Received write request via : %s", esp_rmaker_device_cb_src_to_str(ctx->src));
    }
    
    const char *param_name = esp_rmaker_param_get_name(param);
    
    if (strcmp(param_name, ESP_RMAKER_DEF_POWER_NAME) == 0) {
        /* Power ON/OFF */
        ESP_LOGI(TAG, "Received value = %s for %s", val.val.b ? "true" : "false", param_name);
        app_driver_set_power(val.val.b);
        esp_rmaker_param_update_and_report(param, val);
        
    } else if (strcmp(param_name, ESP_RMAKER_DEF_BRIGHTNESS_NAME) == 0) {
        /* Brightness control (0-100) */
        ESP_LOGI(TAG, "Received value = %d for %s", val.val.i, param_name);
        app_driver_set_brightness(val.val.i);
        esp_rmaker_param_update_and_report(param, val);
        
    } else {
        /* Unknown parameter */
        ESP_LOGW(TAG, "Unknown parameter: %s", param_name);
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

void app_main()
{
    /* Initialize NVS (for WiFi credentials, etc.) */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* Initialize GPIO10 PWM driver */
    app_driver_init();

    /* Initialize App Insights (optional telemetry) */
    app_insights_enable();

    /* Initialize ESP RainMaker */
    esp_rmaker_config_t rainmaker_cfg = {
        .enable_time_sync = false,
    };
    esp_rmaker_node_t *node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", "GPIO10 Controller");
    if (!node) {
        ESP_LOGE(TAG, "Could not initialise node.");
        vTaskDelay(5000/portTICK_PERIOD_MS);
        abort();
    }

    /* Create a Lightbulb device (for power + brightness control) */
    esp_rmaker_device_t *light_device = esp_rmaker_lightbulb_device_create("GPIO10 Output", NULL, DEFAULT_POWER);
    esp_rmaker_device_add_cb(light_device, write_cb, NULL);

    /* Add brightness parameter (0-100) */
    esp_rmaker_device_add_param(light_device, esp_rmaker_brightness_param_create(ESP_RMAKER_DEF_BRIGHTNESS_NAME, DEFAULT_BRIGHTNESS));

    /* Add device to node */
    esp_rmaker_node_add_device(node, light_device);

    /* Enable OTA (firmware updates) */
    esp_rmaker_ota_enable_default();

    /* Enable timezone service */
    esp_rmaker_timezone_service_enable();

    /* Enable scheduling (optional - for timed actions) */
    esp_rmaker_schedule_enable();

    /* Enable scenes (optional - for automation) */
    esp_rmaker_scenes_enable();

    /* Start ESP RainMaker */
    esp_rmaker_start();

    /* Start WiFi */
    err = app_wifi_start(POP_TYPE_RANDOM);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not start WiFi. Aborting!!!");
        vTaskDelay(5000/portTICK_PERIOD_MS);
        abort();
    }

    ESP_LOGI(TAG, "=========================================");
    ESP_LOGI(TAG, "ESP RainMaker GPIO10 Voltage Controller");
    ESP_LOGI(TAG, "GPIO10 Output Control via RainMaker App");
    ESP_LOGI(TAG, "PWM: 1kHz, 10-bit resolution (0-100%%)");
    ESP_LOGI(TAG, "=========================================");
}
