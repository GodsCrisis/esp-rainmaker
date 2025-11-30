/* PWM Driver for GPIO10 - RainMaker Light Device */

#include <sdkconfig.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_standard_params.h>

#include "app_priv.h"

/* Output pin */
#define OUTPUT_GPIO          10

/* PWM (LEDC) configuration */
#include <driver/ledc.h>

#define PWM_CHANNEL      LEDC_CHANNEL_0
#define PWM_TIMER        LEDC_TIMER_0
#define PWM_MODE         LEDC_LOW_SPEED_MODE
#define PWM_RESOLUTION   LEDC_TIMER_10_BIT
#define PWM_FREQUENCY    1000

static bool g_power_state = DEFAULT_POWER;
static float g_brightness = DEFAULT_BRIGHTNESS;
static float g_hue = DEFAULT_HUE;
static float g_saturation = DEFAULT_SATURATION;

/* PWM helpers */
static void gpio10_set_duty(uint32_t duty)
{
    ledc_set_duty(PWM_MODE, PWM_CHANNEL, duty);
    ledc_update_duty(PWM_MODE, PWM_CHANNEL);
}

static void pwm_init()
{
    ledc_timer_config_t timer = {
        .speed_mode = PWM_MODE,
        .duty_resolution = PWM_RESOLUTION,
        .timer_num = PWM_TIMER,
        .freq_hz = PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num = OUTPUT_GPIO,
        .speed_mode = PWM_MODE,
        .channel = PWM_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = PWM_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel);
}

esp_err_t app_driver_set_state(bool state)
{
    g_power_state = state;

    if (state) {
        uint32_t duty = (g_brightness * 1023) / 100;
        gpio10_set_duty(duty);
    } else {
        gpio10_set_duty(0);
    }
    return ESP_OK;
}

esp_err_t app_driver_set_brightness(uint16_t brightness)
{
    if (brightness > 100) brightness = 100;
    g_brightness = brightness;

    if (g_power_state) {
        uint32_t duty = (brightness * 1023) / 100;
        gpio10_set_duty(duty);
    }
    return ESP_OK;
}

esp_err_t app_driver_set_hue(uint16_t hue)
{
    g_hue = hue; 
    return ESP_OK;
}

esp_err_t app_driver_set_saturation(uint16_t saturation)
{
    g_saturation = saturation;
    return ESP_OK;
}

bool app_driver_get_state(void) { return g_power_state; }
uint16_t app_driver_get_brightness(void) { return g_brightness; }
uint16_t app_driver_get_hue(void) { return g_hue; }
uint16_t app_driver_get_saturation(void) { return g_saturation; }

void app_driver_init()
{
    pwm_init();
    app_driver_set_state(DEFAULT_POWER);
}
