#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/timers.h"
// define level and pin
#define GPIO_OUT GPIO_NUM_2
#define HIGH 1
#define LOW 0
// declared
static const char *TAG = "ESP";
static void vTask( void * pvParameters);
void led_init(void);

int app_main() {
    led_init();

    BaseType_t task1 = xTaskCreate( vTask, "Task1", 2048, NULL, 1, NULL);
    if ( task1 == pdPASS) {
        printf("Task was be Created\n");
    }

    for(;;){
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return 0;
}
void led_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << GPIO_OUT,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}
static void vTask ( void *pvParameters){
    ESP_LOGI(TAG, "Task is running");
    for(;;){
        // ESP_LOGI(TAG,)
        gpio_set_level(GPIO_OUT, LOW);
        ESP_LOGI(TAG,"BLUE LED is OFF ");
        vTaskDelay(pdMS_TO_TICKS(50));
        gpio_set_level(GPIO_OUT, HIGH);
        ESP_LOGI(TAG,"BLUE LED is ON  ");
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
