/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>//使用printf函数

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

/* USER CODE BEGIN PV */
uint32_t start_time=0;//记录超声波回响开始的时间（单位：定时器计数值）
uint32_t end_time=0;//记录超声回响结束时间（单位：定时器计数值）

float pulse_width=0.0f;//计算出的超声波高电平脉宽时间（单位：微妙s）
float distance=0.0f;//根据脉宽计算出距离值（单位：厘米）



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void CS100A_TRIG_Start(void)
{
	HAL_GPIO_WritePin(GPIOA,CS100A_TRIG_Pin,GPIO_PIN_SET);
	
	for(uint16_t i=0; i<90; i++);//简单延时，用于产生大约10微妙的脉冲宽度（72MHz）
	
  HAL_GPIO_WritePin(GPIOA,CS100A_TRIG_Pin,GPIO_PIN_RESET);

}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		//清零定时器计数CNT
		__HAL_TIM_SET_COUNTER(&htim2, 0);
		HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_1);//下降沿
		HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_2);//上升沿
		
		
		CS100A_TRIG_Start();//发送超声波触发信号
		
		while (__HAL_TIM_GET_FLAG (&htim2,TIM_FLAG_CC2)==RESET);
		
		//清除上升沿中断标志位
		__HAL_TIM_CLEAR_FLAG(&htim2,TIM_FLAG_CC2);
		
		
		start_time=HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2);//读取通道2捕获的上升沿时间（起始时间）
		
				while (__HAL_TIM_GET_FLAG (&htim2,TIM_FLAG_CC1)==RESET);
		
		//清除上升沿中断标志位
		__HAL_TIM_CLEAR_FLAG(&htim2,TIM_FLAG_CC1);
		
		
		end_time=HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_1);//读取通道1捕获的下降沿时间（结束时间）
		
		
		//停止捕获（为避免干扰下次采样）
		HAL_TIM_IC_Stop(&htim2,TIM_CHANNEL_1);
		HAL_TIM_IC_Stop(&htim2,TIM_CHANNEL_2);
		
		uint32_t delta=end_time-start_time;//计算计数差
		
		pulse_width=(float)delta *1e-6f;//换算成脉宽（单位s）
		distance=pulse_width*340.0f/2.0f;//根据声速换算成距离、
		printf("当前距离:%.2f m\r\n",distance);
		
		HAL_Delay(500);//500ms测量一次
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

int fputc(int ch,FILE*f)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)&ch, 1, HAL_MAX_DELAY);
	return ch;

}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
