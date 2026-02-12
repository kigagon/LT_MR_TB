/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32u0xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32u0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_lpuart1_tx;
extern UART_HandleTypeDef hlpuart1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SVC instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32U0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32u0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(AC_FAULT_Pin);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  AC_FAULT_CNT++;

  /* USER CODE END EXTI4_15_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 2 and channel 3 interrupts.
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */

  /* USER CODE END DMA1_Channel2_3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_lpuart1_tx);
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 1 */

  /* USER CODE END DMA1_Channel2_3_IRQn 1 */
}

/**
  * @brief This function handles TIM1 Capture Compare Interrupt.
  */
void TIM1_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_CC_IRQn 0 */

  /* USER CODE END TIM1_CC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_CC_IRQn 1 */
  /*
  if((AC_FAULT_CNT > 105)& (AC_FAULT_CNT < 145)){
	Pow_Mode = AC_Mode;
  }
  else{
  	Pow_Mode = BAT_Mode;
  }

  AC_FAULT_CNT = 0;
  */
  /* USER CODE END TIM1_CC_IRQn 1 */
}

/**
  * @brief This function handles TIM2 Global Interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  for(int i=0; i<(Save_Max_Num-1); i++){
	  Balsin_Save[i] = Balsin_Save[i+1];
	  HANJUN_Save[i] = HANJUN_Save[i+1];
	  BALJUN_Save[i] = BALJUN_Save[i+1];
  }

  Balsin_Save[Save_Max_Num-1] = Read_BALSIN();
  HANJUN_Save[Save_Max_Num-1] = Read_HANJUN();
  BALJUN_Save[Save_Max_Num-1] = Read_BALJUN();

  Balsin_Avr = 0;
  HANJUN_Avr = 0;
  BALJUN_Avr = 0;

  for(int i=0; i<Save_Max_Num; i++){
	  Balsin_Avr += Balsin_Save[i];
	  HANJUN_Avr += HANJUN_Save[i];
	  BALJUN_Avr += BALJUN_Save[i];
  }

  if(Balsin_Avr >(Save_Max_Num/2)){
	  Balsin_signal = 1;
  }
  else{
	  Balsin_signal = 0;
  }

  if(HANJUN_Avr >(Save_Max_Num/2)){
	  HANJUN_sig = 1;
  }
  else{
	  HANJUN_sig = 0;
  }

  if(BALJUN_Avr >(Save_Max_Num/2)){
	  BALJUN_sig = 1;
  }
  else{
	  BALJUN_sig = 0;
  }
  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM3 Global Interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */


  //  int charge_time = 18;
  //    int wait_time = 40 + charge_time;

  if(Charge_Stat == 0){
	Charge_Cnt = 0;

	HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_RESET);

  }
  else if(Charge_Stat == 1){

	/*
	if(Charge_Cnt < charge_time){

	  HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_SET);

	  Charge_Cnt++;
	}
	else if((Charge_Cnt >= charge_time) & (Charge_Cnt <wait_time)){

	  HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_RESET);

	  Charge_Cnt++;
	}
	else if(Charge_Cnt >=wait_time ){
	  Charge_Cnt = 0;

	  HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_RESET);

	}
	*/

	if(Bat_Charge_Mode == Immobility_Mode){
		if(Charge_Cnt == 0){
			Charge_Cnt++;
		}
		else if ((Charge_Cnt > 0)&(Charge_Cnt <= Immobility_Chargr_Table[0] )){
			//HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_SET);
			Charge_Cnt++;
		}
		else if ((Immobility_Chargr_Table[0] > 0)&(Charge_Cnt <= Immobility_Chargr_Table[1] )){
			//HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_SET);
			HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_RESET);
			Charge_Cnt++;
		}
		else if (Immobility_Chargr_Table[1] > 0){
			//HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_SET);
			Charge_Cnt = 0;
			//Bat_Charge_Mode = Compensation_Mode;
		}
	}
	else if(Bat_Charge_Mode == Compensation_Mode){
		if(Charge_Cnt == 0){
			Charge_Cnt++;
		}
		else if ((Charge_Cnt > 0)&(Charge_Cnt <= Compensation_Chargr_Table[Bat_Charge_Compensation_Volt][0] )){
			//HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_SET);
			Charge_Cnt++;
		}
		else if ((Compensation_Chargr_Table[Bat_Charge_Compensation_Volt][0] > 0)&(Charge_Cnt <= Compensation_Chargr_Table[Bat_Charge_Compensation_Volt][1] )){
			//HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_SET);
			HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_RESET);
			Charge_Cnt++;
		}
		else if (Compensation_Chargr_Table[Bat_Charge_Compensation_Volt][1] > 0){
			//HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(BAT_CHARGE_GPIO_Port , BAT_CHARGE_Pin , GPIO_PIN_SET);
			Charge_Cnt = 0;
		}
	}
  }

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles USART3 (combined with EXTI 24) + LPUART1 global interrupt (combined with EXTI lines 28).
  */
void USART3_LPUART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_LPUART1_IRQn 0 */

  /* USER CODE END USART3_LPUART1_IRQn 0 */
  HAL_UART_IRQHandler(&hlpuart1);
  /* USER CODE BEGIN USART3_LPUART1_IRQn 1 */

  SUB_UART_RX_buf_tmp[SUB_UART_buf_count_tmp] = LPUART1->RDR;
  SUB_UART_buf_count_tmp++;

  if(SUB_UART_buf_count_tmp > 2){
	if((SUB_UART_RX_buf_tmp[SUB_UART_buf_count_tmp-3] == 0x53) &&(SUB_UART_RX_buf_tmp[SUB_UART_buf_count_tmp-2] == 0x54)
			&(SUB_UART_State == 0))
	{
		  SUB_UART_State = 1;
		  SUB_UART_buf_count = 2;
		  SUB_UART_RX_buf[0] = SUB_UART_RX_buf_tmp[SUB_UART_buf_count_tmp-3];
		  SUB_UART_RX_buf[1] = SUB_UART_RX_buf_tmp[SUB_UART_buf_count_tmp-2];

	}
  }

  if(SUB_UART_State == 1){
	  SUB_UART_RX_buf[SUB_UART_buf_count] = SUB_UART_RX_buf_tmp[SUB_UART_buf_count_tmp-1];
	  SUB_UART_buf_count++;

	  if((SUB_UART_RX_buf_tmp[SUB_UART_buf_count_tmp -2 ] == 0x45) &&(SUB_UART_RX_buf_tmp[SUB_UART_buf_count_tmp - 1] == 0x44)) {
		  SUB_UART_Receive_complete = 1;
		  SUB_UART_State = 0;
		  SUB_UART_buf_count_tmp = 0;
		  SUB_UART_buf_count_Save = SUB_UART_buf_count;
		  SUB_UART_buf_count = 0;

	  }
  }

  if(SUB_UART_buf_count > SUB_UART_buf_len -1){
	  SUB_UART_State = 0;
	  SUB_UART_buf_count = 0;
	  SUB_UART_buf_count_tmp = 0;
	  SUB_UART_Receive_complete = 0;
	  for(int k=0;k<SUB_UART_buf_len;k++){
		SUB_UART_RX_buf_tmp[k] = 0;
	  }
  }


  /* USER CODE END USART3_LPUART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
