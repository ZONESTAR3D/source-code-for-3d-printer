#include "../../../inc/MarlinConfig.h"
#include "dwin.h"

#if (HAS_DWIN_LCD && ENABLED(DWIN_AUTO_TEST))
#include <WString.h>
#include <stdio.h>
#include <string.h>

#include "autotest.h"
#include "../../../core/serial.h"
#include "../../../core/macros.h"

#include "../../../module/stepper.h"
#include "../../../module/motion.h"
#include "../../../module/planner.h"
#include "../../../module/temperature.h"
#include "../../../module/tool_change.h"

#include "../../../gcode/gcode.h"
#include "../../../gcode/queue.h"

#include "../../../sd/cardreader.h"
#include "../../../MarlinCore.h"
#include "../../../libs/buzzer.h"
#include "../../../HAL/shared/Delay.h"

_stAutotest_t Autotest::testflag;
Autotest autotest;
uint8_t Autotest::swstatus[6] = {0,0,0,0,0,0};
uint8_t Autotest::old_swstatus[6] = {0,0,0,0,0,0};

void Autotest::Check_Rotary(){
	ENCODER_DiffState encoder_diffState = get_encoder_state();

	if (encoder_diffState == ENCODER_DIFF_NO) return;	
	if (encoder_diffState == ENCODER_DIFF_CW || encoder_diffState == ENCODER_DIFF_CCW) DWIN_FEEDBACK_TICK();
	if (encoder_diffState == ENCODER_DIFF_CW && testflag.rotary_counter < 255) testflag.rotary_counter++;
	else if(encoder_diffState == ENCODER_DIFF_CCW && testflag.rotary_counter > 0) testflag.rotary_counter--;
	else if(encoder_diffState == ENCODER_DIFF_ENTER) {
		DWIN_FEEDBACK_TIPS();
		testflag.click_counter++;
		if(!testflag.autorun && testflag.click_counter >= 3) testflag.state = CHECK_END;							//exist
		if(testflag.rotary_counter == 43 && testflag.click_counter == 17) testflag.state = CHECK_END;	//exist
	}
	if(testflag.autorun) Autotest_ShowKnob();
}

inline void Autotest::Autotest_ShowKnob(){	
	if(testflag.click_counter > 5)
		DRAW_INT_GREEN_FONT12(COLOR_BG_BLUE, 3, LSTART+XCENTER(6), YPOS_MSG(ID_LINE_KNOB), testflag.click_counter);
	else
		DRAW_INT_WHITE_FONT12(COLOR_BG_BLUE, 3, LSTART+XCENTER(6), YPOS_MSG(ID_LINE_KNOB), testflag.click_counter);		
	if(testflag.rotary_counter > 10)
		DRAW_INT_GREEN_FONT12(COLOR_BG_BLUE, 3, LSTART+7*WIDTH, YPOS_MSG(ID_LINE_KNOB), testflag.rotary_counter);
	else
		DRAW_INT_WHITE_FONT12(COLOR_BG_BLUE, 3, LSTART+7*WIDTH, YPOS_MSG(ID_LINE_KNOB), testflag.rotary_counter);
}

inline void Autotest::AutoTest_ShowTemperature(){
	static float test_temp_hotend_target = -100;
	static float test_temp_hotbed_target = -100;
	static float test_temp_hotend_celsius = -100;
	static float test_temp_hotbed_celsius = -100;	
	//hotend temperature
	if(ABS(test_temp_hotend_celsius - thermalManager.degHotend(0)) >=1 ){
		DRAW_INT_WHITE_FONT12(COLOR_BG_WINDOW, 3, XCENTER(0), YPOS_MSG(ID_LINE_ETEMP), thermalManager.temp_hotend[0].celsius);
		test_temp_hotend_celsius = thermalManager.degHotend(0);
	}
	if(ABS(test_temp_hotend_target - thermalManager.degTargetHotend(0)) >= 1){
		DRAW_INT_WHITE_FONT12(COLOR_BG_WINDOW, 3, XCENTER(4), YPOS_MSG(ID_LINE_ETEMP), thermalManager.temp_hotend[0].target);
		test_temp_hotend_target = thermalManager.degTargetHotend(0);
	}
	//bed temperature
	if(ABS(test_temp_hotbed_celsius - thermalManager.degBed()) >= 1){		
		test_temp_hotbed_celsius = thermalManager.degBed();
		DRAW_INT_WHITE_FONT12(COLOR_BG_WINDOW, 3, XCENTER(0), YPOS_MSG(ID_LINE_BTEMP), (uint32_t)test_temp_hotbed_celsius);
	}
	if(ABS(test_temp_hotbed_target - thermalManager.degTargetBed()) >= 1 ){		
		test_temp_hotbed_target = thermalManager.degTargetBed();
		DRAW_INT_WHITE_FONT12(COLOR_BG_WINDOW, 3, XCENTER(4), YPOS_MSG(ID_LINE_BTEMP), (uint32_t)test_temp_hotbed_target);
	}
	//FAN
	if((thermalManager.degHotend(0) <= AUTO_FAN_TEMPERATURE) && (testflag.state > CHECK_FANS)){		
		if(!testflag.fan_fg){
			thermalManager.set_fan_speed(0, 0);
			testflag.fan_fg = true;
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_FAN), DWIN_WIDTH, YPOS(ID_LINE_FAN)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_FAN), PSTR("ALL Fan Off"));
		}
	}
}

inline void Autotest::AutoTest_ShowSWStatus(bool bfirst){
	if(bfirst){
		dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 0, YPOS(ID_LINE_SW), DWIN_WIDTH, YPOS(ID_LINE_SW_RESULT)+ROW_GAP);
		dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 0, YPOS(ID_LINE_SW_STATE), DWIN_WIDTH,  YPOS(ID_LINE_SW_STATE)+ROW_GAP);
		DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_SW), PSTR(" X "));
		DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART+1*SWWIDTH, YPOS_MSG(ID_LINE_SW), PSTR(" Y "));
		DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART+2*SWWIDTH, YPOS_MSG(ID_LINE_SW), PSTR("ZL "));
		DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART+3*SWWIDTH, YPOS_MSG(ID_LINE_SW), PSTR("ZR "));
		DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART+4*SWWIDTH, YPOS_MSG(ID_LINE_SW), PSTR(" F "));
		DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART+5*SWWIDTH, YPOS_MSG(ID_LINE_SW), PSTR(" S "));		
	}		
	//X	
	if(old_swstatus[0] != swstatus[0] || bfirst)
		DRAW_STRING_FONT10((swstatus[0]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[0]==0x07));
	//Y
	if(old_swstatus[1] != swstatus[1] || bfirst)
		DRAW_STRING_FONT10((swstatus[1]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART+1*SWWIDTH, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[1]==0x07));
	//Z
	if(old_swstatus[2] != swstatus[2] || bfirst)
		DRAW_STRING_FONT10((swstatus[2]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART+2*SWWIDTH, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[2]==0x07));
	//Z2
#if PIN_EXISTS(Z2_MIN)
	if(old_swstatus[3] != swstatus[3] || bfirst)
		DRAW_STRING_FONT10((swstatus[3]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART+3*SWWIDTH, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[3]==0x07));
#endif
	//runout sensor
#if PIN_EXISTS(FIL_RUNOUT)
	if(old_swstatus[4] != swstatus[4] || bfirst)
		DRAW_STRING_FONT10((swstatus[4]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART+4*SWWIDTH, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[4]==0x07));		
#endif
	//Probe
#if PIN_EXISTS(Z_MIN_PROBE)
	if(old_swstatus[5] != swstatus[5] || bfirst)
		DRAW_STRING_FONT10((swstatus[5]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART+5*SWWIDTH, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[5]==0x07));
#endif
}

inline void Autotest::AutoTest_Watch_SW(){
	//X
	swstatus[0] <<= 1;
	if(READ(X_MIN_PIN) != X_MIN_ENDSTOP_INVERTING) swstatus[0] |= 0x01; else swstatus[0] &= 0xfe;		
	//Y
	swstatus[1] <<= 1;
	if(READ(Y_MIN_PIN) != Y_MIN_ENDSTOP_INVERTING) swstatus[1] |= 0x01; else swstatus[1] &= 0xfe;
	//Z
	swstatus[2] <<= 1;
	if(READ(Z_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING) swstatus[2] |= 0x01; else swstatus[2] &= 0xfe;
	//Z2
#if PIN_EXISTS(Z2_MIN)
	swstatus[3] <<= 1;
	if(READ(Z2_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING) swstatus[3] |= 0x01; else swstatus[3] &= 0xfe;	
#endif
	//FROD
#if PIN_EXISTS(FIL_RUNOUT)
	swstatus[4] <<= 1;
	if(READ(FIL_RUNOUT_PIN) == FIL_RUNOUT_STATE) swstatus[4] |= 0x01; else swstatus[4] &= 0xfe;	
#endif
	//Leveling sensor
#if PIN_EXISTS(Z_MIN_PROBE)
	swstatus[5] <<= 1;
	if(READ(Z_MIN_PROBE_PIN) != Z_MIN_PROBE_ENDSTOP_INVERTING) swstatus[5] |= 0x01; else swstatus[5] &= 0xfe;	
#endif
	swstatus[0] &= 0x07;
	swstatus[1] &= 0x07;
	swstatus[2] &= 0x07;
	swstatus[3] &= 0x07;
	swstatus[4] &= 0x07;
	swstatus[5] &= 0x07;
}


void Autotest::AutoTest_Loop() {
	static millis_t test_next_rts_update_ms = 0;
	static uint16_t test_counter = 0;
	static uint16_t test_timer;
	static uint16_t test_seconds;
	static bool test_dir;
	static bool hotend_ok = false;
	static bool hotbed_ok = false;
	static float test_temp_hotend_first = -100.0;
	static float test_temp_hotbed_first = -100.0;
	
	if(testflag.state == CHECK_Z_MOTOR){
		stepper.do_Zaxis_step(test_counter >= 6,test_dir);
		DELAY_US(150);
	}
	
	Check_Rotary();
	const millis_t test_ms = millis();
	if (PENDING(test_ms, test_next_rts_update_ms)) return;
	test_next_rts_update_ms = test_ms + 10;
	
	if(testflag.state >= CHECK_SD) AutoTest_ShowTemperature();
	
	switch(testflag.state){
		default:
		case CHECK_START:
			testflag.state = CHECK_SD;
			test_timer = 0;
			test_seconds = 0;
			test_dir = false;
			break;
			
		case CHECK_SD:		
			if(IS_SD_INSERTED()){
				dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD2)+ROW_GAP);
				DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("SD Card OK!"));
			  DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD2), PSTR("SD Size(M):"));
				DRAW_INT_RED_FONT12(COLOR_BG_BLACK, 5, (strlen("SD Size(M):")+1)*WIDTH, YPOS_MSG(ID_LINE_SD2), CardReader::sd2card.cardSize()/2048);				
		 		test_timer = 0;
				if(testflag.autorun)
					testflag.state = CHECK_PREPARE_HEAT;
				else 
					testflag.state = CHECK_FINISHED;
			}
			else{
				if(test_timer++ >= 50) {
					test_timer = 0;
					test_dir = !test_dir;
					if(test_dir){
						dwinLCD.Draw_Rectangle(1, COLOR_BG_RED, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD2)+ROW_GAP);
						DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_RED, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("  SD Card Not Find!  "));
						DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_RED, LSTART, YPOS_MSG(ID_LINE_SD2), PSTR("  Insert to continue "));
					}
					else{
						dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD2)+ROW_GAP);
						DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("  SD Card Not Find!  "));
						DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD2), PSTR("  Insert to continue "));
						test_seconds++;
						if(test_seconds > 20 && !testflag.autorun) testflag.state = CHECK_FINISHED;
					}
				}
			}
		break;

	 case CHECK_PREPARE_HEAT:
	 		if(test_timer++ >= 100){
				// 1s later
				test_timer = 0;				
				//check current temp of hotend
				test_temp_hotend_first = thermalManager.degHotend(0);
				hotend_ok = false;
				if(test_temp_hotend_first < 5){					
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Hotend too cool!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
				else if(test_temp_hotend_first > 150){					
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Hotend too hot!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
				
				//check current temp of hotbed
				test_temp_hotbed_first = thermalManager.degBed();
				hotbed_ok = false;
				if(test_temp_hotbed_first < 5){					
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hot bed too cool!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
				else if(test_temp_hotbed_first > 60){					
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hot bed too hot!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
				thermalManager.setTargetHotend(_MAX(test_temp_hotend_first + 20, 80), 0);
				dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 0, YPOS(ID_LINE_ETEMP_INFO), DWIN_WIDTH, YPOS(ID_LINE_ETEMP_INFO)+ROW_GAP);
				DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Hot end Heating..."));
				thermalManager.setTargetBed(_MAX(test_temp_hotbed_first + 20, 60));
				dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 0, YPOS(ID_LINE_BTEMP_INFO), DWIN_WIDTH, YPOS(ID_LINE_BTEMP_INFO)+ROW_GAP);
				DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hot bed Heating..."));				
				dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
				DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("Testing Hotend / Hotbed"));
				DWIN_FEEDBACK_WARNNING();
				test_seconds = 0;
				testflag.state = CHECK_HEATER;
	 		}
		break;

	case CHECK_HEATER:		
		if(test_timer++ >= 100){
			test_timer = 0;			
			if(hotend_ok && hotbed_ok){
				test_seconds = 0;
				testflag.state = CHECK_FANS_PREPARE;
				break;
			}
			test_seconds++;
			if(test_seconds < 20){
				//If the hotend raised 10 degree within 20 seconds, say okay.
				if(!hotend_ok && thermalManager.degHotend(0) >= test_temp_hotend_first + 10.0) {					
					hotend_ok = true;
					DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Hot end test OK!  "));
				}
				//If the hotbed raised 2 degree within 20 seconds, say okay.
				if(!hotbed_ok && thermalManager.degBed() >= test_temp_hotbed_first + 2.0) {					
					hotbed_ok = true;
					DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hot bed test OK!  "));
					thermalManager.setTargetBed(0);
				}
			}
			else if(test_seconds < 30){
				//If the hotend raise 15 degree within 30 seconds, say check.
				if(!hotend_ok && thermalManager.degHotend(0) >= test_temp_hotend_first + 15.0){					
					hotend_ok = true;
					DRAW_STRING_FONT12(COLOR_YELLOW, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Heating too slow!!"));
				}
				//If the hotbed raised 3 degree within 30 seconds, say okay.
				if(!hotbed_ok && thermalManager.degBed() >= test_temp_hotbed_first + 3.0) {					
					hotbed_ok = true;					
					thermalManager.setTargetBed(0);
					DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hot bed test OK!  "));
				}
			}
			else if(test_seconds == 30){
				if(!hotend_ok){
					DWIN_FEEDBACK_WARNNING();
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Hot end test fail!"));
					thermalManager.setTargetHotend(0, 0);
					//if hotbed okay but hotend fail, bypass testing FANs.
					if(hotbed_ok){
						if(testflag.autorun) 
							testflag.state = CHECK_XY_MOTOR;
						else 
							testflag.state = CHECK_FINISHED;
					}
				}
			}
			else if(test_seconds > 30 && test_seconds <= 50){
				DWIN_FEEDBACK_TIPS();
				//Stop heating the hotend once it is over 80 degree
				if(thermalManager.degHotend(0) >= 80 && thermalManager.degTargetHotend(0) >= 30) thermalManager.setTargetHotend(0, 0);				
				if(!hotbed_ok){
					//If the hotbed raise 5 degree within 40 seconds, say okay, otherwise say check.
					if(thermalManager.degBed() >= test_temp_hotbed_first + 5.0){
						hotbed_ok = true;
						thermalManager.setTargetBed(0);
						if(test_seconds <= 40)
							DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hot bed test OK!  "));
						else
							DRAW_STRING_FONT12(COLOR_YELLOW, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Heating too slow!"));
					}								
				}
			}
			else if(test_seconds > 50){
					thermalManager.setTargetHotend(0, 0);
					thermalManager.setTargetBed(0);
					if(!hotbed_ok)
						DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hot bed test fail!"));
					if(hotend_ok)
						testflag.state = CHECK_FANS_PREPARE;
					else{
						if(testflag.autorun) 
							testflag.state = CHECK_XY_MOTOR;
						else 
							testflag.state = CHECK_FINISHED;
					}
				}
			}		
		break;

		case CHECK_FANS_PREPARE:				
				thermalManager.set_fan_speed(0, 255);
				DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_FAN), PSTR("Extruder Fan On!"));
				dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
				DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("Watch the FANs"));
				DWIN_FEEDBACK_WARNNING();
				test_seconds = 0;
				test_timer = 0;				
				testflag.fan_fg = false;
				test_counter = 0;
				testflag.state = CHECK_FANS;
			break;

		case CHECK_FANS:
			test_timer++;
			//check cooling fan
			thermalManager.checkExtruderAutoFans();
			if(test_counter == 0){
				if((thermalManager.degHotend(0) >= AUTO_FAN_TEMPERATURE + 5) || (test_timer > 2000)){
					test_timer = 0;
					test_counter++;					
					DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_FAN), PSTR("Cooling Fan On!"));
				}
			}
			//check extruder fan
			if(test_counter == 1 && test_timer > 500) {
					thermalManager.set_fan_speed(0, 0);
					DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_FAN), PSTR("Extruder Fan Off!"));
					thermalManager.setTargetHotend(0, 0);
					dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("Watch X&Y motors"));
					DWIN_FEEDBACK_WARNNING();
					test_timer = 0;
					test_counter = 0;
					if(testflag.autorun) 
						testflag.state = CHECK_XY_MOTOR;
					else 
						testflag.state = CHECK_FINISHED;
			}
			break;			
			
	#if ENABLED(COREXY)
		case CHECK_XY_MOTOR:
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_XYMOTOR), DWIN_WIDTH, YPOS(ID_LINE_XYMOTOR)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_XYMOTOR), PSTR("XY Motor On..."));
			if(test_timer++ >= 100){
				test_timer = 0;
				test_dir = !test_dir;				
			  if(test_dir)
					current_position.x += 10;
				else
					current_position.x -= 10;
				planner.buffer_line(current_position, MMM_TO_MMS(HOMING_FEEDRATE_XY), 0);
				planner.synchronize();
				if(test_counter++ >=5){
					test_counter = 0;
					dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_XYMOTOR), DWIN_WIDTH, YPOS(ID_LINE_XYMOTOR)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_XYMOTOR), PSTR("XY Motor Off"));					
					dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("Watch Z motors"));
					DWIN_FEEDBACK_WARNNING();
					testflag.state = CHECK_Z_MOTOR;
				}			
			}
			break;
	#else
		case CHECK_XY_MOTOR:
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_XYMOTOR), DWIN_WIDTH, YPOS(ID_LINE_XYMOTOR)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_XYMOTOR), PSTR("X Motor On..."));
			if(test_timer++ >= 100){
				test_timer = 0;
				test_dir = !test_dir;				
			  if(test_dir)
					current_position.x += 10;
				else
					current_position.x -= 10;
				planner.buffer_line(current_position, MMM_TO_MMS(HOMING_FEEDRATE_XY), 0);
				planner.synchronize();
				if(test_counter++ >=5){
					test_counter = 0;
					dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_XYMOTOR), DWIN_WIDTH, YPOS(ID_LINE_XYMOTOR)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_XYMOTOR), PSTR("X Motor Off"));					
					dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("Watch Y motors"));
					DWIN_FEEDBACK_WARNNING();
					testflag.state = CHECK_Y_MOTOR;
				}			
			}
			break;
			
		case CHECK_Y_MOTOR:
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_XYMOTOR), DWIN_WIDTH, YPOS(ID_LINE_XYMOTOR)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_XYMOTOR), PSTR("Y Motor On..."));
			if(test_timer++ >= 100){
				test_timer = 0;
				test_dir = !test_dir;				
			  if(test_dir)
					current_position.y += 10;
				else
					current_position.y -= 10;
				planner.buffer_line(current_position, MMM_TO_MMS(HOMING_FEEDRATE_XY), 0);
				planner.synchronize();
				if(test_counter++ >=5){
					test_counter = 0;
					dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_XYMOTOR), DWIN_WIDTH, YPOS(ID_LINE_XYMOTOR)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_XYMOTOR), PSTR("Y Motor Off"));					
					dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("Watch Z motors"));
					DWIN_FEEDBACK_WARNNING();
					testflag.state = CHECK_Z_MOTOR;
				}			
			}
			break;
	#endif
		
		case CHECK_Z_MOTOR:
			test_timer++;
			if(test_counter >= 6){
				dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_ZMOTOR), DWIN_WIDTH, YPOS(ID_LINE_ZMOTOR)+ROW_GAP);
				DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_ZMOTOR), PSTR("Z-R Motor On..."));
			}
			else{
				dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_ZMOTOR), DWIN_WIDTH, YPOS(ID_LINE_ZMOTOR)+ROW_GAP);
			#if (NUM_Z_STEPPER_DRIVERS == 2)
				DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_ZMOTOR), PSTR("Z-L Motor On..."));
			#else
				DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_ZMOTOR), PSTR("Z Axis Motor On"));
			#endif
			}

			if(test_timer >= 100){
				test_timer = 0;
				test_dir = !test_dir;
				test_counter++;
			#if (NUM_Z_STEPPER_DRIVERS == 2)
				if(test_counter >= 12)
			#else
				if(test_counter >= 6)
			#endif
				{
					test_counter = 0;					
					dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_ZMOTOR), DWIN_WIDTH, YPOS(ID_LINE_ZMOTOR)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_ZMOTOR), PSTR("Z Axis Motors Off"));
					dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("Watch extrude motors"));
					DWIN_FEEDBACK_WARNNING();
					testflag.state = CHECK_EXTRUDER_MOTOR;
					tool_change(0);
				}
			}
			break;

		case CHECK_EXTRUDER_MOTOR:
			test_timer++;
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_EXTRUDER), DWIN_WIDTH, YPOS(ID_LINE_EXTRUDER)+ROW_GAP);
		#if (E_STEPPERS > 1)
			DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("Test Extruder1 Motor"));
		#else
			DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("Test Extruder Motor"));	
		#endif
			if(test_timer >= 100){
				test_timer = 0;
				test_dir = !test_dir;
				if(test_dir) {					
					queue.inject_P(PSTR("G92 E0\nG1 E10 F3000"));
				}
				else{
					queue.inject_P(PSTR("G92 E0\nG1 E-10 F3000"));
				}

				if(test_counter++ >= 3){
					test_counter = 0;					
					testflag.state++;
					tool_change(1);
				}
			}
			break;

			
	#if (E_STEPPERS > 1)
		case CHECK_MOTOR_E2:
			test_timer++;
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_EXTRUDER), DWIN_WIDTH, YPOS(ID_LINE_EXTRUDER)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_YELLOW, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("Test Extruder2 Motor"));
			if(test_timer >= 100){
				test_timer = 0;
				test_dir = !test_dir;
				if(test_dir) {
					tool_change(1);
					queue.inject_P(PSTR("G92 E0\nG1 E10 F3000"));
				}
				else{
					queue.inject_P(PSTR("G92 E0\nG1 E-10 F3000"));
				}

				if(test_counter++ >= 3){
					test_counter = 0;					
					testflag.state++;
					tool_change(2);
				}
			}
			break;
	#endif

	#if (E_STEPPERS > 2)
		case CHECK_MOTOR_E3:
			test_timer++;
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_EXTRUDER), DWIN_WIDTH, YPOS(ID_LINE_EXTRUDER)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_BG_RED, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("Test Extruder3 Motor"));
			if(test_timer >= 100){
				test_timer = 0;
				test_dir = !test_dir;
				if(test_dir) {
					queue.inject_P(PSTR("G92 E0\nG1 E10 F3000"));
				}
				else{
					queue.inject_P(PSTR("G92 E0\nG1 E-10 F3000"));
				}

				if(test_counter++ >=3){
					test_counter = 0;
					testflag.state++;
					tool_change(3);
				}
			}
			break;
	#endif

	#if (E_STEPPERS > 3)
		case CHECK_MOTOR_E4:
			test_timer++;
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_EXTRUDER), DWIN_WIDTH, YPOS(ID_LINE_EXTRUDER)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("Test Extruder4 Motor"));
			if(test_timer >= 100){
				test_timer = 0;
				test_dir = !test_dir;
				if(test_dir) {
					queue.inject_P(PSTR("G92 E0\nG1 E10 F3000"));
				}
				else{
					queue.inject_P(PSTR("G92 E0\nG1 E-10 F3000"));
				}
				if(test_counter++ >=3){
					test_counter = 0;
					testflag.state++;
					tool_change(0);
				}				
			}
		break;
	#endif

		case CHECK_MOTOR_END:
			test_timer = 0;
			disable_all_steppers();
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_EXTRUDER), DWIN_WIDTH, YPOS(ID_LINE_EXTRUDER)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("Extrude Motors Off"));
			if(testflag.autorun) 
				testflag.state = CHECK_ENDSTOPS_PREPARE;
			else 
				testflag.state = CHECK_FINISHED;
			break;
	
		case CHECK_ENDSTOPS_PREPARE:
			AutoTest_ShowSWStatus(1);
			testflag.Endstops = 0;
		#if !PIN_EXISTS(Z2_MIN)	
			testflag.Endstops |= 1<<3;
		#endif
		#if !PIN_EXISTS(FIL_RUNOUT)
			testflag.Endstops |= 1<<4;
		#endif
		#if !PIN_EXISTS(Z_MIN_PROBE)
			testflag.Endstops |= 1<<5;
		#endif
			if(testflag.autorun) {
				dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 0, YPOS(ID_LINE_SW_RESULT), DWIN_WIDTH, YPOS(ID_LINE_SW_RESULT)+ROW_GAP);
				for(uint8_t i=0; i < 6; i++){ 					
					if((testflag.Endstops & (1<<i)) == 0)
						DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART+WIDTH/2+i*SWWIDTH, YPOS_MSG(ID_LINE_SW_RESULT), PSTR("--"));
					else
						DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_WINDOW, LSTART+WIDTH/2+i*SWWIDTH, YPOS_MSG(ID_LINE_SW_RESULT), PSTR("OK"));
				}
			}
			dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("To press the ENDSTOPs"));
			DWIN_FEEDBACK_WARNNING();
			
			test_timer = 0;
			testflag.state = CHECK_ENDSTOPS;	
		break;

		case CHECK_ENDSTOPS:
			test_timer++;
			AutoTest_Watch_SW();			
		  if(test_timer >= 12){
			 	test_timer = 0;								
				AutoTest_ShowSWStatus(0);
				if(testflag.autorun){
					for(uint8_t i=0; i<6; i++){
						if((testflag.Endstops & (1<<i)) == 0 && (old_swstatus[i] == 0x0 && swstatus[i] == 0x07)){
							testflag.Endstops |= (1<<i);
							dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, LSTART+WIDTH/2+i*SWWIDTH, YPOS(ID_LINE_SW_RESULT), LSTART+WIDTH/2+i*SWWIDTH+SWWIDTH, YPOS(ID_LINE_SW_RESULT)+ROW_GAP);
							DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_WINDOW, LSTART+WIDTH/2+i*SWWIDTH, YPOS_MSG(ID_LINE_SW_RESULT), PSTR("OK"));							
						}				
					}
				}
				for(uint8_t i=0; i<6; i++) old_swstatus[i] = swstatus[i];				
				if(testflag.autorun && testflag.Endstops == 0x3f){					
					dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("Rotate and click knob"));
					DWIN_FEEDBACK_WARNNING();					
					testflag.click_counter = 0;
					testflag.rotary_counter = 0;
					test_dir = 0;
					test_timer = 0;					
					testflag.state = CHECK_KNOB;					
				}				
		  }			
			break;

		case CHECK_FINISHED:				
			break;
			
		case CHECK_KNOB:	
			test_timer++;
			AutoTest_Watch_SW();
			if(test_timer >= 12) {
				test_timer = 0;
				AutoTest_ShowSWStatus(0);
				for(uint8_t i=0; i<6; i++) old_swstatus[i] = swstatus[i];
			}
			if((testflag.autorun) && (testflag.click_counter >= 5) && (testflag.rotary_counter > 10)) testflag.state  = CHECK_END;
			break;

		case CHECK_END:			
			testflag.autorun = false;
			testflag.state = CHECK_START;
			DWIN_status = ID_SM_RETURN_MAIN;			
	}		
}

inline void Autotest::Draw_Autotest_Menu(void){
	//clean screen	
	dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, 0, DWIN_WIDTH, DWIN_HEIGHT);
		
	//titel
	dwinLCD.Draw_Rectangle(1, COLOR_BG_BLUE, 0, 0, DWIN_WIDTH, ROW_GAP);
	DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_BLUE, XCENTER(-8), (F_GAP/2), PSTR("3D Printer Test"));

	//Temperature bar
	dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 0, YPOS(ID_LINE_ETEMP), DWIN_WIDTH, YPOS(ID_LINE_ETEMP)+ROW_GAP*4);
	DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_ETEMP), PSTR("Hot end:"));
	DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, XCENTER(3),  YPOS_MSG(ID_LINE_ETEMP), PSTR("/"));
	DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART,  YPOS_MSG(ID_LINE_BTEMP), PSTR("Hot bed:"));
	DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, XCENTER(3), YPOS_MSG(ID_LINE_BTEMP), PSTR("/"));

	//bottom bar
	dwinLCD.Draw_Rectangle(1, COLOR_BG_BLUE, 0, YPOS(ID_LINE_KNOB), DWIN_WIDTH, YPOS(ID_LINE_KNOB)+ROW_GAP);	
	if(testflag.autorun){	
		DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_BLUE, LSTART, YPOS_MSG(ID_LINE_KNOB), PSTR("Rotary:"));
		DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_BLUE, LSTART+XCENTER(0), YPOS_MSG(ID_LINE_KNOB), PSTR("Click:"));
	}
	else
		DRAW_STRING_FONT8(COLOR_RED, COLOR_BG_BLUE, LSTART+10, YPOS_MSG(ID_LINE_KNOB), PSTR("Click knob 3 times to exit!"));
}

void Autotest:: Autotest_Start(uint8_t check_state/* = CHECK_START*/, bool auto_loop /*= true*/) {	
	testflag.state = check_state;
	testflag.autorun = auto_loop;
	DWIN_status = ID_SM_AUTOTESTING;
	thermalManager.setTargetHotend(0, 0);
	thermalManager.setTargetBed(0);
	testflag.rotary_counter = 0;
	testflag.click_counter = 0;
	Draw_Autotest_Menu();
}
#endif
