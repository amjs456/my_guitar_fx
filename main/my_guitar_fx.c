#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "my_guitar_fx_core.h"

#include "esp_check.h"
#include <stddef.h>
#include <stdint.h>

#include "my_guitar_fx_effects.h"
#include "portmacro.h"
#include "sdkconfig.h"
#include "tuner.h"
#include "tuner_ui.h"
#include "delay.h"
#include "delay_ui.h"
#include "distortion.h"
#include "distortion_ui.h"


static tuner_arg_t tuner_arg = {
    .tuning = "Standard",
    .reference_frequency_hz = 440.0f
};

static delay_args_t delay_args = {
    .delay_time = 800,
    95,
    50
};

static distortion_args_t distortion_args = {
    .gain_db = 10,
};

void app_main(void)
{
    QueueHandle_t queue = xQueueCreate(1, sizeof(TuningResult));
    tuner_arg.queue_handle = queue;

    fx_t fx_array[] = {
        {
            "Tuner",
            tuner,
            tuner_ui_create,
            &tuner_arg
        },
        {
            "Delay",
            delay,
            delay_ui_create,
            &delay_args
        },
        {"Distortion",
            distortion,
            distortion_ui_create,
            &distortion_args
        }
    };

    size_t fx_array_length = sizeof(fx_array) / sizeof(fx_array[0]);

    ESP_ERROR_CHECK(my_guitar_fx_core_init(fx_array, fx_array_length));
    ESP_ERROR_CHECK(start_tasks());
}