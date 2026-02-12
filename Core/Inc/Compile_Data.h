/*
 * Compile_Data.h
 *
 *  Created on: Sep 18, 2025
 *      Author: root
 */

#ifndef INC_COMPILE_DATA_H_
#define INC_COMPILE_DATA_H_

extern uint8_t F_Version_Year;
extern uint8_t F_Version_Month;
extern uint8_t F_Version_Day;
extern uint8_t F_Version_Hour;
extern uint8_t F_Version_Min;
extern uint8_t F_Version_Sec;

extern uint8_t F_Version_Sub_Con[4][15];
extern uint8_t F_Version_CCU[4][8][15];

void Compile_Date(void);

#endif /* INC_COMPILE_DATA_H_ */
