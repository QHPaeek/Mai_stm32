#include "LED.h"
#include "stdio.h"

#define NUM_LED 16
uint8_t RGB_data_raw[48] = {0xff}; //16LED
uint32_t RGB_data_DMA_buffer[609] = {120};

const uint8_t gamma8[256] = {
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,
  2,   2,   2,   2,   2,   3,   3,   3,   4,   4,   4,   5,   5,   5,   6,   6,
  6,   7,   7,   7,   8,   8,   9,   9,   9,  10,  10,  11,  11,  12,  12,  13,
 13,  14,  14,  15,  15,  16,  16,  17,  17,  18,  19,  19,  20,  20,  21,  22,
 22,  23,  23,  24,  25,  25,  26,  27,  27,  28,  29,  29,  30,  31,  32,  32,
 33,  34,  35,  35,  36,  37,  38,  38,  39,  40,  41,  42,  42,  43,  44,  45,
 46,  46,  47,  48,  49,  50,  51,  52,  53,  53,  54,  55,  56,  57,  58,  59,
 60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,
 76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  87,  88,  89,  90,  91,  92,
 93,  94,  96,  97,  98,  99, 100, 101, 103, 104, 105, 106, 107, 109, 110, 111,
112, 114, 115, 116, 117, 119, 120, 121, 122, 124, 125, 126, 128, 129, 130, 131,
133, 134, 135, 137, 138, 139, 141, 142, 144, 145, 146, 148, 149, 150, 152, 153,
155, 156, 158, 159, 160, 162, 163, 165, 166, 168, 169, 171, 172, 174, 175, 177,
178, 180, 181, 183, 184, 186, 187, 189, 190, 192, 193, 195, 196, 198, 200, 201,
203, 204, 206, 208, 209, 211, 212, 214, 216, 217, 219, 221, 222, 224, 225, 227,
229, 231, 232, 234, 236, 237, 239, 241, 242, 244, 246, 248, 249, 251, 253, 255
};

void LED_set(uint8_t led_no,uint8_t r,uint8_t g,uint8_t b){
	if(led_no >= NUM_LED){
		return;
	}
	RGB_data_raw[led_no * 3] = r;
	RGB_data_raw[led_no * 3 + 1] = g;
	RGB_data_raw[led_no * 3 + 2] = b;
}
void LED_refresh()
{
	//pwm计数�??????120重载，设置为120即一直低电平，设置为90表示ws2812的高，设置为30表示ws2812的低
	//ws2812传输数据顺序要求是GRB,游戏下发数据为RGB,�??????要对调顺�??????
	for(uint8_t i = 0 ;i<16;i++)
	{
		for(uint8_t j = 0 ;j <8;j++)
		{
			RGB_data_DMA_buffer[(i*3)*8+j+224] = (gamma8[RGB_data_raw[i*3+1]] & (1<<j)) ? 90:30;
		}
		for(uint8_t j = 0 ;j <8;j++)
		{
			RGB_data_DMA_buffer[(i*3+1)*8+j+224] = (gamma8[RGB_data_raw[i*3]] & (1<<j)) ? 90:30;
		}
		for(uint8_t j = 0 ;j <8;j++)
		{
			RGB_data_DMA_buffer[(i*3+2)*8+j+224] = (gamma8[RGB_data_raw[i*3+2]] & (1<<j)) ? 90:30;
		}
	}
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)RGB_data_DMA_buffer, 609);
}

