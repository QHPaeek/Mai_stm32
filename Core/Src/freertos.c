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
	uint8_t touch_scan_flag = 0;
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
  /* Infinite loop */
	osDelay(8);
	Sensor_Cfg(&hi2c1);
	Sensor_Cfg(&hi2c2);
	Sensor_Cfg(&hi2c3);
	while(1)
	{
		osDelay(5);
		key_scan();
		char cmd_touch[9] = "(0000000)";
		for(uint8_t j = 0;j<8;j++){
			for(uint8_t i = 0;i<5;i++){
				if(key_status[key_sheet[i+j*5]] > key_threshold){
					cmd_touch[8-j] = cmd_touch[8-j] | (1 << (5-i));
				}else{
					cmd_touch[8-j] = cmd_touch[8-j] & ~(1 << (5-i));
				}
			}
		}
		if(touch_scan_flag){
			CDC_Transmit(0, cmd_touch, 9);
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
	osDelay(1000);

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
  for(;;)
  {
	osDelay(5);
	if (rxLen0 != 0){
		//maitouch
		if(rxBuffer0[0] == 0x7B){
			char cmd_tmp[6] = "(RSET)";
			switch (rxBuffer0[1]){
				case 0x53:
					//{STAT}
					touch_scan_flag = 1;
				case 0x48:
					//{HALT}
					touch_scan_flag = 0;
				case 0x52:
					if(rxBuffer0[3] == 0x45){
						//{RSET}
						CDC_Transmit(0,cmd_tmp,6);
					}else if(rxBuffer0[3] == 0x72){
						//Set Touch Panel Ratio
						//todo
						memcpy(cmd_tmp+1,rxBuffer0+1,4);
						CDC_Transmit(0,cmd_tmp,6);
					}else if(rxBuffer0[3] == 0x6b){
						//Set Touch Panel Sensitivity
						//todo
						memcpy(cmd_tmp+1,rxBuffer0+1,4);
						CDC_Transmit(0,cmd_tmp,6);
					}
				default:
					break;
			}
    	}
	  }
	if (rxLen1 != 0){
		//mailed
	  }
	if (rxLen2 != 0){
		//maikey
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
	//pwm计数�?????120重载，设置为120即一直低电平，设置为90表示ws2812的高，设置为30表示ws2812的低
	//ws2812传输数据顺序要求是GRB,游戏下发数据为RGB,�?????要对调顺�?????
//	RGB_data_DMA_buffer[969] = 120;
//	RGB_Air_DMA_buffer[591] = 120;
//	for(;;){
//		if(led_count == 0){
//			system_status = 0;
//			slider_scan_flag = 0;
//			Air_scan_flag = 0;
//			//减到0：没有接到指令，说明游戏已经�?出，切换到模�?0
//		}else{
//			led_count--;
//		}
//		if(system_status == 0){
//			for(uint8_t i = 0 ;i<31;i++)
//		    {
//		    	for(uint8_t j = 0 ;j <8;j++)
//		    	{
//		    		RGB_data_DMA_buffer[(i*3)*8+j+224] = 30;
//		    	}
//		    	for(uint8_t j = 0 ;j <8;j++)
//		    	{
//		    		RGB_data_DMA_buffer[(i*3+1)*8+j+224] = 30;
//		    	}
//		    	for(uint8_t j = 0 ;j <8;j++)
//		    	{
//		    		RGB_data_DMA_buffer[(i*3+2)*8+j+224] = 90;
//		    	}
//		    }
//			for(uint8_t i = 0 ;i<16;i++)
//		    {
//		    	for(uint8_t j = 0 ;j <8;j++)
//		    	{
//		    		RGB_data_DMA_buffer[(i*6+1)*8+j+224] = ((key_status[key_sheet[2*i]] > 128)|(key_status[key_sheet[2*i+1]] > 128)) ? 90:30;
//		    	}
//		    }
//
//		}else{
//			for(uint8_t i = 0 ;i<31;i++)
//		    {
//		    	for(uint8_t j = 0 ;j <8;j++)
//		    	{
//		    		RGB_data_DMA_buffer[(i*3)*8+j+224] = (RGB_data_raw[4+i*3+2] & (1<<j)) ? 90:30;
//		    	}
//		    	for(uint8_t j = 0 ;j <8;j++)
//		    	{
//		    		RGB_data_DMA_buffer[(i*3+1)*8+j+224] = (RGB_data_raw[4+i*3+1] & (1<<j)) ? 90:30;
//		    	}
//		    	for(uint8_t j = 0 ;j <8;j++)
//		    	{
//		    		RGB_data_DMA_buffer[(i*3+2)*8+j+224] = (RGB_data_raw[4+i*3] & (1<<j)) ? 90:30;
//		    	}
//		    }
//		}
//	    for(uint8_t i = 0 ;i<6;i++)
//	    {
//	    	if(Air_key_buffer & (1<<i)){
//	    		for(uint8_t j = 0 ;j <8;j++)
//	    		{
//	    			RGB_Air_DMA_buffer[(i*3)*8+j+224] = 90;
//	    		}
//	    		for(uint8_t j = 0 ;j <8;j++)
//	    		{
//	    			RGB_Air_DMA_buffer[(i*3+1)*8+j+224] =90;
//	    		}
//	    		for(uint8_t j = 0 ;j <8;j++)
//	    		{
//	    			RGB_Air_DMA_buffer[(i*3+2)*8+j+224] =90;
//	    		}
//	    	}
//	    	else{
//	    		for(uint8_t j = 0 ;j <8;j++)
//	    		{
//	    			RGB_Air_DMA_buffer[(i*3)*8+j+224] = 30;
//	    		}
//	    		for(uint8_t j = 0 ;j <8;j++)
//	    		{
//	    			RGB_Air_DMA_buffer[(i*3+1)*8+j+224] =30;
//	    		}
//	    		for(uint8_t j = 0 ;j <8;j++)
//	    		{
//	    			RGB_Air_DMA_buffer[(i*3+2)*8+j+224] =90;
//	    		}
//	    	}
//	    }
//	    //�?????启DMA传输刷灯
//		HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)RGB_data_DMA_buffer, 970);
//		HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_3, (uint32_t *)RGB_Air_DMA_buffer, 592);
		osDelay(50);
//	  }
  /* USER CODE END StartTask04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

