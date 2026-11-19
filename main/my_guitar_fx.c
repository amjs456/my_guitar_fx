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

typedef struct {
    char *name;
    void *effect_args;
} effect_args_t;

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

effect_args_t effect_args[] = {
    {"Tuner", &tuner_arg},
    {"Delay", &delay_args},
    {"Distortion", &distortion_args},
};



static void save_settings_task(void *args){
    while(1){
        uint32_t value;
        xTaskNotifyWait(0, 0, &value, portMAX_DELAY);
    }
}

void app_main(void)
{
    TaskHandle_t save_settings_task_handle;
    BaseType_t res = xTaskCreate(
        save_settings_task,
        "save_settings_task",
        4096,
        NULL,
        5,
        &save_settings_task_handle
    );
    if(res != pdTRUE){
        printf("save_settings_task creation failed: %d", res);
        return;
    }

    QueueHandle_t queue = xQueueCreate(1, sizeof(TuningResult));
    tuner_arg.queue_handle = queue;

    fx_t fx_array[] = {
        {
            tuner,
            tuner_ui_create,
            &tuner_arg,
            &save_settings_task_handle
        },
        {
            delay,
            delay_ui_create,
            &delay_args,
            &save_settings_task_handle
        },
    };

    size_t fx_array_length = sizeof(fx_array) / sizeof(fx_array[0]);

    ESP_ERROR_CHECK(my_guitar_fx_core_init(fx_array, fx_array_length));
    ESP_ERROR_CHECK(start_tasks());
}