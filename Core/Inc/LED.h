#ifndef _LED_H
#define _LED_H

#include "stdio.h"
#include "dma.h"
#include "tim.h"

extern uint8_t RGB_data_raw[48];
extern uint32_t RGB_data_DMA_buffer[609];

void LED_set(uint8_t led_no,uint8_t r,uint8_t g,uint8_t b);
void LED_refresh();
#endif
