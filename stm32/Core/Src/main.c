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
#include "network.h"
#include "network_data.h"
#include <string.h>
#include <stdbool.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define N_FEATURES   23
#define AI_OUT_SIZE   1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
static ai_handle network = AI_HANDLE_NULL;
AI_ALIGNED(4) static uint8_t activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];
AI_ALIGNED(4) static float ai_input_data[N_FEATURES];
AI_ALIGNED(4) static float ai_output_data[AI_OUT_SIZE];

static const float scaler_mean[N_FEATURES] = {
    1.444514270390783f, 0.0f, 18.6305f, 53.98397626495361f,
    0.038f, 0.22137468427738974f, 2.6415f, 815.611f,
    17.66314285714286f, 2.648470289504954f, 0.7549757997377643f,
    16.3445f, 3.8682440878323145f, 13.29497680314301f,
    -0.5328049999914531f, 0.6457016388022208f, 282.4710714285714f,
    0.1393975074128913f, 1.260857142857143f, -0.21860787017979394f,
    0.23717709684105856f, 0.7110714285714286f, 0.0017857142857142857f
};

static const float scaler_scale[N_FEATURES] = {
    0.16950741651351103f, 1.0f, 38.566282068755f, 45.498975200490676f,
    0.1911962342725279f, 0.2806538851250894f, 8.310860744919065f,
    782.567686506843f, 26.632251368963107f, 5.575230912285686f,
    6.868268926877113f, 20.869929626311485f, 10.759259482379667f,
    22.844135198547168f, 0.4987177091276681f, 8.411118684866477f,
    815.6787343890646f, 0.14023648300184988f, 1.4887038877177494f,
    2.681336691408281f, 0.4345793852156317f, 0.4532646600396381f,
    0.04221996577690611f
};

static const float test_data[N_FEATURES] = {
		5.0f,      // ENGINE_POWER (too high for idle/small engine → abnormal)
		    1.0f,      // AUTOMATIC (gear stuck / malfunction)
		    50.0f,     // BAROMETRIC_PRESSUREKPA (too low, sensor fault or altitude issue)
		    130.0f,    // ENGINE_COOLANT_TEMP (overheating engine)
		    2.0f,      // FUEL_LEVEL (almost empty)
		    0.95f,     // ENGINE_LOAD (near 100%, suspicious under low speed)
		    -10.0f,    // AMBIENT_AIR_TEMP (impossible, sensor failure)
		    7500.0f,   // ENGINE_RPM (over-revving, unsafe)
		    10.0f,     // INTAKE_MANIFOLD_PRESSURE (too low, vacuum leak)
		    0.1f,      // MAF (too low, airflow sensor issue)
		    -25.0f,    // LONG_TERM_FUEL_TRIM_BANK_2 (out of normal range, fuel mixture fault)
		    120.0f,    // AIR_INTAKE_TEMP (overheated intake, abnormal)
		    800.0f,    // FUEL_PRESSURE (too high, fuel system fault)
		    5.0f,      // SPEED (too low for such high RPM, clutch/transmission slip)
		    30.0f,     // SHORT_TERM_FUEL_TRIM_BANK_2 (too high, mixture imbalance)
		    -20.0f,    // SHORT_TERM_FUEL_TRIM_BANK_1 (too low, mixture imbalance)
		    10000.0f,  // ENGINE_RUNTIME (unrealistic for test, may indicate overflow)
		    99.0f,     // THROTTLE_POS (wide open, but low speed → suspicious)
		    5.0f,      // DTC_NUMBER (too many fault codes)
		    -5.0f,     // TIMING_ADVANCE (negative abnormal ignition timing)
		    5.0f,      // EQUIV_RATIO (extremely rich mixture, not possible normally)
		    0.0f,      // FUEL_Biodiesel_Ethanol (sensor error or no fuel type detected)
		    1.0f       // Extra field (possibly corrupted or sensor glitch)
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
static bool AI_Init(void);
static bool AI_Run(const float *input, float *output);
static void test_sample_and_send_result(void);
static void uart_send(const char *s);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  HAL_Delay(2000);

  if (!AI_Init()) {
    Error_Handler();
  }

  test_sample_and_send_result();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_Delay(1000);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA6 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static bool AI_Init(void)
{
    ai_error err;
    err = ai_network_create(&network, AI_NETWORK_DATA_CONFIG);
    if (err.type != AI_ERROR_NONE) {
        return false;
    }

    const ai_network_params params = {
        AI_NETWORK_DATA_WEIGHTS(ai_network_data_weights_get()),
        AI_NETWORK_DATA_ACTIVATIONS(activations)
    };

    if (!ai_network_init(network, &params)) {
        return false;
    }

    return true;
}

static bool AI_Run(const float *input, float *output)
{
    ai_i32 batch;
    ai_buffer *ai_input = ai_network_inputs_get(network, NULL);
    ai_buffer *ai_output = ai_network_outputs_get(network, NULL);

    if (!ai_input || !ai_output) {
        return false;
    }

    memcpy(ai_input_data, input, N_FEATURES * sizeof(float));
    ai_input[0].data = AI_HANDLE_PTR(ai_input_data);
    ai_output[0].data = AI_HANDLE_PTR(ai_output_data);

    batch = ai_network_run(network, ai_input, ai_output);
    if (batch != 1) {
        return false;
    }

    memcpy(output, ai_output_data, AI_OUT_SIZE * sizeof(float));
    return true;
}

static void uart_send(const char *s)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)s, strlen(s), HAL_MAX_DELAY);
}

static void test_sample_and_send_result(void)
{
    float scaled[N_FEATURES];
    float probability;
    uint8_t fault_status;

    for (int i = 0; i < N_FEATURES; i++) {
        scaled[i] = (test_data[i] - scaler_mean[i]) / scaler_scale[i];
    }

    if (AI_Run(scaled, &probability)) {
        fault_status = (probability >= 0.5f) ? 1 : 0;
        char msg[50];
        snprintf(msg, sizeof(msg), "Fault: %d\r\n", fault_status);
        uart_send(msg);
        if (fault_status == 1) {
                HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);   // LED ON
            } else {
                HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET); // LED OFF
            }
        // Send raw fault status byte
        HAL_UART_Transmit(&huart2, &fault_status, 1, HAL_MAX_DELAY);
    } else {
        uart_send("AI inference failed!\r\n");
    }
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

#ifdef USE_FULL_ASSERT
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
