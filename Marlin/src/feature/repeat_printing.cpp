/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/**
 * repeat_printing.cpp - repeat printing control
 */

#include "../inc/MarlinConfig.h"

#if ENABLED(OPTION_REPEAT_PRINTING)

#include "repeat_printing.h"
#include "../module/temperature.h"
#include "../module/stepper/indirection.h"
#include "../MarlinCore.h"

#if HAS_DWIN_LCD
#include "../lcd/dwin/dwin_lcd.h"
#include "../lcd/dwin/dwin_ui/dwin.h"
#include "../lcd/dwin/dwin_ui/dwinmenu_repeatprint.h"
#endif

RePrint ReprintManager;

bool RePrint::enabled = false;
		 
int16_t RePrint::Reprint_times = REPEAT_PRINTING_TIMES;
int16_t RePrint::Forward_lenght = REPEAT_PRINTING_PUSH_LENGHT;
millis_t RePrint::reprt_timer = 0;
RePrint_Armstate_t RePrint::reprt_state = REPRINT_ARM_IDLE;
float RePrint::bedtemp_threshold = 25.0;

static void RePrint::RepeatPrint_Arm_Init() {
	SET_INPUT_PULLUP(RPL_MIN_PIN);
	SET_INPUT_PULLUP(RPR_MIN_PIN);
  OUT_WRITE(RP_LFPRWARD_PIN,LOW);
  OUT_WRITE(RP_LBACK_PIN,LOW);
	OUT_WRITE(RP_RFPRWARD_PIN,LOW);
	OUT_WRITE(RP_RBACK_PIN,LOW);
}

static void RePrint::RepeatPrint_Arm_Push(const uint8_t lor) {  
	if(TEST(lor, 0)){
		OUT_WRITE(RP_LFPRWARD_PIN,LOW);
		OUT_WRITE(RP_LBACK_PIN,LOW);
	}
	if(TEST(lor, 1)){
		OUT_WRITE(RP_RFPRWARD_PIN,LOW);
		OUT_WRITE(RP_RBACK_PIN,LOW);
  }
	safe_delay(5);
	if(TEST(lor, 0)) OUT_WRITE(RP_LFPRWARD_PIN,HIGH);
	if(TEST(lor, 1)) OUT_WRITE(RP_RFPRWARD_PIN,HIGH);	
	safe_delay(1);	
}

static void RePrint::RepeatPrint_Arm_Back(const uint8_t lor) {
	if(TEST(lor, 0)){
		OUT_WRITE(RP_LFPRWARD_PIN,LOW);
		OUT_WRITE(RP_LBACK_PIN,LOW);
	}
	if(TEST(lor, 1)){
		OUT_WRITE(RP_RFPRWARD_PIN,LOW);
		OUT_WRITE(RP_RBACK_PIN,LOW);
  }
	safe_delay(5);
	if(TEST(lor, 0)) OUT_WRITE(RP_LBACK_PIN,HIGH);
	if(TEST(lor, 1)) OUT_WRITE(RP_RBACK_PIN,HIGH);	
	safe_delay(1);		
}

static void RePrint::RepeatPrint_Arm_Stop(const uint8_t lor) {
	if(TEST(lor, 0)){
		OUT_WRITE(RP_LFPRWARD_PIN,HIGH);
		OUT_WRITE(RP_LBACK_PIN,HIGH);
	}
	if(TEST(lor, 1)){
		OUT_WRITE(RP_RFPRWARD_PIN,HIGH);
		OUT_WRITE(RP_RFPRWARD_PIN,HIGH);
	}
	safe_delay(10);
	if(TEST(lor, 0)){
		OUT_WRITE(RP_LFPRWARD_PIN,LOW);
		OUT_WRITE(RP_LBACK_PIN,LOW);
	}
	if(TEST(lor, 1)){
		OUT_WRITE(RP_RFPRWARD_PIN,LOW);
		OUT_WRITE(RP_RFPRWARD_PIN,LOW);
	}
	safe_delay(1);
}

static void RePrint::RepeatPrint_HomeArm() {
	uint8_t select_arm = 0;
  if(TEST(endstops.state(), RPL_MIN))
		SBI(endstops_state, 0);
	else{
		CBI(endstops_state, 0);
		select_arm += 1;
	}
	if(TEST(endstops.state(), RPR_MIN)){
		SBI(endstops_state, 1);
	}
	else{
		CBI(endstops_state, 1);
		select_arm += 2;		
	}
	RepeatPrint_Arm_Back(select_arm);	
	reprt_state = REPRINT_ARM_HOMING;
}

static void RePrint::RepeatPrint_PushArm(){
	if(Forward_lenght < 10 || Forward_lenght > REPEAT_PRINTING_ARM_LENGHT) 
		return;
	TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_RED, PSTR("Push the Arm, please wait!")));
	RepeatPrint_Arm_Push(ARM_ALL);		
  reprt_timer = millis() + MM_TO_MS(Forward_lenght);
	reprt_state = REPRINT_ARM_PUSHING;
}

static void RePrint::Init() {
	const float start_bedtemp = thermalManager.degBed();
	
	RepeatPrint_Arm_Init();
	enabled = false;
	if(start_bedtemp >= 30){
		bedtemp_threshold = 30;
	}
	else if(start_bedtemp > 25 && start_bedtemp < 30)
		bedtemp_threshold = start_bedtemp;
	else
		bedtemp_threshold = 25;	
	Reprint_times = REPEAT_PRINTING_TIMES;
}

static bool RePrint::RepeatPrint_ArmControl() {
	const millis_t now = millis();
  switch(reprt_state){
		default:
		case REPRINT_ARM_IDLE:
		break;
		
  	case REPRINT_ARM_INIT:
			RepeatPrint_Arm_Init();
			RepeatPrint_HomeArm();
			reprt_state = REPRINT_ARM_HOMING;
		break;
		
	case REPRINT_ARM_HOMING:
		if(TEST(endstops.state(), RPL_MIN) && !TEST(endstops_state,0)){
			SBI(endstops_state,0);
			RepeatPrint_Arm_Stop(ARM_L);
		}
		if(TEST(endstops.state(), RPR_MIN) && !TEST(endstops_state,1)){
			SBI(endstops_state,1);
			RepeatPrint_Arm_Stop(ARM_R);
		}
		//Bump 2 mm
		if(TEST(endstops.state(), RPL_MIN) && TEST(endstops_state,0) && TEST(endstops.state(), RPR_MIN) && TEST(endstops_state,1)){
			RepeatPrint_Arm_Push(ARM_ALL);
			reprt_timer = millis() + MM_TO_MS(2);
			reprt_state = REPRINT_ARM_HOMING_BUMP;
		}
		break;

	case REPRINT_ARM_HOMING_BUMP:
		if(now >= reprt_timer){
			RepeatPrint_Arm_Stop(ARM_ALL);
			reprt_state = REPRINT_ARM_IDLE;
		}
		break;

	case REPRINT_ARM_PUSHING:
		if(now - reprt_timer >= 0){
			RepeatPrint_Arm_Stop(ARM_ALL);
			reprt_state = REPRINT_ARM_IDLE;
		}
		break;
  } 
}

#endif // REPEAT_PRINTING_CONTROL
