#ifndef CONTROLE_H
#define CONTROLE_H

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <string.h>

#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/gpio.h"

#define BTN_PIN 17  // Defina o pino do botão conforme necessário
#define ESC_PIN 18

typedef struct {
    char ID;
    char value;
} laser_t;

void write_package(laser_t data);
int read_n_detect (char port);
int test (char port, char teste1);
void btn_callback(uint gpio, uint32_t events);
void hc06_task(void *p);
void btn_task(void *p);
void btn_esc_task(void *p);
void laserG_task(void *p);
void laserR_task(void *p);
void laserY_task(void *p);
void uart_task(void *p);

extern QueueHandle_t xQueueA;
extern SemaphoreHandle_t xSemaphoreSpace;
extern SemaphoreHandle_t xSemaphoreEsc;

#endif