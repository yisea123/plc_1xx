/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "arm_math.h"
#include "math.h"
#include <stdint.h>
#include "task.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_tim3_ch4_up;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim7;
UART_HandleTypeDef huart3;
SPI_HandleTypeDef hspi2;

osThreadId defaultTaskHandle;


xQueueHandle q;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM7_Init(void);
static void MX_SPI2_Init(void);
void DMA1_Channel1_IRQHandler(void);
static void MX_USART3_UART_Init(void);
void MakeRMS_Task(void const * argument);
void AverageBufferQueue_Task(void const * argument);
void vApplicationIdleHook( void );
void Ext_ADC_Task(void const * argument);
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );
void UART_Task(void const * argument);
void Flash_Task(void const * argument);




volatile uint32_t raw_adc_value[600];
volatile float32_t float_adc_value[600];
volatile float32_t rms_out = 0;
volatile uint64_t cpu_load = 0;
volatile uint32_t count_idle;
volatile uint32_t value = 0;
volatile uint32_t freeHeapSize = 0;
uint64_t xTimeBefore, xTotalTimeSuspended;

volatile uint64_t temp1 = 0;
volatile uint64_t temp2 = 0;
volatile uint64_t temp3 = 0;
volatile uint64_t temp4 = 0;

xSemaphoreHandle Semaphore1, Semaphore2, Semaphore3;





/* USER CODE END 0 */

int main(void)
{


  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();



  /* Configure the system clock */
  SystemClock_Config();



  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM7_Init();
	MX_SPI2_Init();
	MX_USART3_UART_Init();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	//HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &adc_value, 600);
	
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start_IT(&htim7);
	
	
	q = xQueueCreate(8, sizeof(float32_t));	
	
	vSemaphoreCreateBinary(Semaphore1);
	vSemaphoreCreateBinary(Semaphore2);
	vSemaphoreCreateBinary(Semaphore3);
	
	
	//////////////////////////////////////////////////////////////////////
	
  
	
  
	
  osThreadDef(Task1, MakeRMS_Task, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(Task1), NULL);
	
	osThreadDef(Task2, AverageBufferQueue_Task, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(Task2), NULL);
	
	osThreadDef(Task3, Ext_ADC_Task, osPriorityAboveNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(Task3), NULL);
	
	osThreadDef(Task4, UART_Task, osPriorityAboveNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(Task4), NULL);
	
	osThreadDef(Task5, Flash_Task, osPriorityAboveNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(Task5), NULL);
 

  /* Start scheduler */
  osKernelStart();
  
  

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1);

  

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_1);

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 64;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 400;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM7 init function */
static void MX_TIM7_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 64000;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 1000;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);


}

/** Configure pins
     PA8   ------> RCC_MCO
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
//	GPIO_InitStruct.Pin = GPIO_PIN_0;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//	
//	GPIO_InitStruct.Pin = GPIO_PIN_1;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		


}


/* SPI2 init function */
static void MX_SPI2_Init(void)
{

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}



void MakeRMS_Task(void const * argument)
{
	
	volatile float32_t all_rms = 0;
		
  for(;;)
  {		
				xSemaphoreTake( Semaphore1, portMAX_DELAY );

				for (uint16_t i=0; i<600; i++)
						float_adc_value[i] = (float32_t) raw_adc_value[i];
			
				arm_rms_f32( (float32_t*)&float_adc_value, 600, (float32_t*)&all_rms );				
				
				xQueueSend(q, (void*)&all_rms, 0);				
								
				all_rms = 0;						
												
				xSemaphoreGive( Semaphore2 );		

  }  
	
}



void AverageBufferQueue_Task(void const * argument)
{
	
  uint8_t queue_count;
	volatile float32_t qrms;
	volatile float32_t qrms_array[8];	
	
	
  for(;;)
  {		
			xSemaphoreTake( Semaphore2, portMAX_DELAY );
			
			queue_count = uxQueueMessagesWaiting(q);		
			
			if (queue_count == 8)
			{			
					rms_out = 0.0;		
								
					for (uint16_t i=0; i<8; i++)
					{
							xQueueReceive(q, (void *) &qrms, 0);		
							qrms_array[i] = qrms;												
					}
					
					arm_rms_f32((float32_t*) &qrms_array, 8, (float32_t*)&rms_out);
					
					xTotalTimeSuspended = xTaskGetTickCount() - xTimeBefore;
					xTimeBefore = xTaskGetTickCount();			

					HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_12);					
					
			}
  }
}


void Ext_ADC_Task(void const * argument)
{
	
	volatile uint8_t value1 = 0;		
	volatile uint8_t value2 = 0;		
	volatile uint8_t value3 = 0;		
	
	volatile uint16_t cnt = 0;
	
	
  for(;;)
  {		
				
		xSemaphoreTake( Semaphore3, portMAX_DELAY );		
				
		taskENTER_CRITICAL();
						
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);				
		HAL_SPI_Receive(&hspi2, (uint8_t*)&value1, sizeof(value1), 1);
		HAL_SPI_Receive(&hspi2, (uint8_t*)&value2, sizeof(value2), 1);
		HAL_SPI_Receive(&hspi2, (uint8_t*)&value3, sizeof(value3), 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
						
		taskEXIT_CRITICAL();
		
		value = value1;
		value = (value << 8) | value2;
		value = (value << 8) | value3;
		value = value >>2;		
				
		if (cnt > 600) 
		{
			cnt = 0;
			
			xSemaphoreGive( Semaphore1 );
		}
		
		raw_adc_value[cnt] = (uint32_t) value;
		
		cnt++;
	}
	
}


void UART_Task(void const * argument)
{
	
	for(;;)
	{
		//HAL_UART_Transmit(&huart3, receiveBuffer, 32, 1);
		//HAL_UART_Receive(&huart3, receiveBuffer, 32, 1);
		//osDelay(10);
		
		FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError = 0;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = 0x08010000;
    EraseInitStruct.NbPages     = 1;

    HAL_FLASH_Unlock();   
		
    HAL_FLASHEx_Erase(&EraseInitStruct,&PAGEError);   
		
		for(uint32_t i = 0; i<128; i++)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x08010000 + i*4, 4);   
    
    HAL_FLASH_Lock();   
		
		osDelay(100);
		
	}
	
}

uint32_t FLASH_Read(uint32_t address)
{
    return (*(__IO uint32_t*)address);
}


void Flash_Task(void const * argument)
{
	uint32_t temp;
	uint32_t receiveBuffer[64];
	
	
	for(;;)
	{
		
		
		for(uint32_t i = 0; i<64; i++)
		{ 			
			receiveBuffer[i] = FLASH_Read (0x08010000 + i*4);
		}
    
		
		osDelay(10);
		
	}
	
}

void vApplicationIdleHook( void )
{	
	count_idle++;
	
	freeHeapSize = xPortGetFreeHeapSize();
	
}

void DMA1_Channel1_IRQHandler(void)
{
  
  HAL_DMA_IRQHandler(&hdma_adc1);
  		
	
//	static portBASE_TYPE xHigherPriorityTaskWoken;
//	xHigherPriorityTaskWoken = pdFALSE;	
//	xSemaphoreGiveFromISR(Semaphore1, &xHigherPriorityTaskWoken);
//	if( xHigherPriorityTaskWoken == pdTRUE )
//  {
//			portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
//	}
	
  
}

void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	for(;;);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
/* USER CODE BEGIN Callback 0 */

/* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }

	if (htim->Instance == TIM3) 
	{			
				
			if( Semaphore3 != NULL )
			{
					static signed portBASE_TYPE xHigherPriorityTaskWoken;
					xHigherPriorityTaskWoken = pdFALSE;	
					xSemaphoreGiveFromISR(Semaphore3, &xHigherPriorityTaskWoken);
					if( xHigherPriorityTaskWoken == pdTRUE )
					{
							portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
					}
			}			
  }
	
	if (htim->Instance == TIM7) 
	{
				
		temp1 = count_idle; 
		count_idle = 0;
		
		cpu_load = 100 - (100 * temp1 / 669505);
  }

/* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
