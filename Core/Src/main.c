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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "DO24_Code.h"
#include "Compile_Data.h"
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
ADC_HandleTypeDef hadc1;

IWDG_HandleTypeDef hiwdg;

UART_HandleTypeDef hlpuart1;
DMA_HandleTypeDef hdma_lpuart1_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

PCD_HandleTypeDef hpcd_USB_DRD_FS;

/* USER CODE BEGIN PV */

uint8_t SUB_UART_TX_buf[SUB_UART_buf_len] ;
uint8_t SUB_UART_RX_buf[SUB_UART_buf_len];
uint8_t SUB_UART_Tx_buf_tmp[SUB_UART_buf_len];
uint8_t SUB_UART_RX_buf_tmp[SUB_UART_buf_len];
uint8_t SUB_UART_State ;					// Define reception start state
int SUB_UART_buf_count;					// Save the length of received data after starting reception
int SUB_UART_buf_count_tmp;				// Variable to store the length of the received data before initializing it when reception is completed
uint8_t SUB_UART_Receive_complete;		// Variable indicating that reception has been completed
int SUB_UART_buf_count_Save;

uint32_t AC_FAULT_CNT ;
float MAIN_ADC, SOURCE_24_ADC, BAT_CVH_ADC, BAT_CVL_ADC;

int Bat_Charge_Mode;
int Bat_Charge_Compensation_Volt;
int Immobility_Chargr_Table[2]; //1:On 0: Off
int Compensation_Chargr_Table[5][2]; //1:On 0: Off
int BAT_ADC , Bat_Currunt, Bat_Charge_Currunt, Bat_Charge_Val, Charge_Stat = 0,Charge_Cnt;
int Pow_Mode , BAT_State, Pre_Pow_Mode, Test_status;
int Source_24;
uint8_t Bat_Test_Status;

int Balsin_signal;
int HANJUN_sig;
int BALJUN_sig;

int Balsin_Avr;
int HANJUN_Avr;
int BALJUN_Avr;

int Balsin_Save[Save_Max_Num];
int HANJUN_Save[Save_Max_Num];
int BALJUN_Save[Save_Max_Num];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_USB_PCD_Init(void);
static void MX_TIM1_Init(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

///////////////Start timer interrupt operation function///////////////////

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM1)
  {
    HAL_GPIO_TogglePin(RUN_LED_GPIO_Port, RUN_LED_Pin);
    //HAL_GPIO_TogglePin(BAT_ON_GPIO_Port, BAT_ON_Pin);
    HAL_IWDG_Refresh(&hiwdg);

  }
  else if(htim->Instance == TIM2)
    {
	  //Source_24 = SMPS_AC_Source();
	  //Read_24_ADC();
	  if(SOURCE_24_ADC > 200000){
		  Source_24 = 1;
	  }
	  else{
		  Source_24 = 0;
	  }
	  if(Source_24 == 1){
		  Pow_Mode = AC_Mode;
		  HAL_GPIO_WritePin(BAT_ON_GPIO_Port, BAT_ON_Pin, GPIO_PIN_RESET);
	  }
	  else{
		  Pow_Mode = BAT_Mode;
		  HAL_GPIO_WritePin(BAT_ON_GPIO_Port, BAT_ON_Pin, GPIO_PIN_SET);
	  }

    }
  else if(htim->Instance == TIM3)
  {



  }

}
////////////////Timer interrupt operation function ends ///////////////////

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_LPUART1_UART_Init();
  MX_USB_PCD_Init();
  MX_TIM1_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */

  // Timer1: 1 second cycle
  // Timer2: 0.1 second cycle
  // Timer3: 1 second cycle
  HAL_TIM_OC_Start_IT(&htim1,TIM_CHANNEL_1);
  HAL_TIM_OC_Start_IT(&htim2,TIM_CHANNEL_1);
  HAL_TIM_OC_Start_IT(&htim3,TIM_CHANNEL_1);

  LED_Control(ERR_LED_GPIO_Port, ERR_LED_Pin , LED_Off);

  Compile_Date();

  Pow_Mode = AC_Mode;
  BAT_State = BAT_OK;

  Immobility_Chargr_Table[0] = 6; //1:On 0: Off
  Immobility_Chargr_Table[1] = Immobility_Chargr_Table[0] + 38; //1:On 0: Off

  //25.5V
  Compensation_Chargr_Table[0][0] =  6; //1:On 0: Off
  Compensation_Chargr_Table[0][1] = 26. + Compensation_Chargr_Table[0][0]; //1:On 0: Off
  //26V
  Compensation_Chargr_Table[1][0] =  6; //1:On 0: Off
  Compensation_Chargr_Table[1][1] = 38 + Compensation_Chargr_Table[1][0]; //1:On 0: Off
  //26.5V
  Compensation_Chargr_Table[2][0] =  6; //1:On 0: Off
  Compensation_Chargr_Table[2][1] = 56 + Compensation_Chargr_Table[2][0]; //1:On 0: Off
  //27V
  Compensation_Chargr_Table[3][0] =  6; //1:On 0: Off
  Compensation_Chargr_Table[3][1] = 83 + Compensation_Chargr_Table[3][0]; //1:On 0: Off
  //27.5V
  Compensation_Chargr_Table[4][0] =  6; //1:On 0: Off
  Compensation_Chargr_Table[4][1]= 100 + Compensation_Chargr_Table[4][0] ; //1:On 0: Off

  Bat_Charge_Mode = Immobility_Mode;
  Bat_Test_Status = 0;

  Init_Do24();
  Compile_Date();

  /* huart1 RX Interrupt  Enable */
  /* Process Unlocked */
  __HAL_UNLOCK(&hlpuart1);
  /* Enable the UART Parity Error Interrupt */
  __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_PE);
  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_ERR);
  /* Enable the UART Data Register not empty Interrupt */
  __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
	  __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_ERR);
	  /* Enable the UART Data Register not empty Interrupt */
	  __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);

	  Pre_Pow_Mode = Pow_Mode;

	  /*
	  if(Pow_Mode == AC_Mode){
		  HAL_GPIO_WritePin(BAT_ON_GPIO_Port, BAT_ON_Pin, GPIO_PIN_RESET);
	  }
	  else{
		  HAL_GPIO_WritePin(BAT_ON_GPIO_Port, BAT_ON_Pin, GPIO_PIN_SET);
	  }
*/
	  SUB_Check(1);
	  Read_ADC();
	  SUB_Check(1);

	  Bat_Controll();

	  SUB_Check(1);
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 7;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_12CYCLES_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_12CYCLES_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_16;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  hiwdg.Init.EWI = 0;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 5599;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 9999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 5599;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 5599;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_DRD_FS.Instance = USB_DRD_FS;
  hpcd_USB_DRD_FS.Init.dev_endpoints = 8;
  hpcd_USB_DRD_FS.Init.speed = USBD_FS_SPEED;
  hpcd_USB_DRD_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_DRD_FS.Init.Sof_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_DRD_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RY2_GPIO_Port, RY2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BAT_TEST_Pin|BAT_CHARGE_Pin|BAT_ON_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, RS485_DE_Pin|RS485_RE_Pin|ERR_LED_Pin|RUN_LED_Pin
                          |FIRE_ALERT_1_Pin|FIRE_ALERT_2_Pin|RY1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : RY2_Pin */
  GPIO_InitStruct.Pin = RY2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RY2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : HANJUN_INT_Pin BALJUN_INT_Pin BALSIN_INT_Pin */
  GPIO_InitStruct.Pin = HANJUN_INT_Pin|BALJUN_INT_Pin|BALSIN_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BAT_TEST_Pin BAT_CHARGE_Pin */
  GPIO_InitStruct.Pin = BAT_TEST_Pin|BAT_CHARGE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BAT_ON_Pin */
  GPIO_InitStruct.Pin = BAT_ON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BAT_ON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : AC_FAULT_Pin */
  GPIO_InitStruct.Pin = AC_FAULT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(AC_FAULT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SOURCE_24_Pin */
  GPIO_InitStruct.Pin = SOURCE_24_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(SOURCE_24_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RS485_DE_Pin RS485_RE_Pin ERR_LED_Pin RUN_LED_Pin
                           FIRE_ALERT_1_Pin FIRE_ALERT_2_Pin RY1_Pin */
  GPIO_InitStruct.Pin = RS485_DE_Pin|RS485_RE_Pin|ERR_LED_Pin|RUN_LED_Pin
                          |FIRE_ALERT_1_Pin|FIRE_ALERT_2_Pin|RY1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */



void LED_Control(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin , uint16_t Staus){

  if(Staus == LED_On){
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
  }
  else if(Staus == LED_Off){
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
  }
}

void Read_24_ADC(void){

	ADC_ChannelConfTypeDef sConfig = {0};

	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.LowPowerAutoPowerOff = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;
	hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_160CYCLES_5;
	hadc1.Init.OversamplingMode = DISABLE;
	hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}

	// SOURCE_24_ADC;
	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1000);
	SOURCE_24_ADC= (HAL_ADC_GetValue(&hadc1) -120 )*(3.25/4096)*15*10000;

	HAL_ADC_Stop(&hadc1);
}
void Read_ADC(void){

	ADC_ChannelConfTypeDef sConfig = {0};

	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.LowPowerAutoPowerOff = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;
	hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_160CYCLES_5;
	hadc1.Init.OversamplingMode = DISABLE;
	hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}

	// MAIN_ADC;
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1000);
	MAIN_ADC= (HAL_ADC_GetValue(&hadc1) -120 )*(3.25/4096)*15*10000;

	HAL_ADC_Stop(&hadc1);


	// SOURCE_24_ADC;
	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1000);
	SOURCE_24_ADC= (HAL_ADC_GetValue(&hadc1) -120 )*(3.25/4096)*15*10000;

	HAL_ADC_Stop(&hadc1);


	  if(SOURCE_24_ADC > 200000){
		  Source_24 = 1;
	  }
	  else{
		  Source_24 = 0;
	  }
	  if(Source_24 == 1){
		  Pow_Mode = AC_Mode;
		  HAL_GPIO_WritePin(BAT_ON_GPIO_Port, BAT_ON_Pin, GPIO_PIN_RESET);
	  }
	  else{
		  Pow_Mode = BAT_Mode;
		  HAL_GPIO_WritePin(BAT_ON_GPIO_Port, BAT_ON_Pin, GPIO_PIN_SET);
	  }

	// BAT_CVH_ADC;
	sConfig.Channel = ADC_CHANNEL_10;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1000);
	BAT_CVH_ADC= (HAL_ADC_GetValue(&hadc1) -120 )*(3.25/4096)*15*10000;
	HAL_ADC_Stop(&hadc1);

	// BAT_CVL_ADC;
	sConfig.Channel = ADC_CHANNEL_14;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1000);
	BAT_CVL_ADC= (HAL_ADC_GetValue(&hadc1) -120 )*(3.25/4096)*15*10000;
	HAL_ADC_Stop(&hadc1);
}

void Battery_Test(void){

  int i = 0;

  Test_status = 0;

  if(Pow_Mode == AC_Mode){
    Charge_Stat = 2;
    HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_RESET);

    for(i=0;i<80;i++){

      HAL_GPIO_WritePin(BAT_TEST_GPIO_Port , BAT_TEST_Pin , GPIO_PIN_SET);
      /*
      HAL_Delay(100);

      if(SUB_UART_Receive_complete == 1){
        SW_Com();
      }
      */
      HAL_Delay(100);
      SUB_Check(1);

      Read_ADC();
      BAT_ADC = BAT_CVL_ADC;

      if(BAT_ADC < Err_Battery_val){
        HAL_GPIO_WritePin(BAT_TEST_GPIO_Port , BAT_TEST_Pin , GPIO_PIN_RESET);
        Test_status = 2;
        break;
      }
    }

    if(Test_status == 0){
    	Test_status = 1;
    }

    Charge_Stat = 0;
    HAL_GPIO_WritePin(BAT_TEST_GPIO_Port , BAT_TEST_Pin , GPIO_PIN_RESET);
  }
  else{
    Test_status = 1;
  }
}

int SMPS_AC_Source(void){

	int Read_val = 0;
	if(HAL_GPIO_ReadPin(SOURCE_24_GPIO_Port, SOURCE_24_Pin) == GPIO_PIN_RESET){
		Read_val = 0;
	}
	else{
		Read_val = 1;
	}

	return Read_val;
}
int Read_BALSIN(void){
  int Read_val = 0;
  if(HAL_GPIO_ReadPin(BALSIN_INT_GPIO_Port, BALSIN_INT_Pin) == GPIO_PIN_RESET){
    Read_val = 1;
  }
  else{
    Read_val = 0;
  }

  return Read_val;
}

int Read_HANJUN(void){
  int Read_val = 0;
  if(HAL_GPIO_ReadPin(HANJUN_INT_GPIO_Port, HANJUN_INT_Pin) == GPIO_PIN_RESET){
    Read_val = 1;
  }
  else{
    Read_val = 0;
  }

  return Read_val;
}

int Read_BALJUN(void){
  int Read_val = 0;
  if(HAL_GPIO_ReadPin(BALJUN_INT_GPIO_Port, BALJUN_INT_Pin) == GPIO_PIN_RESET){
    Read_val = 1;
  }
  else{
    Read_val = 0;
  }

  return Read_val;
}

void Bat_Controll(void){

    if(Pow_Mode == AC_Mode){

      //HAL_GPIO_WritePin(BAT_ON_GPIO_Port, BAT_ON_Pin, GPIO_PIN_RESET);
	  //HAL_Delay(50);
      SUB_Check(50);
      Read_ADC();
      SUB_Check(1);
      /*
      if(Bat_Test_Status == 1){
    	  Bat_Test_Status = 0;
    	  Battery_Test();
      }
      */
      SUB_Check(1);
      if((BAT_ADC >= Err_Battery_val)&(BAT_ADC < High_Battery_val )){
        Charge_Stat = 1;
        BAT_State = BAT_OK;
        if(Bat_Charge_Mode == Immobility_Mode){

        	if(BAT_ADC > 280000){
        		Bat_Charge_Mode = Compensation_Mode;
        	}
        }
        else if(Bat_Charge_Mode == Compensation_Mode){
        	if((BAT_ADC >= 255000)&(BAT_ADC < 260000 )){
        		Bat_Charge_Compensation_Volt = Charge_255_Mode;
        	}
        	else if((BAT_ADC >= 260000)&(BAT_ADC < 265000 )){
        		Bat_Charge_Compensation_Volt = Charge_260_Mode;
        	}
        	else if((BAT_ADC >= 265000)&(BAT_ADC < 270000 )){
        		Bat_Charge_Compensation_Volt = Charge_265_Mode;
        	}
        	else if((BAT_ADC >= 270000)&(BAT_ADC < 275000 )){
        		Bat_Charge_Compensation_Volt = Charge_270_Mode;
        	}
        	else if((BAT_ADC >= 275000)&(BAT_ADC < 280000 )){
        		Bat_Charge_Compensation_Volt = Charge_275_Mode;
        	}
        	else{
        		Bat_Charge_Compensation_Volt = Charge_Off_Mode;
        	}
        }
      }
      else if(BAT_ADC >= High_Battery_val ){
        Charge_Stat = 0;
        Bat_Charge_Currunt = 0;
        BAT_State = BAT_OK;
      }
      else if(BAT_ADC < Err_Battery_val){
        Charge_Stat = 2;
        Bat_Charge_Currunt = 0;
        BAT_State = BAT_False;
      }
    }
    else{
      Charge_Stat = 0;
      Read_ADC();
      BAT_ADC = BAT_CVL_ADC;

      if(BAT_ADC < Off_Battery_val){
        HAL_GPIO_WritePin(BAT_ON_GPIO_Port, BAT_ON_Pin, GPIO_PIN_RESET);
      }
      else{
        HAL_GPIO_WritePin(BAT_ON_GPIO_Port, BAT_ON_Pin, GPIO_PIN_SET);
      }
    }

    SUB_Check(1);

    if(Charge_Stat != 1){
      Read_ADC();
      BAT_ADC = BAT_CVL_ADC;
    }
    else{
      if(Charge_Cnt == 4){
        Read_ADC();       //충전중에는 베터리 값을 갱신하지 않는다.
        Bat_Charge_Currunt = Bat_Currunt ;
        Bat_Charge_Val = BAT_CVH_ADC;
      }
      else if((Charge_Cnt >= 15) & (Charge_Cnt <40)){
      Read_ADC();
      BAT_ADC = BAT_CVL_ADC;
      }
    }
    SUB_Check(1);
}

void SW_Com(void){

	  // CRC 체크
	  uint8_t pow_st;
	  uint8_t Uart_crc = 0;

	  // 전송할 중계기 정보를 확인한다.

	  for(int i = 0; i <SUB_UART_buf_count_Save - 5 ; i++){
		  Uart_crc = Uart_crc ^ SUB_UART_RX_buf[i+2];
	  }

	  if(Uart_crc == SUB_UART_RX_buf[SUB_UART_buf_count_Save - 3]){
	    //0x51 , 'Q' ,중계기 정보 요청 (SUB에서 설정한 값만)

	    if(SUB_UART_RX_buf[4] == 0x54){        // TB board: '0x54'
	      if(SUB_UART_RX_buf[3] == 0x51){

	    	SUB_UART_TX_buf[0] = 0x53;    //S
	    	SUB_UART_TX_buf[1] = 0x54;    //T
	    	SUB_UART_TX_buf[2] = 0x54;    //T
	    	SUB_UART_TX_buf[3] = 0x52;    //R
	    	SUB_UART_TX_buf[4] = 0x01;    //address
	    	SUB_UART_TX_buf[5] = 0x0A;    //version 10 -> 1.0
	    	SUB_UART_TX_buf[6] = 0x00;    //외부 발신
	    	SUB_UART_TX_buf[7] = 0x00;    //외부 전원
	    	SUB_UART_TX_buf[8] = 0x00;    //베터리 AC 상태
	    	SUB_UART_TX_buf[9] = 0x00;    //베터리 전압
	    	SUB_UART_TX_buf[10] = 0x00;   //메인 전압
	    	SUB_UART_TX_buf[11] = 0x00;   //충전 전압
	    	SUB_UART_TX_buf[12] = 0x00;   //베터리 테스트 결과
	    	SUB_UART_TX_buf[13] = 0x00;   //dummy1
	    	SUB_UART_TX_buf[14] = 0x00;   //dummy2
	    	SUB_UART_TX_buf[15] = 0x00;   //CRC
	    	SUB_UART_TX_buf[16] = 0x45;   //E
	    	SUB_UART_TX_buf[17] = 0x44;   //D

	        Uart_crc = 0;

	        if((HANJUN_sig == 0) &(BALJUN_sig ==0)){
	          pow_st = 0;
	        }
	        else if((HANJUN_sig == 1) &(BALJUN_sig ==0)){
	          pow_st = 1;
	        }
	        else if((HANJUN_sig == 0) &(BALJUN_sig ==1)){
	          pow_st = 2;
	        }
	        else if((HANJUN_sig == 1) &(BALJUN_sig ==1)){
	          pow_st = 3;
	        }

	        SUB_UART_TX_buf[6] = Balsin_signal;
	        SUB_UART_TX_buf[7] = pow_st;    //외부 전원 1: 한전 , 2:발전
	        //Uart_tx_buf[8] = AC_FAULT;    //베터리 AC 상태
	        SUB_UART_TX_buf[8] = Pow_Mode;

	        if(((BAT_ADC / 1000) - 100)<0){
	        	SUB_UART_TX_buf[9] = 0;    //베터리 전압
	        }
	        else {
	        	SUB_UART_TX_buf[9] = (BAT_ADC / 1000) - 100;    //베터리 전압
	        }

	        if(((MAIN_ADC / 1000) - 100)<0){
	        	SUB_UART_TX_buf[10] = 0;    //베터리 전압
	        }
	        else {
	        	SUB_UART_TX_buf[10] = (MAIN_ADC / 1000) - 100;    //베터리 전압
	        }

	         if(((Bat_Charge_Val / 1000) - 100)<0){
	        	 SUB_UART_TX_buf[11] = 0;    //베터리 전압
	        }
	        else {
	        	SUB_UART_TX_buf[11] = (Bat_Charge_Val / 1000) - 100;    //베터리 전압
	        }


	        //Uart_tx_buf[10] = (MAIN_ADC / 1000) - 100;   //메인 전압
	        //Uart_tx_buf[11] = (Bat_Charge_Val / 1000) - 100;   //충전 전압
	         SUB_UART_TX_buf[12] = ((Charge_Stat << 4 )&0xF0) | (Test_status&0x0F);   //베터리 테스트 결과

	         SUB_UART_TX_buf[14] = 24 ;//dummy2   표시등 퓨즈 단선 전압값 , 벽부형만 적용 자립형은 24v고정
	        for(int i = 0; i <TB_TX_data_Length - 5 ; i++){
	          Uart_crc = Uart_crc ^ SUB_UART_TX_buf[i+2];
	        }
	        SUB_UART_TX_buf[15] = Uart_crc;


	        HAL_GPIO_WritePin(RS485_DE_GPIO_Port , RS485_DE_Pin , GPIO_PIN_SET);
	        HAL_GPIO_WritePin(RS485_RE_GPIO_Port , RS485_RE_Pin , GPIO_PIN_SET);

	        HAL_Delay(1);
	        if(HAL_UART_Transmit(&hlpuart1, SUB_UART_TX_buf, TB_TX_data_Length, 1000)!= HAL_OK)
	            {
	              Error_Handler();
	            }

	        HAL_Delay(1);

	        HAL_GPIO_WritePin(RS485_DE_GPIO_Port , RS485_DE_Pin , GPIO_PIN_RESET);
	        HAL_GPIO_WritePin(RS485_RE_GPIO_Port , RS485_RE_Pin , GPIO_PIN_RESET);


	      }
	      else if(SUB_UART_RX_buf[3] == 0x53){

	        if(SUB_UART_RX_buf[6] == 0x01){
	        	LED_Control(ERR_LED_GPIO_Port, ERR_LED_Pin , LED_On);

	            if(Bat_Test_Status == 0){
	            	Bat_Test_Status = 1;
	            	Battery_Test();
	            }

	            Bat_Test_Status = 0;
	            LED_Control(ERR_LED_GPIO_Port, ERR_LED_Pin , LED_Off);
	        }
	        else if(SUB_UART_RX_buf[6] == 0x00){
	        	Test_status = 0;
	        }

	        if(SUB_UART_RX_buf[8] == 0x01){

	          HAL_GPIO_WritePin(FIRE_ALERT_1_GPIO_Port , FIRE_ALERT_1_Pin , GPIO_PIN_SET);
	          HAL_GPIO_WritePin(FIRE_ALERT_2_GPIO_Port , FIRE_ALERT_2_Pin , GPIO_PIN_SET);
	        }
	        else if(SUB_UART_RX_buf[8] == 0x00){

	          HAL_GPIO_WritePin(FIRE_ALERT_1_GPIO_Port , FIRE_ALERT_1_Pin , GPIO_PIN_RESET);
	          HAL_GPIO_WritePin(FIRE_ALERT_2_GPIO_Port , FIRE_ALERT_2_Pin , GPIO_PIN_RESET);
	        	}
	      }
		else if(SUB_UART_RX_buf[3] == 0x56){		// Version V , 0x56
			SUB_Com_SW_V();
		}

	    }


	    if(SUB_UART_RX_buf[4] == 0x44){        // DO24 board: '0x44'
	    	if(SUB_UART_RX_buf[3] == 0x51){
	    		UI_Do24_Q();
	    	}
	    	else if(SUB_UART_RX_buf[3] == 0x53){
	    		UI_Do24_S();
	    	}

	    }
	  }

	  for(int i = 0; i <SUB_UART_buf_len ; i++){
		  SUB_UART_RX_buf[i] = 0;
	  }


	  SUB_UART_Receive_complete = 0;

}

void SUB_Check(int Wtime){
	for(int i=0 ; i< Wtime; i++){
		if(SUB_UART_Receive_complete == 1){
			HAL_Delay(1);
		  SW_Com();
		  SUB_UART_Receive_complete = 0;
	  }
	}
}

void SUB_Com_SW_V(void){

	uint8_t Uart_crc;

    SUB_UART_TX_buf[0] = 0x53;    //S
    SUB_UART_TX_buf[1] = 0x54;    //T
    SUB_UART_TX_buf[2] = 0x53;    //S
    SUB_UART_TX_buf[3] = 0x76;    //v
    SUB_UART_TX_buf[4] = 0x54;    //T
    SUB_UART_TX_buf[5] = 1;    //address
    SUB_UART_TX_buf[6] = F_Version_Year;
    SUB_UART_TX_buf[7] = F_Version_Month;
    SUB_UART_TX_buf[8] = F_Version_Day;
    SUB_UART_TX_buf[9] = F_Version_Hour;
    SUB_UART_TX_buf[10] = F_Version_Min;
    SUB_UART_TX_buf[11] = F_Version_Sec;
    SUB_UART_TX_buf[12] = 0x00;   //CRC
    SUB_UART_TX_buf[13] = 0x45;   //E
    SUB_UART_TX_buf[14] = 0x44;   //D


    for(int i = 0; i <Sub_TB_V_length - 5 ; i++){
      Uart_crc = Uart_crc ^ SUB_UART_TX_buf[i+2];
    }
    SUB_UART_TX_buf[Sub_TB_V_length-3] = Uart_crc;

    HAL_GPIO_WritePin(RS485_DE_GPIO_Port , RS485_DE_Pin , GPIO_PIN_SET);
    HAL_GPIO_WritePin(RS485_RE_GPIO_Port , RS485_RE_Pin , GPIO_PIN_SET);

    HAL_Delay(1);
    if(HAL_UART_Transmit(&hlpuart1, SUB_UART_TX_buf, Sub_TB_V_length, 1000)!= HAL_OK)
        {
          Error_Handler();
        }

    HAL_Delay(1);

    HAL_GPIO_WritePin(RS485_DE_GPIO_Port , RS485_DE_Pin , GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RS485_RE_GPIO_Port , RS485_RE_Pin , GPIO_PIN_RESET);

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
