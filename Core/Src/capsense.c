/*
 * capsense.c
 *
 *  Created on: Jan 8, 2025
 *      Author: Qinh
 */
#include "capsense.h"
#include "usart.h"
#include "string.h"
#include <math.h>

#define CAPSENSE_BASELINE_VARIANCE 255 //基线计算所能允许的最大方差，如果超过此数值则不更新基线
#define CAPSENSE_LOW_BASELINE_RESET 500 //低基线复位，如果基线减去raw大于这个数值，则立即以当前的raw作为基线。

extern uint8_t uart_dma_buffer[128];
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

packet_capsense_t Touch;
uint16_t capsense_raw_windows[10][34];
uint8_t capsense_raw_bet = 0;
uint16_t capsense_baseline[34];
uint16_t capsense_threshold[34];
uint8_t capsense_data_ready = 0;
uint8_t capsense_bit;
uint8_t capsense_touch_status[34];

//触摸区块映射实际触摸通道数值
uint8_t touch_sheet[34] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33};

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
//	if(Size != 70){
//		return;
//	}
    if (huart->Instance == USART1)
    {
        HAL_UART_DMAStop(&huart1);
        if((uart_dma_buffer[0] == 0) && (uart_dma_buffer[1] == 0)){
        	memcpy(&Touch.data[0],uart_dma_buffer,70);
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_dma_buffer, 70);
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
    }
    capsense_data_ready = 1;
}

void capsense_init(){
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_dma_buffer, 70);
	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
	//while(!capsense_data_ready);
	for(uint8_t i = 0;i<34;i++){
		capsense_baseline[i] = Touch.channel_raw[i];
	}
	capsense_data_ready = 0;
}

void capsense_baseline_updata(uint8_t channel){
	float average = 0;
	for(uint8_t k = 0;k < 10;k++){
		average += capsense_raw_windows[k][channel]/10;
	}
	float variance = 0;
	for (uint8_t k = 0;k < 10;k++) {
		variance += powf(capsense_raw_windows[k][channel] - average, 2);
	}
	variance /= 10;
	if(variance < CAPSENSE_BASELINE_VARIANCE){
		capsense_baseline[channel] = average;
	}
}

void capsense_check(){
	//while(!capsense_data_ready);
	memcpy(&capsense_raw_windows[capsense_raw_bet][0],&Touch.channel_raw[0],34*2);
	capsense_raw_bet ++;
	if(capsense_raw_bet > 9){
		capsense_raw_bet = 0;
	}
	for(uint8_t i = 0;i<34;i++){
		if((capsense_baseline[i] > Touch.channel_raw[i]) && (capsense_baseline[i] - Touch.channel_raw[i] > CAPSENSE_LOW_BASELINE_RESET)){
			capsense_baseline[i] = Touch.channel_raw[i];
		}else if(capsense_raw_bet == 0){
			capsense_baseline_updata(i);
		}
	}
	for(uint8_t i = 0;i<34;i++){
		if((capsense_baseline[i] < Touch.channel_raw[i]) && (Touch.channel_raw[i] - capsense_baseline[i] > capsense_threshold[i])){
			capsense_touch_status[i] = 1;
		}else{
			capsense_touch_status[i] = 0;
		}
	}
	capsense_data_ready = 0;

}
