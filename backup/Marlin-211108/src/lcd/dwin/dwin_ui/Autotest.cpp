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


static uint8_t swstatus[6]={0,0,0,0,0,0};
static uint8_t old_swstatus[6]={0,0,0,0,0,0};


#define	USED_FONT		font12x24
#define WIDTH  				12
#define HEIGH  				24
#define F_GAP	 				8
#define LSTART	 			5
#define SWWIDTH	 			45
#define TEST_EXTRUDER_AUTO_FAN_TEMPERATURE	40


#define ROW_GAP				(HEIGH+F_GAP)

#define	ID_LINE_TITLE						1
#define	ID_LINE_SD1							2
#define	ID_LINE_SD2							3
#define	ID_LINE_ETEMP						4
#define	ID_LINE_ETEMP_INFO			5
#define	ID_LINE_BTEMP						6
#define	ID_LINE_BTEMP_INFO			7
#define	ID_LINE_FAN							8
#define	ID_LINE_XYMOTOR					9
#define	ID_LINE_ZMOTOR					10
#define	ID_LINE_EXTRUDER				11
#define	ID_LINE_SW							12
#define	ID_LINE_SW_STATE				13
#define	ID_LINE_SW_RESULT				14
#define	ID_LINE_KNOB						15

#define	XSTART					0
#define	YPOS(L)					(ROW_GAP*(L-1))
#define	YPOS_MSG(L)			(ROW_GAP*(L-1)+F_GAP/2)
#define	XCENTER(L)			(DWIN_WIDTH/2+L*WIDTH)
#define	Y_BOTTOMBAR			(DWIN_HEIGHT-HEIGH)

#define	DRAW_INT_WHITE_FONT12(a,b,x,y,v) dwinLCD.Draw_IntValue(true, true, 0, USED_FONT, COLOR_WHITE, a, b, x, y, v)
#define	DRAW_INT_RED_FONT12(a,b,x,y,v) dwinLCD.Draw_IntValue(true, true, 0, USED_FONT, COLOR_RED, a, b, x, y, v)
#define	DRAW_INT_GREEN_FONT12(a,b,x,y,v) dwinLCD.Draw_IntValue(true, true, 0, USED_FONT, COLOR_GREEN, a, b, x, y, v)
#define	DRAW_STRING_FONT12(a,b,x,y,s) dwinLCD.Draw_String(false, true, USED_FONT, a, b, x, y, s)
#define	DRAW_STRING_FONT8(a,b,x,y,s) dwinLCD.Draw_String(false, true, font8x16, a, b, x, y, s)


void Autotest::Check_Rotary(){
	ENCODER_DiffState encoder_diffState = get_encoder_state();

	if (encoder_diffState == ENCODER_DIFF_NO) return;	
	if (encoder_diffState == ENCODER_DIFF_CW && testflag.rotary_counter_rg < 255) testflag.rotary_counter_rg++;
	else if(encoder_diffState == ENCODER_DIFF_CCW && testflag.rotary_counter_rg > 0) testflag.rotary_counter_rg--;
	else if(encoder_diffState == ENCODER_DIFF_ENTER) {
		buzzer.tone(200, 3000);
		testflag.rotary_click_rg++;		
	}	
	Autotest_ShowKnob(testflag.rotary_counter_rg);
}

inline void Autotest::Autotest_ShowKnob(uint8_t rotates){
	dwinLCD.Draw_Rectangle(1, COLOR_BG_BLUE, LSTART+XCENTER(7), YPOS(ID_LINE_KNOB), LSTART+XCENTER(10), YPOS(ID_LINE_KNOB)+ROW_GAP);	
	DRAW_STRING_FONT12((testflag.rotary_click_rg>4?COLOR_GREEN : COLOR_WHITE), COLOR_BG_BLUE, LSTART+XCENTER(7), YPOS_MSG(ID_LINE_KNOB), F_STRING_ONOFF(testflag.rotary_click_rg&0x01));	
	if(rotates > 10)
		DRAW_INT_GREEN_FONT12(COLOR_BG_BLUE, 3, LSTART+7*WIDTH, YPOS_MSG(ID_LINE_KNOB), rotates);
	else
		DRAW_INT_WHITE_FONT12(COLOR_BG_BLUE, 3, LSTART+7*WIDTH, YPOS_MSG(ID_LINE_KNOB), rotates);
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
	if((thermalManager.degHotend(0) <= TEST_EXTRUDER_AUTO_FAN_TEMPERATURE) && (testflag.loops > CHECK_FAN_SPEED)){
		thermalManager.fan_speed[0] = 0;
		if(!testflag.fan_fg){
			testflag.fan_fg = 1;
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
		DRAW_STRING_FONT12((swstatus[0]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[0]==0x07));
	//Y
	if(old_swstatus[1] != swstatus[1] || bfirst)
		DRAW_STRING_FONT12((swstatus[1]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART+1*SWWIDTH, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[1]==0x07));
	//Z
	if(old_swstatus[2] != swstatus[2] || bfirst)
		DRAW_STRING_FONT12((swstatus[2]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART+2*SWWIDTH, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[2]==0x07));
	//Z2
#if PIN_EXISTS(Z2_MIN)
	if(old_swstatus[3] != swstatus[3] || bfirst)
		DRAW_STRING_FONT12((swstatus[3]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART+3*SWWIDTH, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[3]==0x07));
#endif
	//runout sensor
#if PIN_EXISTS(FIL_RUNOUT)
	if(old_swstatus[4] != swstatus[4] || bfirst)
		DRAW_STRING_FONT12((swstatus[4]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART+4*SWWIDTH, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[4]==0x07));		
#endif
	//Probe
#if PIN_EXISTS(Z_MIN_PROBE)
	if(old_swstatus[5] != swstatus[5] || bfirst)
		DRAW_STRING_FONT12((swstatus[5]==0x07)?COLOR_GREEN:COLOR_WHITE, COLOR_BG_WINDOW, LSTART+5*SWWIDTH, YPOS_MSG(ID_LINE_SW_STATE), F_STRING_ONOFF(swstatus[5]==0x07));
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
		
	#if PIN_EXISTS(Z2_MIN)
		swstatus[3] <<= 1;
		if(READ(Z2_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING) swstatus[3] |= 0x01; else swstatus[3] &= 0xfe;	
	#endif

	#if PIN_EXISTS(FIL_RUNOUT)
		swstatus[4] <<= 1;
		if(READ(FIL_RUNOUT_PIN) == FIL_RUNOUT_STATE) swstatus[4] |= 0x01; else swstatus[4] &= 0xfe;	
	#endif

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

bool Autotest::DWIN_AutoTesting() {
	static millis_t test_next_rts_update_ms = 0;
	static uint16_t test_counter = 0;
	static uint16_t test_timer = 0;
	static uint16_t temp_test_timer = 0;
	static bool test_dir = 0;
	static float test_temp_hotend_first = -100.0;
	static float test_temp_hotbed_first = -100.0;
	
	if(testflag.loops == CHECK_Z_MOTOR){
		stepper.do_Zaxis_step(test_counter >= 6,test_dir);
		DELAY_US(150);
	}
	
	Check_Rotary();
	const millis_t test_ms = millis();
	if (PENDING(test_ms, test_next_rts_update_ms)) return false;
	test_next_rts_update_ms = test_ms + 10;
	
	if(testflag.loops >= CHECK_SD) AutoTest_ShowTemperature();
	
	switch(testflag.loops){
		default:
		case CHECK_START:
			testflag.loops = CHECK_SD;
			break;
			
		case CHECK_SD:		
			if(IS_SD_INSERTED()){
				dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD2)+ROW_GAP);
				DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("SD Card OK!"));
			  DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD2), PSTR("SD Size(M):"));
				DRAW_INT_RED_FONT12(COLOR_BG_BLACK, 5, (strlen("SD Size(M):")+1)*WIDTH, YPOS_MSG(ID_LINE_SD2), CardReader::sd2card.cardSize()/2000);				
		 		test_timer = 0;
				testflag.loops++;
			}
			else{
				if(test_timer++ >= 50) {
					test_timer = 0;
					test_dir = !test_dir;
					if(test_dir){
						dwinLCD.Draw_Rectangle(1, COLOR_BG_RED, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD2)+ROW_GAP);
						DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_RED, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("Please insert SD Card!"));
						DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_RED, LSTART, YPOS_MSG(ID_LINE_SD2), PSTR("Or SD Card error!"));
					}
					else{
						dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD2)+ROW_GAP);
						DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("Please insert SD Card!"));
						DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD2), PSTR("Or SD Card error!"));
					}
				}
			}
		break;

	 case CHECK_PREPARE_HEAT:
	 		if(test_timer++ >= 100){
				test_timer = 0;
				//check current temp of hotend
				test_temp_hotend_first = thermalManager.degHotend(0);
				if(test_temp_hotend_first <= 1){					
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Hotend testing fail!"));
					buzzer.tone(20, 4000);
					break;
				}
				else if(test_temp_hotend_first >= 150){					
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Hotend too hot!!"));
					buzzer.tone(20, 4000);
					break;
				}
				//check current temp of hotbed
				test_temp_hotbed_first = thermalManager.degBed();
				if(test_temp_hotbed_first <= 1){					
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hotbed testing fail!"));
					buzzer.tone(20, 4000);
					break;
				}
				else if(test_temp_hotbed_first >= 60){					
					DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hotbed too hot!!"));
					buzzer.tone(20, 4000);
					break;
				}
				thermalManager.setTargetHotend(max(test_temp_hotend_first + 20, 60), 0);
				dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 0, YPOS(ID_LINE_ETEMP_INFO), DWIN_WIDTH, YPOS(ID_LINE_ETEMP_INFO)+ROW_GAP);
				DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Hotend Heating..."));
				thermalManager.setTargetBed(max(test_temp_hotbed_first + 20, 60));
				dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 0, YPOS(ID_LINE_BTEMP_INFO), DWIN_WIDTH, YPOS(ID_LINE_BTEMP_INFO)+ROW_GAP);
				DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hotbed Heating..."));				
				testflag.loops++;
	 		}
		break;

	case CHECK_PREPARE_HEAT2:
		if(test_timer++ >= 100){
			test_timer = 0;
			if(thermalManager.degHotend(0) >= test_temp_hotend_first + 2.0 && thermalManager.degBed() >= test_temp_hotbed_first + 1.0){
				test_temp_hotend_first = thermalManager.degHotend(0);
				test_temp_hotbed_first = thermalManager.degBed();
				testflag.loops++;
			} 
			else{
				temp_test_timer++;
				if(temp_test_timer > 20){
					temp_test_timer = 0;
					if(thermalManager.degHotend(0) <= test_temp_hotend_first + 2.0)
						DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Hotend testing fail!"));
					if(thermalManager.degBed() <= test_temp_hotbed_first + 2.0)
						DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hotbed testing fail!"));
					thermalManager.setTargetBed(0);
					testflag.loops = CHECK_FAN_SPEED;
				}				
			}
		}		
		break;
		
	 case CHECK_HOTEND_TEMP:
			//raised 2 degree within 5 seconds
			test_timer++;
			if(test_timer%100 == 0) buzzer.tone(10, 1000); 
			if(thermalManager.degHotend(0) >= test_temp_hotend_first + 2.0){
				DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Hotend testing OK!"));
			}
			else if(test_timer > 500){ 
				DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_ETEMP_INFO), PSTR("Please check Hotend!"));
			}
			else break;
			
			test_timer = 0;
			testflag.loops++;			
			break;

		case CHECK_HOTBED_TEMP:
			//raised 2 degree within 10 seconds
			test_timer++;
			if(test_timer%100 == 0) buzzer.tone(10, 1000); 				
			if(thermalManager.degBed() >= test_temp_hotbed_first + 1.0){
				DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Hotbed testing OK!"));
			}
			else if(test_timer > 500){
				DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_WINDOW, LSTART, YPOS_MSG(ID_LINE_BTEMP_INFO), PSTR("Please check Hotbed!"));
			}
			else break;
			
			thermalManager.setTargetBed(0);			
			thermalManager.set_fan_speed(0, 255);			
			DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_FAN), PSTR("Fans On, check them"));

			test_timer = 0;
			testflag.loops++;
			testflag.fan_fg = 0;
			break;

		case CHECK_FAN_SPEED:
			test_timer++;
			thermalManager.checkExtruderAutoFans();
			if((thermalManager.degHotend(0) >= TEST_EXTRUDER_AUTO_FAN_TEMPERATURE + 10) || (test_timer > 2000)){
				test_timer = 0;
				test_counter = 0;
				thermalManager.setTargetHotend(0, 0);
				testflag.loops++;
			}
			break;

		case CHECK_XY_MOTOR:
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_XYMOTOR), DWIN_WIDTH, YPOS(ID_LINE_XYMOTOR)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_XYMOTOR), PSTR("XY Axis Motor On..."));
			if(test_timer++ >= 100){
				test_timer = 0;
				test_dir = !test_dir;
			  if(test_dir) {
					current_position.x += 10;
					#if DISABLED(COREXY)
					current_position.y += 10;
					#endif
			  }
				else {
					current_position.x -= 10;
					#if DISABLED(COREXY)
					current_position.y -= 10;
					#endif					
				}
				planner.buffer_line(current_position, MMM_TO_MMS(HOMING_FEEDRATE_XY), 0);
				planner.synchronize();
				if(test_counter++ >=5){
					test_counter = 0;
					dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_XYMOTOR), DWIN_WIDTH, YPOS(ID_LINE_XYMOTOR)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_XYMOTOR), PSTR("XY Axis Motor Off"));
					testflag.loops++;
				}			
			}
			break;

		
		case CHECK_Z_MOTOR:
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_ZMOTOR), DWIN_WIDTH, YPOS(ID_LINE_ZMOTOR)+ROW_GAP);
			if(test_counter >= 6)
				DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_ZMOTOR), PSTR("ZR Axis Motor On..."));
			else
				DRAW_STRING_FONT12(COLOR_RED, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_ZMOTOR), PSTR("ZL Axis Motor On..."));

			if(test_timer++ >= 100){
				test_timer = 0;
				test_dir = !test_dir;					
			#if 0	
			  if(test_dir)
					current_position.z += 3;
				else 
					current_position.z -= 2;
				planner.buffer_line(current_position, MMM_TO_MMS(HOMING_FEEDRATE_Z), active_extruder);
				planner.synchronize();
			#endif
			#if (NUM_Z_STEPPER_DRIVERS >= 2)
				if(test_counter++ >= 11)
			#else
				if(test_counter++ >= 5)
			#endif
				{
					test_counter = 0;					
					dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_ZMOTOR), DWIN_WIDTH, YPOS(ID_LINE_ZMOTOR)+ROW_GAP);
					DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_ZMOTOR), PSTR("Z Axis Motor Off"));
					testflag.loops++;
				}
			}
			break;

		case CHECK_MOTOR_E1:
			 dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_EXTRUDER), DWIN_WIDTH, YPOS(ID_LINE_EXTRUDER)+ROW_GAP);
			 DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("Extruder1 Motor On..."));
			 if(test_timer++ >= 100){
			 	test_timer = 0;
				test_dir = !test_dir;
			  if(test_dir) {
			 		queue.inject_P(PSTR("T0\nG92 E0\nG1 E10 F3000"));
			  }
				else{
					queue.inject_P(PSTR("T0\nG92 E0\nG1 E-10 F3000"));
				}

				if(test_counter++ >=3){
					test_counter = 0;					
					testflag.loops++;
				}
			 }
			break;

			
#if (E_STEPPERS > 1)
		case CHECK_MOTOR_E2:
			 dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_EXTRUDER), DWIN_WIDTH, YPOS(ID_LINE_EXTRUDER)+ROW_GAP);
			 DRAW_STRING_FONT12(COLOR_YELLOW, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("Extruder2 Motor On..."));
			 if(test_timer++ >= 100){
			 	test_timer = 0;
				test_dir = !test_dir;
			  if(test_dir) {
			 		queue.inject_P(PSTR("T1\nG92 E0\nG1 E10 F3000"));
			  }
				else{
					queue.inject_P(PSTR("T1\nG92 E0\nG1 E-10 F3000"));
				}

				if(test_counter++ >=3){
					test_counter = 0;					
					testflag.loops++;
				}
			 }
			break;
#endif

#if (E_STEPPERS > 2)
		case CHECK_MOTOR_E3:
			 dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_EXTRUDER), DWIN_WIDTH, YPOS(ID_LINE_EXTRUDER)+ROW_GAP);
			 DRAW_STRING_FONT12(COLOR_BG_RED, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("Extruder3 Motor On..."));
			 if(test_timer++ >= 100){
			 	test_timer = 0;
				test_dir = !test_dir;
			  if(test_dir) {
			 		queue.inject_P(PSTR("T2\nG92 E0\nG1 E10 F3000"));
			  }
				else{
					queue.inject_P(PSTR("T2\nG92 E0\nG1 E-10 F3000"));
				}

				if(test_counter++ >=3){
					test_counter = 0;
					testflag.loops++;
				}
			 }
			break;
#endif

#if (E_STEPPERS > 3)
		case CHECK_MOTOR_E4:
			dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_EXTRUDER), DWIN_WIDTH, YPOS(ID_LINE_EXTRUDER)+ROW_GAP);
			DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("Extruder4 Motor On..."));
			if(test_timer++ >= 100){
				test_timer = 0;
				test_dir = !test_dir;
				if(test_dir) {
					queue.inject_P(PSTR("T3\nG92 E0\nG1 E10 F3000"));
				}
				else{
					queue.inject_P(PSTR("T3\nG92 E0\nG1 E-10 F3000"));
				}
				if(test_counter++ >=3){
					test_counter = 0;
					testflag.loops++;
				}				
			}
		break;
#endif
		case CHECK_MOTORS_END:
				disable_all_steppers();
				dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, 0, YPOS(ID_LINE_EXTRUDER), DWIN_WIDTH, YPOS(ID_LINE_EXTRUDER)+ROW_GAP);
				DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_DEEPBLUE, LSTART, YPOS_MSG(ID_LINE_EXTRUDER), PSTR("ALL Extruder Motor Off"));					
		
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
				dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 0, YPOS(ID_LINE_SW_RESULT), DWIN_WIDTH, YPOS(ID_LINE_SW_RESULT)+ROW_GAP);
				for(uint8_t i=0; i<6; i++){ 					
					if((testflag.Endstops & (1<<i)) == 0)
						DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_WINDOW, LSTART+WIDTH/2+i*SWWIDTH, YPOS_MSG(ID_LINE_SW_RESULT), PSTR("--"));
					else
						DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_WINDOW, LSTART+WIDTH/2+i*SWWIDTH, YPOS_MSG(ID_LINE_SW_RESULT), PSTR("OK"));
				}
				test_timer = 0;
				testflag.loops++;			
		break;

		case CHECK_ENDSTOPS:
			AutoTest_Watch_SW();			
		  if(++test_timer >= 12){
			 	test_timer = 0;								
				AutoTest_ShowSWStatus(0);
				for(uint8_t i=0; i<6; i++){
					if((testflag.Endstops & (1<<i)) == 0 && (old_swstatus[i] == 0x0 && swstatus[i] == 0x07)){
						testflag.Endstops |= (1<<i);
						dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, LSTART+WIDTH/2+i*SWWIDTH, YPOS(ID_LINE_SW_RESULT), LSTART+WIDTH/2+i*SWWIDTH+SWWIDTH, YPOS(ID_LINE_SW_RESULT)+ROW_GAP);
						DRAW_STRING_FONT12(COLOR_GREEN, COLOR_BG_WINDOW, LSTART+WIDTH/2+i*SWWIDTH, YPOS_MSG(ID_LINE_SW_RESULT), PSTR("OK"));							
					}				
				}				
				if(testflag.Endstops == 0x3f){
					testflag.loops++;
					testflag.rotary_click_rg = 0;
					testflag.rotary_counter_rg = 0;
					test_dir = 0;
					test_timer = 0;
				}
				for(uint8_t i=0; i<6; i++) old_swstatus[i] = swstatus[i];
		  }			
			break;

		case CHECK_KEY:	
			AutoTest_Watch_SW();
			if(test_timer++ >= 12){
				test_timer = 0;
				AutoTest_ShowSWStatus(0);
			}
			if((testflag.rotary_click_rg >= 5) && (testflag.rotary_counter_rg > 10)){
				testflag.loops++;					
			}
			break;

		case CHECK_END:		
			testflag.loops = 0;
			return true;
	}		
	return false;
}

void Autotest::HMI_StartTest() {	
	testflag.loops = 0;

	thermalManager.setTargetHotend(0, 0);
	thermalManager.setTargetBed(0);
	
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
	DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_BLUE, LSTART, YPOS(ID_LINE_KNOB), PSTR("Rotary:"));
	DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_BLUE, LSTART+XCENTER(0), YPOS(ID_LINE_KNOB), PSTR("Buzzer:"));
}
#endif
