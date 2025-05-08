#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/timers.h"

#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS( 3333 )
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS( 500 )

static void prvOneShotTimerCallback( TimerHandle_t xTimer);
static void prvAutoReloadTimerCallback( TimerHandle_t xTimer);
void led_init(void);

int app_main() {
    led_init();
    TimerHandle_t xAutoReloadTimer, xOneShotTimer;
    BaseType_t xTimer1Started, xTimer2Started;
                            prvOneShotTimerCallback );

    xAutoReloadTimer = xTimerCreate("AutoReload", 
                                mainAUTO_RELOAD_TIMER_PERIOD,
                                pdTRUE,
                                0,
                                prvAutoReloadTimerCallback );
    if( ( xAutoReloadTimer != NULL ) ){
        xTimer2Started = xTimerStart( xAutoReloadTimer, 0);
    }
    for(;;){
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return 0;
}
void led_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << GPIO_NUM_2,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}
static void prvOneShotTimerCallback( TimerHandle_t xTimer){
    TickType_t xTimeNow;
    xTimeNow = xTaskGetTickCount();
    printf( " One-shot timer callback executing %ld\n", xTimeNow );
}

static void prvAutoReloadTimerCallback( TimerHandle_t xTimer){
    TickType_t xTimeNow;
    xTimeNow = xTaskGetTickCount();
    gpio_set_level(GPIO_NUM_2, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(GPIO_NUM_2, 0);
    printf(" Auto-reload timer callback executing %ld\n", xTimeNow);
}
