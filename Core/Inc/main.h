/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

extern UART_HandleTypeDef hlpuart1;

#define LED_On  1
#define LED_Off 0

#define Off_Battery_val 210000
#define Err_Battery_val 220000
#define Low_Battery_val 256000
#define High_Battery_val 285000                 // 자립형

#define DO24_TX_data_Length  14
#define TB_TX_data_Length  18

#define SUB_UART_buf_len 512
extern uint8_t SUB_UART_TX_buf[SUB_UART_buf_len] ;
extern uint8_t SUB_UART_RX_buf[SUB_UART_buf_len];
extern uint8_t SUB_UART_Tx_buf_tmp[SUB_UART_buf_len];
extern uint8_t SUB_UART_RX_buf_tmp[SUB_UART_buf_len];
extern uint8_t SUB_UART_State ;					// Define reception start state
extern int SUB_UART_buf_count;					// Save the length of received data after starting reception
extern int SUB_UART_buf_count_tmp;				// Variable to store the length of the received data before initializing it when reception is completed
extern uint8_t SUB_UART_Receive_complete;		// Variable indicating that reception has been completed
extern int SUB_UART_buf_count_Save;

#define AC_Mode 1
#define BAT_Mode 0

#define BAT_OK 1
#define BAT_False 2

#define Charge_Off_Mode		0
#define Immobility_Mode		1
#define Compensation_Mode	2


#define Charge_255_Mode		0
#define Charge_260_Mode		1
#define Charge_265_Mode		2
#define Charge_270_Mode		3
#define Charge_275_Mode		4

# define Sub_TB_V_length      15

extern uint32_t AC_FAULT_CNT ;

extern int Bat_Charge_Mode;
extern int Bat_Charge_Compensation_Volt;
extern int Immobility_Chargr_Table[2]; //1:On 0: Off
extern int Compensation_Chargr_Table[5][2]; //1:On 0: Off
extern int BAT_ADC , Bat_Currunt, Bat_Charge_Currunt, Bat_Charge_Val, Charge_Stat, Charge_Cnt;
extern int Pow_Mode , BAT_State, Pre_Pow_Mode,Test_status;
extern uint8_t Bat_Test_Status;


#define Save_Max_Num 5

extern int Balsin_signal;
extern int HANJUN_sig;
extern int BALJUN_sig;

extern int Balsin_Avr;
extern int HANJUN_Avr;
extern int BALJUN_Avr;

extern int Balsin_Save[Save_Max_Num];
extern int HANJUN_Save[Save_Max_Num];
extern int BALJUN_Save[Save_Max_Num];

void LED_Control(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin , uint16_t Staus);
void Read_24_ADC(void);
void Read_ADC(void);
void Battery_Test(void);
void SW_Com(void);
void SUB_Com_SW_V(void);

int SMPS_AC_Source(void);
int Read_BALSIN(void);
int Read_HANJUN(void);
int Read_BALJUN(void);

void Bat_Controll(void);
void SUB_Check(int Wtime);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RY2_Pin GPIO_PIN_15
#define RY2_GPIO_Port GPIOC
#define HANJUN_INT_Pin GPIO_PIN_1
#define HANJUN_INT_GPIO_Port GPIOC
#define BALJUN_INT_Pin GPIO_PIN_2
#define BALJUN_INT_GPIO_Port GPIOC
#define BALSIN_INT_Pin GPIO_PIN_3
#define BALSIN_INT_GPIO_Port GPIOC
#define DC2_MAIN_Pin GPIO_PIN_0
#define DC2_MAIN_GPIO_Port GPIOA
#define BAT_TEST_Pin GPIO_PIN_1
#define BAT_TEST_GPIO_Port GPIOA
#define BAT_CHARGE_Pin GPIO_PIN_2
#define BAT_CHARGE_GPIO_Port GPIOA
#define BAT_ON_Pin GPIO_PIN_3
#define BAT_ON_GPIO_Port GPIOA
#define AC_FAULT_Pin GPIO_PIN_5
#define AC_FAULT_GPIO_Port GPIOA
#define AC_FAULT_EXTI_IRQn EXTI4_15_IRQn
#define BAT_CVH_Pin GPIO_PIN_6
#define BAT_CVH_GPIO_Port GPIOA
#define BAT_CVL_Pin GPIO_PIN_7
#define BAT_CVL_GPIO_Port GPIOA
#define SOURCE_24_Pin GPIO_PIN_0
#define SOURCE_24_GPIO_Port GPIOB
#define RS485_DE_Pin GPIO_PIN_1
#define RS485_DE_GPIO_Port GPIOB
#define RS485_RE_Pin GPIO_PIN_2
#define RS485_RE_GPIO_Port GPIOB
#define ERR_LED_Pin GPIO_PIN_12
#define ERR_LED_GPIO_Port GPIOB
#define RUN_LED_Pin GPIO_PIN_13
#define RUN_LED_GPIO_Port GPIOB
#define FIRE_ALERT_1_Pin GPIO_PIN_7
#define FIRE_ALERT_1_GPIO_Port GPIOB
#define FIRE_ALERT_2_Pin GPIO_PIN_8
#define FIRE_ALERT_2_GPIO_Port GPIOB
#define RY1_Pin GPIO_PIN_9
#define RY1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
