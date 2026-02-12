/*
 * DO24_Code.h
 *
 *  Created on: Sep 5, 2025
 *      Author: root
 */

#ifndef INC_DO24_CODE_H_
#define INC_DO24_CODE_H_


extern uint8_t RY_Status[24] ;
extern uint8_t DO24_Address;
extern uint8_t Fire_Status[2] ;

void PY1_ON(void);
void PY1_OFF(void);
void PY2_ON(void);
void PY2_OFF(void);
void FIRE_ALERT_1_ON(void);
void FIRE_ALERT_1_OFF(void);
void FIRE_ALERT_2_ON(void);
void FIRE_ALERT_2_OFF(void);

void UI_Do24_Q(void);
void UI_Do24_S(void);
#endif /* INC_DO24_CODE_H_ */
