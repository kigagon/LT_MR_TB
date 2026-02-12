/*
 * DO24_Code.c
 *
 *  Created on: Sep 5, 2025
 *      Author: root
 */

#include "main.h"
#include "DO24_Code.h"

uint8_t RY_Status[24] ;
uint8_t Fire_Status[2] ;

uint8_t DO24_Address;

void Init_Do24(void){

	  DO24_Address = 1;

	  if(DO24_Address == 0){
	    PY1_ON();
	    HAL_Delay(100);
	    PY2_ON();
	    HAL_Delay(100);
	    FIRE_ALERT_1_ON();
	    HAL_Delay(100);
	    FIRE_ALERT_2_ON();
		HAL_Delay(100);

	    HAL_Delay(1000);
	    PY1_OFF();
	    HAL_Delay(100);
	    PY2_OFF();
	    HAL_Delay(100);
	    FIRE_ALERT_1_OFF();
	    HAL_Delay(100);
	    FIRE_ALERT_2_OFF();
		HAL_Delay(100);

	  }

	  for(int i=0;i<24;i++){
	    RY_Status[i] = 0;
	  }
}

void Set_Do24(void){

    if(RY_Status[0] == 1){
      PY1_ON();
    }
    else if(RY_Status[0] == 0){
      PY1_OFF();
    }

    if(RY_Status[1] == 1){
      PY2_ON();
    }
    else if(RY_Status[1] == 0){
      PY2_OFF();
    }


    if(Fire_Status[1] == 1){
    	FIRE_ALERT_2_ON();
    }
    else if(Fire_Status[1] == 0){
    	FIRE_ALERT_2_OFF();
    }
}

void UI_Do24_Q(void){

	uint8_t Uart_crc, tx_buf_len = DO24_TX_data_Length;

    SUB_UART_TX_buf[0] = 0x53;    //S
    SUB_UART_TX_buf[1] = 0x54;    //T
    SUB_UART_TX_buf[2] = 0x44;    //D
    SUB_UART_TX_buf[3] = 0x52;    //R
    SUB_UART_TX_buf[4] = DO24_Address;    //address
    SUB_UART_TX_buf[5] = 0x0A;    //version 10 -> 1.0
    SUB_UART_TX_buf[6] = 0x00;    //Relay 정보 1 ~8
    SUB_UART_TX_buf[7] = 0x00;    //Relay 정보 9 ~16
    SUB_UART_TX_buf[8] = 0x00;    //Relay 정보 17 ~24
    SUB_UART_TX_buf[9] = 0x00;   //dummy1
    SUB_UART_TX_buf[10] = 0x00;   //dummy2
    SUB_UART_TX_buf[11] = 0x00;   //CRC
    SUB_UART_TX_buf[12] = 0x45;   //E
    SUB_UART_TX_buf[13] = 0x44;   //D

    Uart_crc = 0;

    for(int i=0 ; i<8 ; i++){
      SUB_UART_TX_buf[6] = 0;
      SUB_UART_TX_buf[6] = (RY_Status[0] << 7) | (RY_Status[1] << 6) | (RY_Status[2] << 5) | (RY_Status[3] << 4) |
                       (RY_Status[4] << 3) | (RY_Status[5] << 2) | (RY_Status[6] << 1) | (RY_Status[7] << 0);
      SUB_UART_TX_buf[7] = 0;
      SUB_UART_TX_buf[7] = (RY_Status[0+8] << 7) | (RY_Status[1+8] << 6) | (RY_Status[2+8] << 5) | (RY_Status[3+8] << 4) |
                       (RY_Status[4+8] << 3) | (RY_Status[5+8] << 2) | (RY_Status[6+8] << 1) | (RY_Status[7+8] << 0);
      SUB_UART_TX_buf[8] = 0;
      SUB_UART_TX_buf[8] = (RY_Status[0+8+8] << 7) | (RY_Status[1+8+8] << 6) | (RY_Status[2+8+8] << 5) | (RY_Status[3+8+8] << 4) |
                       (RY_Status[4+8+8] << 3) | (RY_Status[5+8+8] << 2) | (RY_Status[6+8+8] << 1) | (RY_Status[7+8+8] << 0);

    }
    for(int i = 0; i <tx_buf_len - 5 ; i++){
      Uart_crc = Uart_crc ^ SUB_UART_TX_buf[i+2];
    }
    SUB_UART_TX_buf[11] = Uart_crc;

    HAL_GPIO_WritePin(RS485_DE_GPIO_Port , RS485_DE_Pin , GPIO_PIN_SET);
    HAL_GPIO_WritePin(RS485_RE_GPIO_Port , RS485_RE_Pin , GPIO_PIN_SET);

    HAL_Delay(1);
    if(HAL_UART_Transmit(&hlpuart1, SUB_UART_TX_buf, tx_buf_len, 1000)!= HAL_OK)
        {
          Error_Handler();
        }

    HAL_Delay(1);

    HAL_GPIO_WritePin(RS485_DE_GPIO_Port , RS485_DE_Pin , GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RS485_RE_GPIO_Port , RS485_RE_Pin , GPIO_PIN_RESET);
}

void UI_Do24_S(void){
    for(int i=0;i<8;i++){
      RY_Status[i] = (SUB_UART_RX_buf[6] >> (7 - i)) & 0x01;
      RY_Status[i+8] = (SUB_UART_RX_buf[7] >> (7 - i)) & 0x01;
      RY_Status[i+8+8] = (SUB_UART_RX_buf[8] >> (7 - i)) & 0x01;
    }
    Set_Do24();
}

void PY1_ON(void){
	HAL_GPIO_WritePin(RY1_GPIO_Port,RY1_Pin,GPIO_PIN_SET);
}
void PY1_OFF(void){
	HAL_GPIO_WritePin(RY1_GPIO_Port,RY1_Pin,GPIO_PIN_RESET);
}
void PY2_ON(void){
	HAL_GPIO_WritePin(RY2_GPIO_Port,RY2_Pin,GPIO_PIN_SET);
}
void PY2_OFF(void){
	HAL_GPIO_WritePin(RY2_GPIO_Port,RY2_Pin,GPIO_PIN_RESET);
}
void FIRE_ALERT_1_ON(void){
	HAL_GPIO_WritePin(FIRE_ALERT_1_GPIO_Port,FIRE_ALERT_1_Pin,GPIO_PIN_SET);
}
void FIRE_ALERT_1_OFF(void){
	HAL_GPIO_WritePin(FIRE_ALERT_1_GPIO_Port,FIRE_ALERT_1_Pin,GPIO_PIN_RESET);
}
void FIRE_ALERT_2_ON(void){
	HAL_GPIO_WritePin(FIRE_ALERT_2_GPIO_Port,FIRE_ALERT_2_Pin,GPIO_PIN_SET);
}
void FIRE_ALERT_2_OFF(void){
	HAL_GPIO_WritePin(FIRE_ALERT_2_GPIO_Port,FIRE_ALERT_2_Pin,GPIO_PIN_RESET);
}
