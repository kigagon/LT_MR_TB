/*
 * Compile_Data.c
 *
 *  Created on: Sep 18, 2025
 *      Author: root
 */

#include "main.h"
#include "Compile_Data.h"

////////////////////////////
uint8_t F_Version_Year;
uint8_t F_Version_Month;
uint8_t F_Version_Day;
uint8_t F_Version_Hour;
uint8_t F_Version_Min;
uint8_t F_Version_Sec;

uint8_t F_Version_Sub_Con[4][15];
uint8_t F_Version_CCU[4][8][15];


void Compile_Date(void){

	  ////////////////////////////

	  uint8_t Date_Year[4], tmp_Date_Year;
	  uint8_t Date_Month[3], tmp_Date_Month;
	  uint8_t Date_Day[2], tmp_Date_Day;

	  uint8_t Date_Hour[2];
	  uint8_t Date_Min[2];
	  uint8_t Date_Sec[2];

	  for(int i=0; i<4 ; i++){
		  Date_Year[i] = __DATE__[i+7];
	  }
	  for(int i=0; i<3 ; i++){
		  Date_Month[i] = __DATE__[i+0];
	  }
	  for(int i=0; i<2 ; i++){
		  Date_Day[i] = __DATE__[i+4];
	  }

	  tmp_Date_Year = (Date_Year[2]-48)*10 + (Date_Year[3] - 48);
	  if((Date_Month[0] == 'J')&(Date_Month[1] == 'a')&(Date_Month[2] == 'n')){
		  tmp_Date_Month = 1;
	  }
	  else if((Date_Month[0] == 'F')&(Date_Month[1] == 'a')&(Date_Month[2] == 'b')){
		  tmp_Date_Month = 2;
	  }
	  else if((Date_Month[0] == 'M')&(Date_Month[1] == 'a')&(Date_Month[2] == 'r')){
		  tmp_Date_Month = 3;
	 }
	  else if((Date_Month[0] == 'A')&(Date_Month[1] == 'p')&(Date_Month[2] == 'r')){
		  tmp_Date_Month = 4;
	 }
	  else if((Date_Month[0] == 'M')&(Date_Month[1] == 'a')&(Date_Month[2] == 'y')){
		  tmp_Date_Month = 5;
	 }
	  else if((Date_Month[0] == 'J')&(Date_Month[1] == 'u')&(Date_Month[2] == 'n')){
		  tmp_Date_Month = 6;
	 }
	  else if((Date_Month[0] == 'J')&(Date_Month[1] == 'u')&(Date_Month[2] == 'l')){
		  tmp_Date_Month = 7;
	 }
	  else if((Date_Month[0] == 'A')&(Date_Month[1] == 'u')&(Date_Month[2] == 'g')){
		  tmp_Date_Month = 8;
	 }
	  else if((Date_Month[0] == 'S')&(Date_Month[1] == 'e')&(Date_Month[2] == 'p')){
		  tmp_Date_Month = 9;
	 }
	  else if((Date_Month[0] == 'O')&(Date_Month[1] == 'c')&(Date_Month[2] == 't')){
	      tmp_Date_Month = 10;
	}
	  else if((Date_Month[0] == 'N')&(Date_Month[1] == 'o')&(Date_Month[2] == 'v')){
		  tmp_Date_Month = 11;
	 }
	  else if((Date_Month[0] == 'D')&(Date_Month[1] == 'e')&(Date_Month[2] == 'c')){
		  tmp_Date_Month = 12;
	 }

	  F_Version_Year = tmp_Date_Year;

	  F_Version_Month = tmp_Date_Month;

	  if(Date_Day[0] < 48){
		  tmp_Date_Day =(Date_Day[1] - 48);
	  }
	  else{
		  tmp_Date_Day = (Date_Day[0]-48)*10 + (Date_Day[1] - 48);
	  }
	  F_Version_Day = tmp_Date_Day;


	  uint8_t Date_Hour_tmp[8];

	  for(int i=0; i<8; i++){
		  Date_Hour_tmp[i] = __TIME__[i];
	  }
	  for(int i=0; i<2; i++){
		  Date_Hour[i] = Date_Hour_tmp[i];
		  Date_Min[i] = Date_Hour_tmp[i+3];
		  Date_Sec[i] = Date_Hour_tmp[i+6];
	  }

	  F_Version_Hour = (Date_Hour[0]-48)*10 + (Date_Hour[1] - 48);

	  F_Version_Min = (Date_Min[0]-48)*10 + (Date_Min[1] - 48);

	  F_Version_Sec = (Date_Sec[0]-48)*10 + (Date_Sec[1] - 48);

	  ////////////////////////////
}
