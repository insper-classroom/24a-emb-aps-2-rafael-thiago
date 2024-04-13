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
    char ID;
    char value;
} laser_t;

void write_package(laser_t data) {
    uart_putc_raw(uart0, data.ID);
    uart_putc_raw(uart0, data.value);
    uart_putc_raw(uart0, -1);
}

int read_n_detect (char port) { //lê a entrada adc recebida e devolve 1 se o sensor detecta proximidade
    adc_select_input(port);
    int result = adc_read();
    float voltage = result * 3.3 / 4096;

    if (voltage <= 0.9)
        return 0;

    return 1;

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

void laserG_task(void *p) {
    adc_gpio_init(28);
    char listV[2];
    laser_t data;
    data.ID = 0;
    while(1){
        listV[0] = listV[1];
        listV[1] = read_n_detect(2);

        if(listV[0] != listV[1]){
            data.value = listV[1];
            xQueueSend(xQueueA, &data, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void laserR_task(void *p) {
    adc_gpio_init(27);
    char listV[2];
    laser_t data;
    data.ID = 1;
    while(1){
        listV[0] = listV[1];
        listV[1] = read_n_detect(1);

        if(listV[0] != listV[1]){
            data.value = listV[1];
            xQueueSend(xQueueA, &data, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void laserY_task(void *p) {
    adc_gpio_init(26);
    char listV[2];
    laser_t data;
    data.ID = 2;
    while(1){
        listV[0] = listV[1];
        listV[1] = read_n_detect(0);

        if(listV[0] != listV[1]){
            data.value = listV[1];
            xQueueSend(xQueueA, &data, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void uart_task(void *p) {
    laser_t data_to_send;
    while (1) {
        // xQueueReceive(xQueueA, &data, portMAX_DELAY);
        // uart_write_blocking(uart0, &data, sizeof(int));
        if (xQueueReceive(xQueueA, &data_to_send, pdMS_TO_TICKS(10))) {
            write_package(data_to_send);
        }
    }
}

int main() {
    stdio_init_all();

    adc_init();

    xQueueA = xQueueCreate(32, sizeof(laser_t));

    printf("Start bluetooth task\n");

    xTaskCreate(laserG_task, "LASER_Task G", 4096, NULL, 1, NULL);
    xTaskCreate(laserR_task, "LASER_Task R", 4096, NULL, 1, NULL);
    xTaskCreate(laserY_task, "LASER_Task Y", 4096, NULL, 1, NULL);
    xTaskCreate(uart_task, "Uart_Task 1", 4096, NULL, 1, NULL);
    //xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
