

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "FreeRTOSConfig.h"
#include "esp_log.h"

#define OUTPUT_GPIO GPIO_NUM_2
static uint8_t led_state = 0;



void Task1 (void *parameter){
    for(;;){
        led_state  = !led_state;
        gpio_set_level( OUTPUT_GPIO, led_state );
        ESP_LOGI(TAG," %s " ,led_state==true ? "ON":"OFF" );
        printf("Running system!\n");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int app_main(void) {
    gpio_config_t io_out = {
        .pin_bit_mask = (1ULL << OUTPUT_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    xTaskCreate(Task1, "Task1", 4096, NULL, 1, NULL);

    return 0;
}
