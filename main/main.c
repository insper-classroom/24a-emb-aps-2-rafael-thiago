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

typedef struct {
    int queue_number;
    int is_pressed;
} laser_t;

void write_package(laser_t data) {
    int queue_number = queue_number;
    int is_pressed = data.is_pressed;

    uart_putc_raw(uart0, queue_number);
    uart_putc_raw(uart0, is_pressed);
    uart_putc_raw(uart0, -1);
}

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
            printf("result: %d \n", listV[1]);
            data = listV[1];
            xQueueSend(xQueueA, &data, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void uart_task(void *p) {
    laser_t data_to_send;
    int data;
    while (1) {
        // xQueueReceive(xQueueA, &data, portMAX_DELAY);
        // uart_write_blocking(uart0, &data, sizeof(int));
        if (xQueueReceive(xQueueA, &data, pdMS_TO_TICKS(10))) {
            data_to_send.queue_number = 0;
            data_to_send.is_pressed = data;
            write_package(data_to_send);
        }
    }
}

int main() {
    stdio_init_all();

    adc_init();

    xQueueA = xQueueCreate(32, sizeof(int));

    printf("Start bluetooth task\n");

    xTaskCreate(laser_task, "LASER_Task 1", 4096, NULL, 1, NULL);
    xTaskCreate(uart_task, "Uart_Task 1", 4096, NULL, 1, NULL);
    //xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
