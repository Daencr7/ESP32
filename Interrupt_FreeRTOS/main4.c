#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/timers.h"

#define GPIO_OUT GPIO_NUM_2
#define GPIO_OUT_CONTROL GPIO_NUM_15
#define HIGH 1
#define LOW 0
#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS( 3333UL )
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS( 500UL )
#define mainINTERRUPT_NUMBER 3
// const TickType_t xHealthyTimerPeriod = pdMS_TO_TICKS( 3000 );
// const TickType_t xErrorTimerPeriod = pdMS_TO_TICKS( 200 );
// static void prvOneShotTimerCallback( TimerHandle_t xTimer);
// static void prvAutoReloadTimerCallback( TimerHandle_t xTimer);
// static void prvTimerCallback( TimerHandle_t xTimer);
// static const char *TAG = "ESP";
// static void vTask1( void * pvParameters);
// static void vTask2( void * pvParameters);
// void led_init(void);
// TimerHandle_t xAutoReloadTimer, xOneShotTimer;
// SemaphoreHandle_t xBinarySemahore;
static void vStringPrinter( void *pvParameters);
static void vIntegerGenerator( void *pvParameters );
static void IRAM_ATTR gpio_isr_handler(void *arg);
void led_init(void);
QueueHandle_t xIntegerQueue, xStringQueue;
int app_main() {
    led_init();
    
    xIntegerQueue = xQueueCreate( 10, sizeof(uint32_t) );
    xStringQueue = xQueueCreate( 10, sizeof(char*) );
    xTaskCreate( vIntegerGenerator, "IntGen", 1000, NULL, 1, NULL);
    xTaskCreate( vStringPrinter, "String", 1000, NULL, 2, NULL);

    gpio_set_level(GPIO_OUT_CONTROL, LOW);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(GPIO_OUT_CONTROL, HIGH);
    vTaskDelay(pdMS_TO_TICKS(50));
    
    for(;;){
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return 0;
}

static void vStringPrinter( void *pvParameters){
    char *pcString;
    for(;;){
        xQueueReceive( xStringQueue, &pcString, portMAX_DELAY );
        printf("%s\n",pcString);
    }
}

static void IRAM_ATTR gpio_isr_handler(void *arg){
    BaseType_t xHigherPriorityTaskWoken; 
    uint32_t ulReceivedNumber;

    static const char *pcStrings[] = { 
        "String 0\r\n", 
        "String 1\r\n", 
        "String 2\r\n", 
        "String 3\r\n" 
    }; 
    xHigherPriorityTaskWoken = pdFALSE; 
    while( xQueueReceiveFromISR( xIntegerQueue, 
                                 &ulReceivedNumber, 
                                 &xHigherPriorityTaskWoken ) != errQUEUE_EMPTY ) {
        ulReceivedNumber &= 0x03; 
        xQueueSendToBackFromISR( xStringQueue, 
                                 &pcStrings[ ulReceivedNumber ], 
                                 &xHigherPriorityTaskWoken ); 
    } 
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void vIntegerGenerator( void *pvParameters ) { 
    TickType_t xLastExecutionTime; 
    uint32_t ulValueToSend = 0; 
    int i; 
    xLastExecutionTime = xTaskGetTickCount(); 
    for( ;; ) { 
        vTaskDelayUntil( &xLastExecutionTime, pdMS_TO_TICKS( 200 ) ); 
        for( i = 0; i < 5; i++ ) { 
            xQueueSendToBack( xIntegerQueue, &ulValueToSend, 0 ); 
            ulValueToSend++; 
        } 
        printf( "Generator task - About to generate an interrupt.\r\n" ); 
        // vPortGenerateSimulatedInterrupt( mainINTERRUPT_NUMBER ); 
        gpio_set_level(GPIO_OUT_CONTROL, LOW);
        vTaskDelay(pdMS_TO_TICKS(50));
        gpio_set_level(GPIO_OUT_CONTROL, HIGH);
        vTaskDelay(pdMS_TO_TICKS(50));
        printf( "Generator task - Interrupt generated.\r\n\r\n\r\n" ); 
    } 
} 


void led_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << GPIO_OUT,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };
    gpio_config(&io_conf);
    gpio_config_t io_conf_control = {
        .pin_bit_mask = 1ULL << GPIO_OUT_CONTROL,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_control);
    
    gpio_install_isr_service(0);
    
    gpio_isr_handler_add(GPIO_OUT, gpio_isr_handler, NULL);
}

