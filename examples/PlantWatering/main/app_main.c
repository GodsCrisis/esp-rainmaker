/* LED Light Example - Modified for GPIO10 PWM Output
   
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <sdkconfig.h>
#include <iot_button.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_standard_params.h>
#include <app_reset.h>
#include <ws2812_led.h>

#include "app_priv.h"

/* This is the button that is used for toggling the power */
#define BUTTON_GPIO          CONFIG_EXAMPLE_BOARD_BUTTON_GPIO
#define BUTTON_ACTIVE_LEVEL  0

/* This is the GPIO on which the power will be set */
#define OUTPUT_GPIO    10  // Changed from CONFIG_EXAMPLE_OUTPUT_GPIO to GPIO10

static bool g_power_state = DEFAULT_POWER;
static float g_hue = DEFAULT_HUE;
static float g_saturation = DEFAULT_SATURATION;
static float g_value = DEFAULT_BRIGHTNESS;

/**************************************************************************************************
 *
 * PWM (LEDC) initialization for GPIO10
 *
 **************************************************************************************************/
#include <driver/ledc.h>

#define PWM_CHANNEL     LEDC_CHANNEL_0
#define PWM_TIMER       LEDC_TIMER_0
#define PWM_MODE        LEDC_LOW_SPEED_MODE
#define PWM_RESOLUTION  LEDC_TIMER_10_BIT  // 0-1023
#define PWM_FREQUENCY   1000               // 1kHz

static void app_indicator_set(bool state)
{
    /* No indicator needed for GPIO output */
}

static void app_indicator_init(void)
{
    /* Initialize PWM for GPIO10 */
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = PWM_MODE,
        .duty_resolution  = PWM_RESOLUTION,
        .timer_num        = PWM_TIMER,
        .freq_hz          = PWM_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num       = OUTPUT_GPIO,
        .speed_mode     = PWM_MODE,
        .channel        = PWM_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = PWM_TIMER,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

static void gpio10_set_duty(uint32_t duty)
{
    ledc_set_duty(PWM_MODE, PWM_CHANNEL, duty);
    ledc_update_duty(PWM_MODE, PWM_CHANNEL);
}

esp_err_t app_driver_set_state(bool state)
{
    g_power_state = state;
    
    if (state) {
        // Turn ON - apply current brightness
        uint32_t duty = (g_value * 1023) / 100;  // Convert 0-100 to 0-1023
        gpio10_set_duty(duty);
    } else {
        // Turn OFF
        gpio10_set_duty(0);
    }
    
    return ESP_OK;
}

bool app_driver_get_state(void)
{
    return g_power_state;
}

esp_err_t app_driver_set_brightness(uint16_t brightness)
{
    if (brightness > 100) {
        brightness = 100;
    }
    
    g_value = brightness;
    
    // Apply if currently ON
    if (g_power_state) {
        uint32_t duty = (brightness * 1023) / 100;
        gpio10_set_duty(duty);
    }
    
    return ESP_OK;
}

uint16_t app_driver_get_brightness(void)
{
    return g_value;
}

esp_err_t app_driver_set_hue(uint16_t hue)
{
    // Hue not used for simple PWM output
    g_hue = hue;
    return ESP_OK;
}

uint16_t app_driver_get_hue(void)
{
    return g_hue;
}

esp_err_t app_driver_set_saturation(uint16_t saturation)
{
    // Saturation not used for simple PWM output
    g_saturation = saturation;
    return ESP_OK;
}

uint16_t app_driver_get_saturation(void)
{
    return g_saturation;
}

static void push_btn_cb(void *arg)
{
    bool new_state = !g_power_state;
    app_driver_set_state(new_state);
    esp_rmaker_param_update_and_report(
            esp_rmaker_device_get_param_by_type(light_device, ESP_RMAKER_PARAM_POWER),
            esp_rmaker_bool(new_state));
}

void app_driver_init()
{
    button_handle_t btn_handle = iot_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LEVEL);
    if (btn_handle) {
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_RELEASE, push_btn_cb, "PUSH");
    }

    /* Initialize indicator (PWM on GPIO10) */
    app_indicator_init();
    
    /* Set initial state */
    app_driver_set_state(DEFAULT_POWER);
}
