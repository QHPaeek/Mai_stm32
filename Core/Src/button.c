/*
 * button.c
 *
 *  Created on: Aug 1, 2024
 *      Author: Qinh
 */
#include "button.h"
uint8_t button_scan(){
	uint8_t key_tmp = 0;
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)){
		key_tmp = key_tmp | 1;
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)){
		key_tmp = key_tmp | 1 << 1;
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)){
		key_tmp = key_tmp | 1 << 2;
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)){
		key_tmp = key_tmp | 1 << 3;
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)){
		key_tmp = key_tmp | 1 << 4;
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6)){
		key_tmp = key_tmp | 1 << 5;
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)){
		key_tmp = key_tmp | 1 << 6;
	}
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)){
		key_tmp = key_tmp | 1 << 7;
	}
	return key_tmp;
}

