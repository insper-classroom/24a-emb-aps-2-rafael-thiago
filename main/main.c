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
#include "controle.h"


void main() {
    stdio_init_all();

    adc_init();
    xQueueA = xQueueCreate(32, sizeof(laser_t));
    xSemaphoreSpace = xSemaphoreCreateBinary();
    xSemaphoreEsc = xSemaphoreCreateBinary();

    gpio_init(13);
    gpio_set_dir(13, GPIO_OUT);
    gpio_init(14);
    gpio_set_dir(14, GPIO_OUT);
    gpio_init(15);
    gpio_set_dir(15, GPIO_OUT);

    printf("Start bluetooth task\n");

    xTaskCreate(laserG_task, "LASER_Task G", 4096, NULL, 1, NULL);
    xTaskCreate(laserR_task, "LASER_Task R", 4096, NULL, 1, NULL);
    xTaskCreate(laserY_task, "LASER_Task Y", 4096, NULL, 1, NULL);
    // xTaskCreate(uart_task, "Uart_Task 1", 4096, NULL, 1, NULL);
    xTaskCreate(btn_task, "BTN_Task", 4096, NULL, 1, NULL);
    xTaskCreate(btn_esc_task, "BTN_ESC_Task", 4096, NULL, 1, NULL);
    xTaskCreate(hc06_task, "bluetooth Task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}