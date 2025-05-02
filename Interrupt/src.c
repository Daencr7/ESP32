
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "FreeRTOSConfig.h"
#include "esp_log.h"
static const char *TAG = "My_ESP";
// static uint8_t led_state = 0;
#define BUTTON_PIN GPIO_NUM_0
#define ESP_INTR_FLAG_DEFAULT 0
#define OUTPUT_GPIO GPIO_NUM_2

static uint8_t flag_blink = 0;
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    flag_blink = 1;
}


int app_main(void) {
    gpio_config_t io_out = {
        .pin_bit_mask = (1ULL << OUTPUT_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_out);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);  // nên là 0 hoặc NULL
    gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, NULL);

    gpio_set_level(OUTPUT_GPIO,1);
    for(;;){
        if(flag_blink){
            flag_blink = 0;
            for(int i = 0; i < 10; i++){
                gpio_set_level(GPIO_NUM_2,0);
                vTaskDelay(pdMS_TO_TICKS(10));
                gpio_set_level(GPIO_NUM_2,1);
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return 0;
}
