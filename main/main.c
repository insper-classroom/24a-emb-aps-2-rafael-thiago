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
#include "hardware/gpio.h"

#include "hc06.h"

QueueHandle_t xQueueA;
SemaphoreHandle_t xSemaphore;

#define BTN_PIN 17  // Defina o pino do botão conforme necessário

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

    if (voltage <= 0.95)
        return 0;

    return 1;

}

int test (char port, char teste1) {
    char teste2;
    vTaskDelay(pdMS_TO_TICKS(5));
    teste2 = read_n_detect(port);
    if (teste1 == teste2){
        return 1;
    }
    return 0;     
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {  // borda de queda (falling edge)
        xSemaphoreGiveFromISR(xSemaphore, NULL);
    }
} 

void hc06_task(void *p) {
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("Bateria_Laser", "1234");

    while (true) {
        uart_puts(HC06_UART_ID, "OLAAA ");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void btn_task(void *p){
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled(BTN_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    laser_t data;
    data.ID = 5;

    while(1){
        if( xSemaphoreTake(xSemaphore, 100) == pdTRUE ){
            data.value = 1;
            xQueueSend(xQueueA, &data, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Tarefa ociosa
    }
}

void laserG_task(void *p) {
    adc_gpio_init(28);
    char listV[2];
    laser_t data;
    data.ID = 0;
    char value;

    while(1){
        listV[0] = listV[1]; 
        value = read_n_detect(2);

        if (test(2, value)){
            listV[1] = value;

            if(listV[0] != listV[1]){
                data.value = listV[1];
                xQueueSend(xQueueA, &data, 0);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void laserR_task(void *p) {
    adc_gpio_init(27);
    char listV[2];
    laser_t data;
    data.ID = 1;
    char value;

    while(1){
        listV[0] = listV[1]; 
        value = read_n_detect(1);

        if (test(1,value)){
            listV[1] = value;
            
            if(listV[0] != listV[1]){
                data.value = listV[1];
                xQueueSend(xQueueA, &data, 0);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void laserY_task(void *p) {
    adc_gpio_init(26);
    char listV[2];
    laser_t data;
    data.ID = 2;
    char value;
    while(1){
        listV[0] = listV[1]; 
        value = read_n_detect(0);

        if (test(0, value)){
            listV[1] = value;
            
            if(listV[0] != listV[1]){
                data.value = listV[1];
                xQueueSend(xQueueA, &data, 0);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void uart_task(void *p) {
    laser_t data_to_send;
    while (1) {
        if (xQueueReceive(xQueueA, &data_to_send, pdMS_TO_TICKS(10))) {
            write_package(data_to_send);
        }
    }
}

void main() {
    stdio_init_all();

    adc_init();
    xQueueA = xQueueCreate(32, sizeof(laser_t));
    xSemaphore = xSemaphoreCreateBinary();

    printf("Start bluetooth task\n");

    xTaskCreate(laserG_task, "LASER_Task G", 4096, NULL, 1, NULL);
    xTaskCreate(laserR_task, "LASER_Task R", 4096, NULL, 1, NULL);
    xTaskCreate(laserY_task, "LASER_Task Y", 4096, NULL, 1, NULL);
    xTaskCreate(uart_task, "Uart_Task 1", 4096, NULL, 1, NULL);
    xTaskCreate(btn_task, "BTN_Task", 4096, NULL, 1, NULL);
    xTaskCreate(hc06_task, "bluetooth Task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
