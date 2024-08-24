/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usb_otg.h"
#include "usb_device.h"
#include "cy8cmbr3116.h"
#include "serial.h"
#include "button.h"
#include "dma.h"
#include "tim.h"
#include "LED.h"
#include "usbd_cdc_acm_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
	//volatile unsigned long  ulHighFrequencyTimerTicks = 0ul;
	extern USBD_HandleTypeDef hUsbDevice;
	volatile uint8_t touch_scan_flag = 0;
	volatile uint8_t key_scan_flag = 0;
	uint8_t touch_cmd_flag = 0;
	uint8_t led_fade_target[2]; //0:start 1:end
	uint8_t led_fade_buff[3];
	uint8_t led_fade_flag = 0;
	uint16_t led_fade_time;
	uint8_t led_refresh_flag = 0;
	uint8_t heart_beat = 255;
	char cmd_tmp[6] = "(RSET)";
	char cmd_rst[6] = "(RSET)";
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task02 */
osThreadId_t Task02Handle;
const osThreadAttr_t Task02_attributes = {
  .name = "Task02",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task03 */
osThreadId_t Task03Handle;
const osThreadAttr_t Task03_attributes = {
  .name = "Task03",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task04 */
osThreadId_t Task04Handle;
const osThreadAttr_t Task04_attributes = {
  .name = "Task04",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);
void StartTask04(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Task02 */
  Task02Handle = osThreadNew(StartTask02, NULL, &Task02_attributes);

  /* creation of Task03 */
  Task03Handle = osThreadNew(StartTask03, NULL, &Task03_attributes);

  /* creation of Task04 */
  Task04Handle = osThreadNew(StartTask04, NULL, &Task04_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  //mai_touch
	osDelay(8);
	Sensor_Cfg(&hi2c1);
	Sensor_Cfg(&hi2c2);
	Sensor_Cfg(&hi2c3);
	Sensor_softRST(&hi2c1);
	Sensor_softRST(&hi2c2);
	Sensor_softRST(&hi2c3);
	memset(key_threshold,64,35);
	while(1)
	{
		osDelay(5);
		key_scan();
		uint8_t cmd_touch[9] = {0x28,0,0,0,0,0,0,0,0x29};
		for(uint8_t j = 0;j<7;j++){
			for(uint8_t i = 0;i<5;i++){
				if(j == 7 && i == 4){
					break;
					//没有�????35个触摸点
				}
				if(key_status[key_sheet[i+j*5]] > key_threshold[i+j*5]){
					cmd_touch[j+1] = cmd_touch[7-j] | (1 << i);
				}
			}
		}
		if(touch_cmd_flag == 1){
			CDC_Transmit(0,(uint8_t*)cmd_tmp, 6);
			touch_cmd_flag = 0;
		}
		else if(touch_scan_flag == 1){
			CDC_Transmit(0,(uint8_t*)cmd_touch, 9);
		}

	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the Task02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
	//mai_key
	while(1){
		osDelay(5);
		uint8_t key_cmd[6] = {0xff,0x01,0x02,0x00,0x00,0x00};
		button_scan(&key_cmd[3]);
		key_cmd[5] = 0x01+0x02+key_cmd[3]+key_cmd[4]; // checksum
		CDC_Transmit(2,key_cmd, 6);
	}

  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the Task03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
//	uint8_t led_flag = 0;
//	uint8_t led_flag_len = 0;
	uint8_t mai_led_default_response[8] = {0xe0,0x01,0x11,0x03,0x01,0x31,0x01,0x48};
	uint8_t mai_led_eeprom_response[9] = {0xe0,0x01 ,0x11,0x04,0x01,0x7c,0x01,0x00,0x94};
	uint8_t mai_led_boardinfo_response[18] = {0xe0,0x01,0x11,0x0d,0x01,0xf0,0x01,0x31,0x35,0x30,0x37,0x30,0x2d,0x30,0x34,0xff,0x90,0x2e};
	uint8_t mai_led_boardstatus_response[12] = {0xe0,0x01,0x11,0x07,0x01,0xf1,0x01,00,00,00,00,0x0c};
	uint8_t mai_led_protocolversion_response[11] = {0xe0,0x01,0x11,0x06,0x01,0xf3,0x01,0x01,0x00,0x00,0x0e};
  for(;;)
  {
	osDelay(1);
	if (rxLen0 != 0){
		//maitouch
			if(rxBuffer0[0] == 0x7B){
				switch (rxBuffer0[1]){
					case 0x53:
						//{STAT}
						touch_scan_flag = 1;
						//cmd_tmp = "(STAT)";
						//CDC_Transmit(0,(uint8_t*)cmd_tmp,6);
						break;
					case 0x48:
						//{HALT}
						touch_scan_flag = 0;
						break;
					case 0x52:
						touch_scan_flag = 0;
						touch_cmd_flag = 1;
						if(rxBuffer0[3] == 0x45){
							//{RSET}
							memcpy(cmd_tmp+1,cmd_rst+1,4);
							break;
						}else if(rxBuffer0[3] == 0x72){
							//Set Touch Panel Ratio
							//TODO
							memcpy(cmd_tmp+1,rxBuffer0+1,4);
							break;
						}else if(rxBuffer0[3] == 0x6b){
							//Set Touch Panel Sensitivity
							key_threshold[rxBuffer0[2] - 0x41] = rxBuffer0[4] + 40;
							memcpy(cmd_tmp+1,rxBuffer0+1,4);
							break;
						}
					case 0x4c:
						touch_scan_flag = 0;
						touch_cmd_flag = 1;
						if(rxBuffer0[3] == 0x45){
							//{RSET}
							//CDC_Transmit(0,(uint8_t*)cmd_tmp,6);
							break;
						}else if(rxBuffer0[3] == 0x72){
							//Set Touch Panel Ratio
							//TODO
							memcpy(cmd_tmp+1,rxBuffer0+1,4);
							break;
						}else if(rxBuffer0[3] == 0x6b){
							//Set Touch Panel Sensitivity
							key_threshold[rxBuffer0[2] - 0x41] = rxBuffer0[4] + 40;
							memcpy(cmd_tmp+1,rxBuffer0+1,4);
							break;
						}
					default:
						break;
				}
			}
			rxLen0 = 0;
		}
		if (rxLen1 != 0){
			//mailed
			if(rxBuffer1[0] == 0xE0){
				switch(rxBuffer1[4]){
					case 0x31:
						//setLedGs8Bit
						LED_set(2*rxBuffer1[5],rxBuffer1[6],rxBuffer1[7],rxBuffer1[8]);
						LED_set(2*rxBuffer1[5]+1,rxBuffer1[6],rxBuffer1[7],rxBuffer1[8]);
						mai_led_default_response[5] = 0x31;
						mai_led_default_response[6] = 0x48;
						while(CDC_Transmit(1,mai_led_default_response,8) != USBD_OK);
						break;
					case 0x32:
						//setLedGs8BitMulti
						for(uint8_t i = rxBuffer1[5];i < rxBuffer1[6];i++){
							LED_set(2*i,rxBuffer1[8],rxBuffer1[9],rxBuffer1[10]);
							LED_set(2*i+1,rxBuffer1[8],rxBuffer1[9],rxBuffer1[10]);
						}
						mai_led_default_response[5] = 0x32;
						mai_led_default_response[6] = 0x49;
						while(CDC_Transmit(1,mai_led_default_response,8) != USBD_OK);
						break;
					case 0x33:
						//setLedGs8BitMultiFade4
						memcpy(led_fade_target,rxBuffer1 + 5,2);
						memcpy(led_fade_buff,rxBuffer1 + 8,3);
						led_fade_flag = 1;
						led_fade_time = 4095/rxBuffer1[9]*8;
						mai_led_default_response[5] = 0x33;
						mai_led_default_response[6] = 0x4a;
						while(CDC_Transmit(1,mai_led_default_response,8) != USBD_OK);
						break;
					case 0x39:
						//setLedFet
						//BodyLed ExtLed SideLed
						__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,rxBuffer1[5]);
						__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,rxBuffer1[6]);
						__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,rxBuffer1[7]);
						mai_led_default_response[5] = 0x39;
						mai_led_default_response[6] = 0x50;
						while(CDC_Transmit(1,mai_led_default_response,8) != USBD_OK);
						break;
					case 0x3c:
						//SetLedGsUpdate
						led_refresh_flag = 1;
						mai_led_default_response[5] = 0x3c;
						mai_led_default_response[6] = 0x53;
						while(CDC_Transmit(1,mai_led_default_response,8) != USBD_OK);
						break;
					case 0x7c:
						//GetEEPRom
						while(CDC_Transmit(1,mai_led_eeprom_response,9) != USBD_OK);
						break;
					case 0xf0:
						//getBoardInfo
						while(CDC_Transmit(1,mai_led_boardinfo_response,18) != USBD_OK);
						break;
					case 0xf1:
						//getBoardStatus
						while(CDC_Transmit(1,mai_led_boardstatus_response,12) != USBD_OK);
						break;
					case 0xf3:
						//getProtocolVersion
						while(CDC_Transmit(1,mai_led_protocolversion_response,11) != USBD_OK);
						break;
				}
			}
			rxLen1 = 0;
		}
		if (rxLen2 != 0){
		//maikey
			if(rxBuffer2[0] == 0xFF){
				switch(rxBuffer2[1]){
					case 0x03:
						//SERIAL_CMD_AUTO_SCAN_START
						key_scan_flag = 1;
						break;
					case 0x04:
						//SERIAL_CMD_AUTO_SCAN_STOP
						key_scan_flag = 0;
						break;
					case 0x11:
						//SERIAL_CMD_HEART_BEAT
						heart_beat = 255;
						break;
					default:
						break;
				}
			}
			rxLen2 = 0;
		}
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the Task04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void *argument)
{
  /* USER CODE BEGIN StartTask04 */
	uint8_t red,green,blue;
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,0);
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,0);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,0);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
//	memset(RGB_data_raw,128,48);
//	LED_refresh();
//	osDelay(1000);
//	for(uint8_t i=0;i<8;i++){
//		LED_set(2*i,255,0,0);
//		LED_set(2*i+1,255,0,0);
//		LED_refresh();
//		osDelay(1000);
//	}
//	for(uint8_t i=0;i<8;i++){
//		LED_set(2*i,0,255,0);
//		LED_set(2*i+1,0,255,0);
//		LED_refresh();
//		osDelay(1000);
//	}
//	for(uint8_t i=0;i<8;i++){
//		LED_set(2*i,0,0,255);
//		LED_set(2*i+1,0,0,255);
//		LED_refresh();
//		osDelay(1000);
//	}
	while(1){
		if(led_fade_flag){
			if(led_fade_time <= 10){
				red = led_fade_buff[0];
				green = led_fade_buff[1];
				blue = led_fade_buff[2];
				led_fade_flag = 0;
			}else{
				red = RGB_data_raw[led_fade_target[0] * 3] + (led_fade_buff[0] - RGB_data_raw[led_fade_target[0] * 3])/(led_fade_time / 10);
				green = RGB_data_raw[led_fade_target[0] * 3 + 1] + (led_fade_buff[1] - RGB_data_raw[led_fade_target[0] * 3 + 1])/(led_fade_time / 10);
				blue = RGB_data_raw[led_fade_target[0] * 3 + 2] + (led_fade_buff[2] - RGB_data_raw[led_fade_target[0] * 3 + 2])/(led_fade_time / 10);
				led_fade_time -= 10;
			}
			for(uint8_t i = led_fade_target[0];i < led_fade_target[1];i++){
				LED_set(2*i,red,green,blue);
				LED_set(2*i+1,red,green,blue);
			}
			LED_refresh();
		}else if(led_refresh_flag){
			led_refresh_flag = 0;
			LED_refresh();
		}
		if(heart_beat == 0){
			for(uint8_t j=0;j<16;j++){
				LED_set(j,0,0,0);
			}
			LED_refresh();
		}else{
			heart_beat --;
		}
		osDelay(10);
	}

  /* USER CODE END StartTask04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

