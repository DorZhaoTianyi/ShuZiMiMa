/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "adc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_CHANNEL_COUNT 12
#define SAMPLE_BUFFER_SIZE 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t ADC_Value1 = 0;
uint16_t ADC_Value2 = 0;
uint16_t ADC_Value3 = 0;
uint16_t ADC_Value4 = 0;
uint16_t ADC_Value5 = 0;
uint16_t ADC_Value6 = 0;
uint16_t ADC_Value7 = 0;
uint16_t ADC_Value8 = 0;
uint16_t ADC_Value9 = 0;
uint16_t ADC_Value10 = 0;
uint16_t ADC_Value11 = 0;
uint16_t ADC_Value12 = 0;
uint8_t uart_buffer[26]; // 用于存储发送的数据
// 新增的数组和计数器
uint16_t adc_values[ADC_CHANNEL_COUNT][SAMPLE_BUFFER_SIZE];
uint8_t sample_count[ADC_CHANNEL_COUNT] = {0};
uint8_t buffer_full_flag = 0; // 缓冲区满标志
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint16_t get_max_value(uint16_t *array, uint8_t size);
void Process_ADC_Data(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Send_UART_Data(void)
{
    uart_buffer[0] = 0x0A; // 起始帧头
    uart_buffer[1] = (uint8_t)((ADC_Value1 >> 8) & 0xFF); // 高字节
    uart_buffer[2] = (uint8_t)(ADC_Value1 & 0xFF);       // 低字节
    uart_buffer[3] = (uint8_t)((ADC_Value2 >> 8) & 0xFF);
    uart_buffer[4] = (uint8_t)(ADC_Value2 & 0xFF);
    uart_buffer[5] = (uint8_t)((ADC_Value3 >> 8) & 0xFF);
    uart_buffer[6] = (uint8_t)(ADC_Value3 & 0xFF);
    uart_buffer[7] = (uint8_t)((ADC_Value4 >> 8) & 0xFF);
    uart_buffer[8] = (uint8_t)(ADC_Value4 & 0xFF);
    uart_buffer[9] = (uint8_t)((ADC_Value5 >> 8) & 0xFF);
    uart_buffer[10] = (uint8_t)(ADC_Value5 & 0xFF);
    uart_buffer[11] = (uint8_t)((ADC_Value6 >> 8) & 0xFF);
    uart_buffer[12] = (uint8_t)(ADC_Value6 & 0xFF);
    uart_buffer[13] = (uint8_t)((ADC_Value7 >> 8) & 0xFF);
    uart_buffer[14] = (uint8_t)(ADC_Value7 & 0xFF);
    uart_buffer[15] = (uint8_t)((ADC_Value8 >> 8) & 0xFF);
    uart_buffer[16] = (uint8_t)(ADC_Value8 & 0xFF);
    uart_buffer[17] = (uint8_t)((ADC_Value9 >> 8) & 0xFF);
    uart_buffer[18] = (uint8_t)(ADC_Value9 & 0xFF);
    uart_buffer[19] = (uint8_t)((ADC_Value10 >> 8) & 0xFF);
    uart_buffer[20] = (uint8_t)(ADC_Value10 & 0xFF);
    uart_buffer[21] = (uint8_t)((ADC_Value11 >> 8) & 0xFF);
    uart_buffer[22] = (uint8_t)(ADC_Value11 & 0xFF);
    uart_buffer[23] = (uint8_t)((ADC_Value12 >> 8) & 0xFF);
    uart_buffer[24] = (uint8_t)(ADC_Value12 & 0xFF);
    uart_buffer[25] = 0x0B; // 最终帧尾

    HAL_UART_Transmit(&huart3, uart_buffer, 26, HAL_MAX_DELAY); // 发送数据
		HAL_Delay(500);
}

uint16_t get_max_value(uint16_t *array, uint8_t size)
{
    uint16_t max_val = 0;
    for(uint8_t i = 0; i < size; i++)
    {
        if(array[i] > max_val)
        {
            max_val = array[i];
        }
    }
    return max_val;
}

void Process_ADC_Data(void)
{
    // 读取所有ADC通道
    uint16_t current_values[ADC_CHANNEL_COUNT] = {
        ADC_Read(ADC_CHANNEL_0),
        ADC_Read(ADC_CHANNEL_1),
        ADC_Read(ADC_CHANNEL_2),
        ADC_Read(ADC_CHANNEL_3),
        ADC_Read(ADC_CHANNEL_4),
        ADC_Read(ADC_CHANNEL_5),
        ADC_Read(ADC_CHANNEL_6),
        ADC_Read(ADC_CHANNEL_7),
        ADC_Read(ADC_CHANNEL_8),
        ADC_Read(ADC_CHANNEL_9),
        ADC_Read(ADC_CHANNEL_10),
        ADC_Read(ADC_CHANNEL_11)
    };
    
    // 将每个通道的值存入对应的数组
    for(uint8_t ch = 0; ch < ADC_CHANNEL_COUNT; ch++)
    {
        adc_values[ch][sample_count[ch]] = current_values[ch];
        sample_count[ch]++;
        
        // 如果数组已满，计算最大值并重置计数器
        if(sample_count[ch] >= SAMPLE_BUFFER_SIZE)
        {
            // 设置缓冲区满标志
            buffer_full_flag = 1;
            
            // 计算最大值并赋给对应的ADC_Value变量
            switch(ch)
            {
                case 0: ADC_Value1 = get_max_value(adc_values[0], SAMPLE_BUFFER_SIZE); break;
                case 1: ADC_Value2 = get_max_value(adc_values[1], SAMPLE_BUFFER_SIZE); break;
                case 2: ADC_Value3 = get_max_value(adc_values[2], SAMPLE_BUFFER_SIZE); break;
                case 3: ADC_Value4 = get_max_value(adc_values[3], SAMPLE_BUFFER_SIZE); break;
                case 4: ADC_Value5 = get_max_value(adc_values[4], SAMPLE_BUFFER_SIZE); break;
                case 5: ADC_Value6 = get_max_value(adc_values[5], SAMPLE_BUFFER_SIZE); break;
                case 6: ADC_Value7 = get_max_value(adc_values[6], SAMPLE_BUFFER_SIZE); break;
                case 7: ADC_Value8 = get_max_value(adc_values[7], SAMPLE_BUFFER_SIZE); break;
                case 8: ADC_Value9 = get_max_value(adc_values[8], SAMPLE_BUFFER_SIZE); break;
                case 9: ADC_Value10 = get_max_value(adc_values[9], SAMPLE_BUFFER_SIZE); break;
                case 10: ADC_Value11 = get_max_value(adc_values[10], SAMPLE_BUFFER_SIZE); break;
                case 11: ADC_Value12 = get_max_value(adc_values[11], SAMPLE_BUFFER_SIZE); break;
            }
            
            sample_count[ch] = 0; // 重置计数器重新开始采集
        }
    }
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
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start(&hadc1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		HAL_ADC_Start(&hadc1);//启动一次转换
		
		// 处理ADC数据（采集并检查是否需要计算最大值）
		Process_ADC_Data();
		
		HAL_ADC_Stop(&hadc1);
		
		// 只有当所有通道都采集了10个数据后才发送
		if(buffer_full_flag)
		{
			Send_UART_Data(); // 发送数据
			buffer_full_flag = 0; // 清除标志
		}
		
		HAL_Delay(10);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
