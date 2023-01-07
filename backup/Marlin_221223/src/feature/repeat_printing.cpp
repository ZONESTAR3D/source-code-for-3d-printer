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
#include "../module/endstops.h"
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
bool RePrint::is_repeatPrinting = false;
bool RePrint::is_ArmHomed = false;
bool RePrint::is_ArmL_Stopped = false;
bool RePrint::is_ArmR_Stopped = false;
bool RePrint::gotReferenceBedTemp = false;

char rePrint_filename[20] = {0};
char rePrint_filename_next[20] = {0};
int16_t RePrint::RepeatTimes = 0;
int16_t RePrint::Push_length = DEFAULT_REPRINT_ARM_LENGHT;
int16_t RePrint::arm_current_pos = 0;
int16_t RePrint::Bedtemp = DEFAULT_REPRINT_BEDTEMP;
int16_t RePrint::RePrintZHeigth = DEFAULT_REPRINT_ZHEIGTH;
#if HAS_REPEATPRINT_BASE
float RePrint::RePrintBaseHeigth = DEFAULT_REPEATPRINT_BASE_HEIGTH;
#endif
int16_t RePrint::RePrint_wait_seconds = WAIT_SECONDS_AFTER_BEDCOOL;
uint8_t RePrint::RePrint_status = REPRINT_IDLE;

millis_t Reprint_timer;
//Push: N = HIGH, P = LOW 
//Pull: N = LOW, P = HIGH 
//Brake: N = HIGH, P = HIGH, 
//Stop: N = LOW, P = LOW 
void RePrint::RepeatPrint_ArmPort_Init() {
	SET_INPUT_PULLUP(RPARML_MIN_PIN);
	SET_INPUT_PULLUP(RPARMR_MIN_PIN);
	
  OUT_WRITE(RP_ARMLP_PIN, LOW);
	OUT_WRITE(RP_ARMLN_PIN, LOW);	
	OUT_WRITE(RP_ARMRP_PIN, LOW);
	OUT_WRITE(RP_ARMRN_PIN, LOW);
}

void RePrint::RepeatPrint_Arm_Push(const uint8_t lor) {  
	if(TEST(lor, BIT_ARM_L)){
		is_ArmL_Stopped = false;		
		OUT_WRITE(RP_ARMLP_PIN, LOW);
		OUT_WRITE(RP_ARMLN_PIN, LOW);
	}
	if(TEST(lor, BIT_ARM_R)){
		is_ArmR_Stopped = false;		
		OUT_WRITE(RP_ARMRP_PIN, LOW);
		OUT_WRITE(RP_ARMRN_PIN, LOW);
  }
	safe_delay(5);
	if(TEST(lor, BIT_ARM_L)) OUT_WRITE(RP_ARMLP_PIN, HIGH);
	if(TEST(lor, BIT_ARM_R)) OUT_WRITE(RP_ARMRP_PIN, HIGH);	
	safe_delay(1);	
}

void RePrint::RepeatPrint_Arm_Pull(const uint8_t lor) {
	if(TEST(lor, BIT_ARM_L)){		
		OUT_WRITE(RP_ARMLP_PIN, LOW);
		OUT_WRITE(RP_ARMLN_PIN, LOW);
	}
	if(TEST(lor, BIT_ARM_R)){		
		OUT_WRITE(RP_ARMRP_PIN, LOW);
		OUT_WRITE(RP_ARMRN_PIN, LOW);
  }
	safe_delay(5);
	if(TEST(lor, BIT_ARM_L)) OUT_WRITE(RP_ARMLN_PIN, HIGH);
	if(TEST(lor, BIT_ARM_R)) OUT_WRITE(RP_ARMRN_PIN, HIGH);	
	safe_delay(1);		
}

void RePrint::RepeatPrint_Arm_Stop(const uint8_t lor) {
	if(TEST(lor, BIT_ARM_L) && !is_ArmL_Stopped){
		is_ArmL_Stopped = true;
		OUT_WRITE(RP_ARMLP_PIN, HIGH);
		OUT_WRITE(RP_ARMLN_PIN, HIGH);		
	}
	if(TEST(lor, BIT_ARM_R) && !is_ArmR_Stopped){
		is_ArmR_Stopped = true;		
		OUT_WRITE(RP_ARMRP_PIN, HIGH);
		OUT_WRITE(RP_ARMRN_PIN, HIGH);
	}
	safe_delay(10);
	if(TEST(lor, BIT_ARM_L)){
		OUT_WRITE(RP_ARMLP_PIN, LOW);
		OUT_WRITE(RP_ARMLN_PIN, LOW);
		
	}
	if(TEST(lor, BIT_ARM_R)){
		OUT_WRITE(RP_ARMRP_PIN, LOW);
		OUT_WRITE(RP_ARMRN_PIN, LOW);		
	}
	safe_delay(1);
}

bool RePrint::RepeatPrint_HomeArm(bool isblocked /*= true*/) {	
	uint8_t select_arm = 0;
	millis_t now = millis();
	millis_t next_temp_ms = now + 1000UL;

	is_ArmHomed = false;
	TERN_(HAS_DWIN_LCD, Updata_RePrint_Popup_Window(REPRINT_HOMING));
	TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Homing the arm...")));
	TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Home the arm")));
  if(!TEST(endstops.state(), RPARML_MIN)) 	select_arm |= (1<<BIT_ARM_L);	
	if(!TEST(endstops.state(), RPARMR_MIN)) 	select_arm |= (1<<BIT_ARM_R);
	if(select_arm){
		RepeatPrint_Arm_Pull(select_arm);		
		Reprint_timer = millis() + ARM_MM_TO_MS(MAX_REPRINT_ARM_LENGHT);		
	}
	do{
		now = millis();
		if(isblocked){
			if(ELAPSED(now, next_temp_ms)){
				next_temp_ms = now + 1000UL;
				TERN_(USE_WATCHDOG, HAL_watchdog_refresh());
			}
		}
		else{
			if(ELAPSED(now, next_temp_ms)){
				next_temp_ms = now + 100UL;
				idle();
			}
		}
		if(TEST(endstops.state(), RPARML_MIN))	RepeatPrint_Arm_Stop(1<<BIT_ARM_L);
		if(TEST(endstops.state(), RPARMR_MIN))	RepeatPrint_Arm_Stop(1<<BIT_ARM_R);
		if(TEST(endstops.state(), RPARML_MIN) && TEST(endstops.state(), RPARMR_MIN)){ 
			is_ArmHomed = true;
			arm_current_pos = 0;
		}
	}while(PENDING(now, Reprint_timer) && !is_ArmHomed);
		
	if(!is_ArmHomed) {
		RepeatPrinting_Reset();	
		TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Home fail, check endstop!")));
		TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Home fail!")));	
		return false;
	}
	else {
		//Home Bumping
		RepeatPrint_Arm_Push((1<<BIT_ARM_L)|(1<<BIT_ARM_R));				
		Reprint_timer = now + ARM_MM_TO_MS(REPRINT_ARM_HOMEBUMP_LENGTH);
		TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Homing bump...")));
		TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Home bump")));	
		do{
			now = millis();
			if(isblocked){
				if(ELAPSED(now, next_temp_ms)){
					next_temp_ms = now + 100UL;
					TERN_(USE_WATCHDOG, HAL_watchdog_refresh());
				}
			}
			else{
				if(ELAPSED(now, next_temp_ms)){
					next_temp_ms = now + 100UL;
					idle();
				}
			}		
		}while(PENDING(now, Reprint_timer));
		RepeatPrint_Arm_Stop((1<<BIT_ARM_L)|(1<<BIT_ARM_R));
		TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Arm homed")));
		TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Arm homed")));	
	}
	return true;
}

void RePrint::RepeatPrint_MoveArm(const int16_t pos/*=DEFAULT_REPRINT_ARM_LENGHT*/, bool isblocked /*=true*/){
	millis_t now = millis();	
	millis_t next_temp_ms = now;
	uint8_t select_arm = 0;
	int16_t newpos = pos;	
	NOLESS(newpos, 0);
	NOMORE(newpos, MAX_REPRINT_ARM_LENGHT);
	
	int16_t move_length = newpos - arm_current_pos;	
	if(move_length == 0) return;
	
	TERN_(HAS_DWIN_LCD, Updata_RePrint_Popup_Window(REPRINT_PUSHING));	
	is_ArmHomed = false;
	if(move_length > 0){
		TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Pushing the arm..."), 30));
		TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Push the arm")));		
		RepeatPrint_Arm_Push((1<<BIT_ARM_L)|(1<<BIT_ARM_R));	
		Reprint_timer = now + ARM_MM_TO_MS(move_length);	
		do {
			now = millis();
			if(isblocked){
				if(ELAPSED(now, next_temp_ms)){
					next_temp_ms = now + 1000UL;
					TERN_(USE_WATCHDOG, HAL_watchdog_refresh());
				}
			}
			else{
				if(ELAPSED(now, next_temp_ms)){
					next_temp_ms = now + 100UL;
					idle();
				}
			}
		}while(PENDING(now, Reprint_timer));
		RepeatPrint_Arm_Stop((1<<BIT_ARM_L)|(1<<BIT_ARM_R));
		TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Push done!")));
		TERN_(HAS_LCD_MENU, ui.reset_status());			
	}
	else {	
		TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Pulling the arm..."), 30));
		TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Pulling the arm")));
	  if(!TEST(endstops.state(), RPARML_MIN)) 	select_arm |= (1<<BIT_ARM_L);	
		if(!TEST(endstops.state(), RPARMR_MIN)) 	select_arm |= (1<<BIT_ARM_R);
		if(select_arm){
			RepeatPrint_Arm_Pull(select_arm);		
			Reprint_timer = millis() + ARM_MM_TO_MS(-move_length);		
		}
		do{
			now = millis();
			if(isblocked){
				if(ELAPSED(now, next_temp_ms)){
					next_temp_ms = now + 1000UL;
					TERN_(USE_WATCHDOG, HAL_watchdog_refresh());
				}
			}
			else{
				if(ELAPSED(now, next_temp_ms)){
					next_temp_ms = now + 100UL;
					idle();
				}
			}
			if(TEST(endstops.state(), RPARML_MIN))	RepeatPrint_Arm_Stop(1<<BIT_ARM_L);
			if(TEST(endstops.state(), RPARMR_MIN))	RepeatPrint_Arm_Stop(1<<BIT_ARM_R);
			if(TEST(endstops.state(), RPARML_MIN) && TEST(endstops.state(), RPARMR_MIN)) 
				is_ArmHomed = true;
		}while(PENDING(now, Reprint_timer) && !is_ArmHomed);
		TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Pull done!")));
		TERN_(HAS_LCD_MENU, ui.reset_status());	
	}	
}

void RePrint::getRefernceBedTemp(const float hotend_temp, const float bed_temp) {
	if(!gotReferenceBedTemp) {
		gotReferenceBedTemp = true;
		if(ABS(hotend_temp - bed_temp) <= 2 && bed_temp < DEFAULT_REPRINT_BEDTEMP){
			if(bed_temp < 20)
				Bedtemp = 20;
			else
				Bedtemp = (int16_t)bed_temp + 1;
		}
		else
			Bedtemp = DEFAULT_REPRINT_BEDTEMP;
	}
}

void RePrint::RepeatPrinting_Reset() {
	enabled = false;
	RepeatTimes = 0;	
	is_ArmHomed = false;
	is_repeatPrinting = false;
	RePrint_status = REPRINT_IDLE;	
}

void RePrint::initialize() {	
	RepeatPrint_ArmPort_Init();
	RepeatPrinting_Reset();
}

char string_buf[50] = {0};

static millis_t RepeatPrint_temp_ms = millis();
void RePrint::RepeatPrinting_process() {	
	
	if(!enabled) return;
	millis_t now = millis();
	static uint8_t count = 0;
	switch(RePrint_status){
		default:			
			RePrint_status = REPRINT_IDLE;
			break;
			
		case REPRINT_PREPARE:
			planner.synchronize();			
			thermalManager.setTargetBed(0);
			TERN_(HAS_DWIN_LCD, Popup_Window_RepeatPrint());
			TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Start repeat printing..."),10));
			TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Start repeat print")));		
			RePrint_status = REPRINT_HOMING;
			break;
			
		case REPRINT_HOMING:		
			if(!TEST(axis_homed, X_AXIS) || !TEST(axis_homed, Y_AXIS) || current_position.x > 0 || current_position.y > 0)
			{			
				queue.inject_P(PSTR("G28 XY"));
				planner.synchronize();
			}
			if(!is_ArmHomed) {
				queue.inject_P(PSTR("M180"));
				planner.synchronize();				
			}			
			RePrint_status = REPRINT_PREPARE_STARTCOOL;
			break;

		case REPRINT_PREPARE_STARTCOOL:
			//Homing Arm
			TERN_(HAS_DWIN_LCD, Updata_RePrint_Popup_Window(REPRINT_COOLDOWN));			
			sprintf_P(string_buf, PSTR("Wait bed cool ( < %2d )."), Bedtemp);
			TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, string_buf));
			TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Hotbed cool down")));	
			RepeatPrint_temp_ms = now + 1000UL;
			RePrint_status = REPRINT_COOLDOWN;		
			break;
			
		case REPRINT_COOLDOWN:
			//Wait hotbed Cooldown			
			if(thermalManager.degBed() > Bedtemp){
				if(ELAPSED(now, RepeatPrint_temp_ms)){
					DWIN_FEEDBACK_TICK();
					RepeatPrint_temp_ms = now + 1000UL;
					count++;
					sprintf_P(string_buf, PSTR("Wait bed cool ( < %2d )."), Bedtemp);
					for(uint8_t i = 0; i < count%10; i++) strcat(string_buf, PSTR("."));
					TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, string_buf));
					TERN_(HAS_LCD_MENU, ui.set_status(string_buf));	
				}
			}
			else {				
				RePrint_status = REPRINT_PREPARE_YMOVE;
			}
			break;

		case REPRINT_PREPARE_YMOVE:
			queue.inject_P(PSTR("G90\nG1 X150 Y305 F3600"));
			planner.synchronize();
			RePrint_status = REPRINT_PREPARE_ZMOVE;
			break;
			
		case REPRINT_PREPARE_ZMOVE:
			if(TEST(axis_known_position, Z_AXIS)){
				sprintf_P(string_buf,PSTR("G1 Z%d F480"), RePrintZHeigth);
				queue.inject(string_buf);
				planner.synchronize();
			}
			TERN_(HAS_DWIN_LCD, Updata_RePrint_Popup_Window(REPRINT_COOLDOWN_WAIT));
			TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Wait more seconds...")));
			TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Wait more seconds")));				
			RePrint_wait_seconds = WAIT_SECONDS_AFTER_BEDCOOL;
			RepeatPrint_temp_ms = now + 1000UL;
			RePrint_status = REPRINT_COOLDOWN_WAIT;		
			break;
			
		case REPRINT_COOLDOWN_WAIT:
			//Wait more seconds					
			if (ELAPSED(now, RepeatPrint_temp_ms)){
				RepeatPrint_temp_ms = now + 1000UL;
				RePrint_wait_seconds--;
				DWIN_FEEDBACK_TIPS();
				TERN_(HAS_DWIN_LCD, Updata_RePrint_Popup_Window(REPRINT_COOLDOWN_WAIT));
				if(RePrint_wait_seconds == 0)	RePrint_status = REPRINT_PUSHING;				
			}
			break;

		case REPRINT_PUSHING:						
			RepeatPrint_MoveArm(Push_length, false);
			//queue.inject_P(PSTR("M181"));	
			//planner.synchronize();
			RePrint_status = REPRINT_HOMEAGAIN;
			break;

		case REPRINT_HOMEAGAIN:
			TERN_(HAS_DWIN_LCD, Updata_RePrint_Popup_Window(REPRINT_HOMING));
			RepeatPrint_HomeArm(false);
			//queue.inject_P(PSTR("G28 XY"));
			//planner.synchronize();
			//RePrint_status = REPRINT_CHECK_FILEEXIST;
			RePrint_status = REPRINT_PRINTNEXTONT;
			break;
	
		case REPRINT_PRINTNEXTONT:
			TERN_(HAS_DWIN_LCD, Updata_RePrint_Popup_Window(REPRINT_PRINTNEXTONT));
			sprintf_P(string_buf, PSTR("Start print %s"), rePrint_filename);
			TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, string_buf));
			TERN_(HAS_LCD_MENU, ui.set_status(string_buf));
			RepeatPrint_temp_ms = now + 3000UL;
			do{
				now = millis();
				idle();
			}while(PENDING(now, RepeatPrint_temp_ms));			
			
			DWIN_FEEDBACK_CONFIRM();
			card.openAndPrintFile(rePrint_filename);			
			RePrint_status = REPRINT_IDLE;
			break;
	}
}

void RePrint::CheckandStart_RepeatPrint() { 
	if(!enabled) return;
	
	if(is_repeatPrinting) {
		//check repeat printing next one
		if(RepeatTimes <= 0) {
			RepeatPrinting_Reset();
			return;
		}
		RepeatTimes--;		
		if(RepeatTimes == 0){
			//Repeat Finished
			if(strstr(rePrint_filename_next, ".gco")){
				//print a new file
				TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Print the next file!")));
				TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Print next file!")));
				ZERO(rePrint_filename);
				strcpy(rePrint_filename, rePrint_filename_next);
				ZERO(rePrint_filename_next);
				is_repeatPrinting = false;
				RePrint_status = REPRINT_PREPARE;
				return;
			}
			else{
				TERN_(HAS_DWIN_LCD, DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Repeat printing finished!")));
				TERN_(HAS_LCD_MENU, ui.set_status_P(PSTR("Repeat Finished!")));
				RepeatPrinting_Reset();
				return;
			}
		}
		else {
			//Repeat next times
			is_repeatPrinting = true;
			RePrint_status = REPRINT_PREPARE;
			return;
		}
	}
	else {
		if(RepeatTimes > 0){
			//First Repeat
			is_repeatPrinting = true;
			RePrint_status = REPRINT_PREPARE;
			return;
		}
		else {
			RepeatPrinting_Reset();
			return;
		}
	}
}
#endif // REPEAT_PRINTING_CONTROL
