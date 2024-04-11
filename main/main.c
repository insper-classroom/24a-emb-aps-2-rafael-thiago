/*
 * LED blink with FreeRTOS
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include <string.h>

#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/adc.h"

#include "hc06.h"

QueueHandle_t xQueueA;

void hc06_task(void *p) {
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("aps2_legal", "1234");

    while (true) {
        uart_puts(HC06_UART_ID, "OLAAA ");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void laser_task(void *p) {
    adc_gpio_init(27);
    int listV[2] = {0 ,0};
    int data;
    while(1){
        adc_select_input(1);
        int result = adc_read();
        float voltage = result * 3.3 / 4096;
        listV[0] = listV[1];
        if (voltage <= 0.9){
            listV[1] = 0;
        } else {
            listV[1] = 1;
        }

        if(listV[0] != listV[1]){
            printf("result: %f \n", listV[1]);
            data = listV[1];
            xQueueSend(xQueueA, &data, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main() {
    stdio_init_all();

    adc_init();

    xQueueA = xQueueCreate(32, sizeof(int));

    printf("Start bluetooth task\n");

    xTaskCreate(laser_task, "LASER_Task 1", 4096, NULL, 1, NULL);
    //xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
