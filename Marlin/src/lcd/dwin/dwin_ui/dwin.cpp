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
 * DWIN.cpp
 */

#include "../../../inc/MarlinConfig.h"

#if HAS_DWIN_LCD
#include <WString.h>
#include <stdio.h>
#include <string.h>

#include "../language/dwin_multi_language.h"
#include "../dwin_lcd.h"
#include "../rotary_encoder.h"
#include "dwin_comm.h"
#include "autotest.h"
#include "dwinmenu_main.h"
#include "dwinmenu_print.h"
#include "dwinmenu_prepare.h"
#include "dwinmenu_control.h"
#include "dwinmenu_infor.h"

#if ENABLED(MIXING_EXTRUDER)
  #include "../../../feature/mixing.h"
#endif

#if ENABLED(OPTION_REPEAT_PRINTING)
  #include "../../../feature/repeat_printing.h"
	#include "dwinmenu_repeatprint.h"
#endif

#include "../../fontutils.h"
#include "../../ultralcd.h"

#include "../../../sd/cardreader.h"

#include "../../../MarlinCore.h"
#include "../../../core/serial.h"
#include "../../../core/macros.h"
#include "../../../gcode/queue.h"

#ifdef DEBUG_DWIN_LCD
static uint8_t old_DwinMenuID = 255;
static uint8_t old_DwinStatus = 255;
#endif

_emDWINState DWIN_status = ID_SM_START;
_emDWIN_MENUID_ DwinMenuID = DWMENU_MAIN;
HMI_value_t HMI_Value;
HMI_Flag_t HMI_flag;
MIXER_DIS MixerDis;

uint8_t DwinMenu::now;
uint8_t DwinMenu::last;
uint8_t DwinMenu::index;


DwinMenu DwinMenu_main;
//Prepare
DwinMenu DwinMenu_prepare;
DwinMenu DwinMenu_home;
DwinMenu DwinMenu_move;
DwinMenu DwinMenu_temp;
DwinMenu DwinMenu_filament;
DwinMenu DwinMenu_leveling;
DwinMenu DwinMenu_powerdown;
DwinMenu DwinMenu_language;
//control
DwinMenu DwinMenu_control;
DwinMenu DwinMenu_motion;
DwinMenu DwinMenu_PreheatPLA;
DwinMenu DwinMenu_PreheatABS;
DwinMenu DwinMenu_configure;
DwinMenu DwinMenu_feedrate;
DwinMenu DwinMenu_accel;
DwinMenu DwinMenu_jerk;
DwinMenu DwinMenu_step;
#if ENABLED(MIXING_EXTRUDER)
DwinMenu DwinMenu_mixer;
DwinMenu DwinMenu_manualmix;
DwinMenu DwinMenu_GradientMix;
DwinMenu DwinMenu_RandomMix;
#endif
#if ENABLED(BLTOUCH)
DwinMenu DwinMenu_bltouch;
#endif
#if ENABLED(FWRETRACT)
DwinMenu DwinMenu_fwretract;
#endif
#if ENABLED(PID_EDIT_MENU)
DwinMenu DwinMenu_PIDTune;
#endif

#if ENABLED(OPTION_REPEAT_PRINTING)
DwinMenu DwinMenu_reprint;
#endif
//Print
DwinMenu DwinMenu_print;
DwinMenu DwinMenu_file;
DwinMenu DwinMenu_PauseOption;
DwinMenu DwinMenu_tune;
//Infor
DwinMenu DwinMenu_infor;

void set_status_msg_showtime(const uint16_t t){
	HMI_flag.clean_status_delay = t;
}

inline void _check_clean_status_bar(){
	if(HMI_flag.clean_status_delay > 0){
		HMI_flag.clean_status_delay--;
		if(HMI_flag.clean_status_delay == 0){ 
			Clear_Dwin_Area(AREA_BOTTOM);
			TERN_(MIXING_EXTRUDER, HMI_Value.old_mix_mode = -1);
		}
	}
}

inline void _check_kill_times_ElapsedTime(){
	if(HMI_flag.killElapsedTime > 0 && HMI_flag.killtimes > 0){
		HMI_flag.killElapsedTime--;
		if(HMI_flag.killElapsedTime == 0){ 
			HMI_flag.killtimes = 0;
			if(HMI_flag.clean_status_delay == 0) Clear_Dwin_Area(AREA_BOTTOM);
		}
	}
}


inline void _Update_printing_Timer(){
	// print process
	if(card.isPrinting()){
		// Print time so far
		duration_t elapsed = print_job_timer.duration(); // print timer
		static uint16_t last_Printtime = 0;
		const uint16_t seconds = elapsed.value % 3600;
		if (last_Printtime != seconds) { // 1 second update
			last_Printtime = seconds;
			if(DwinMenuID == DWMENU_PRINTING) Draw_Print_ElapsedTime();

		#if HAS_PRINT_PROGRESS_PERMYRIAD
			// Update remaining time
			const uint16_t card_permyriad = card.permyriadDone();
			static uint16_t last_cardpermyriad = 20000;
			if(last_cardpermyriad != card_permyriad){
				last_cardpermyriad = card_permyriad;
				HMI_Value.remain_time = ((elapsed.value > HMI_Value.dwin_heat_time?(elapsed.value-HMI_Value.dwin_heat_time) : elapsed.value) * 10000)/HMI_Value.card_permyriad - elapsed.value;
			}
		#endif
		}
		
		//Print Process Percent
		const uint8_t card_pct = card.percentDone();
		static uint8_t last_cardpercentValue = 255;
	
		if(last_cardpercentValue != card_pct) { // print percent
			last_cardpercentValue = card_pct;
			if(card_pct) {
				HMI_Value.Percentrecord = card_pct;
				if(DwinMenuID == DWMENU_PRINTING) Draw_Print_ProgressBar();
				
			// Update remaining time	
			#if (!HAS_PRINT_PROGRESS_PERMYRIAD)
				HMI_Value.remain_time = ((elapsed.value > HMI_Value.dwin_heat_time?elapsed.value-HMI_Value.dwin_heat_time:elapsed.value) * 100)/HMI_Value.Percentrecord - elapsed.value;
				if(DwinMenuID == DWMENU_PRINTING) Draw_Print_RemainTime();
			#endif
			}
		}		
	}
}


#if ENABLED(OPTION_AUTOPOWEROFF)
inline void _setAutoPowerDown(){
	HMI_flag.free_close_timer_rg = POWERDOWN_MACHINE_TIMER;
}
#endif


#if ENABLED(OPTION_WIFI_MODULE)
inline void _check_wifi_feedback(){	
	if(WiFi_Enabled && HMI_flag.wifi_link_timer > 0){
		HMI_flag.wifi_link_timer--;
		if(queue.wifi_Handshake_ok){
			DWIN_FEEDBACK_CONFIRM();
			HMI_flag.wifi_link_timer = 0;			
			if(DwinMenuID == DWMENU_MAIN)	Draw_ICON_WIFI();
		}
		else if(HMI_flag.wifi_link_timer == (WIFI_LINK_CHECK_TIME/3)) {
			DWIN_FEEDBACK_TIPS();
			DWIN_Show_Status_Message(COLOR_RED, PSTR("WiFi is optional, did it install?"), 0);
		}
		else if(HMI_flag.wifi_link_timer == 0){
			WiFi_Enabled = false;
			settings.save();
			if(DwinMenuID == DWMENU_CONFIG) Draw_Config_Menu(CONFIG_CASE_WIFI);
			DWIN_FEEDBACK_WARNNING();
			DWIN_Show_Status_Message(COLOR_RED, PSTR("WiFi connect fail!"), 5);
		}		
	}
}
#endif

#if(ENABLED(POWER_LOSS_RECOVERY))
inline void _check_Powerloss_resume(){
	static bool recovery_flag = false;

	auto update_selection = [&](const bool sel) {
		HMI_flag.select_flag = sel;
		const uint16_t c1 = sel ? COLOR_BG_WINDOW : COLOR_RED;
		dwinLCD.Draw_Rectangle(0, c1, 25, 306, 126, 345);
		dwinLCD.Draw_Rectangle(0, c1, 24, 305, 127, 346);
		const uint16_t c2 = sel ? COLOR_RED : COLOR_BG_WINDOW;
		dwinLCD.Draw_Rectangle(0, c2, 145, 306, 246, 345);
		dwinLCD.Draw_Rectangle(0, c2, 144, 305, 247, 346);
	};
	
	if(HMI_flag.lcd_sd_status && recovery.dwin_flag && recovery.enabled){
		recovery.dwin_flag = false;
		recovery_flag = true;
		Popup_Window_Resume();
		update_selection(true);
		/// TODO: Get the name of the current file from someplace
		//
		//(void)recovery.interrupted_file_exists();
		char * const name = card.longest_filename();
		const int8_t npos = _MAX(0U, DWIN_WIDTH - strlen(name) * 10) / 2;
		DWIN_Draw_MaskString_Default_PopMenu( npos, 252, name);
		dwinLCD.UpdateLCD();

		while (recovery_flag) {
			ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
			if (encoder_diffState != ENCODER_DIFF_NO) {
				if (encoder_diffState == ENCODER_DIFF_ENTER) {
					//cancel resume!
					recovery_flag = false;
					if(HMI_flag.select_flag) break;
					queue.inject_P(PSTR("M1000C"));
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Delete Power loss Record!"));
					DWIN_status = ID_SM_RETURN_MAIN;
					return;
				}
				else {
					DWIN_FEEDBACK_TICK();
					update_selection(encoder_diffState == ENCODER_DIFF_CW);					
				#if (DWINLCD_MENU_VERSION >= 2)
					if(HMI_flag.select_flag){
						DWIN_Show_ICON(ICON_CANCEL_E,  26, 307);
						DWIN_Show_ICON(ICON_CONTINUE_C, 146, 307);
					}
					else{
						DWIN_Show_ICON(ICON_CANCEL_C,  26, 307);
						DWIN_Show_ICON(ICON_CONTINUE_E, 146, 307);
					}
				#endif
				}
				dwinLCD.UpdateLCD();
			}
			TERN_(USE_WATCHDOG, HAL_watchdog_refresh());
		}
		HMI_flag.show_mode = SHOWED_TUNE;
		DWIN_status = ID_SM_PRINTING;
		queue.inject_P(PSTR("M1000"));
		DwinMenu_print.reset();
		Draw_Printing_Menu(PRINT_CASE_TUNE, true);
	}
}
#endif//(ENABLED(POWER_LOSS_RECOVERY))

void DWIN_Show_M117(const char * const message){
	#define MAX_WIFI_MESSAGE_LENGTH 40
	#define START_OF_UTF8_CHAR(C) (((C) & 0xC0u) != 0x80U)
	
	char status_message[MAX_WIFI_MESSAGE_LENGTH+1] = {0};
	// Here we have a problem. The message is encoded in UTF8, so
	// arbitrarily cutting it will be a problem. We MUST be sure
	// that there is no cutting in the middle of a multibyte character!
	// Get a pointer to the null terminator
	const char* pend = message + strlen(message);
	// If length of supplied UTF8 string is greater than
	// our buffer size, start cutting whole UTF8 chars
	while ((pend - message) > MAX_WIFI_MESSAGE_LENGTH) {
	 --pend;
	 while (!START_OF_UTF8_CHAR(*pend)) --pend;
	};	
	uint8_t maxLen = pend - message;
	strncpy(status_message, message, maxLen);
	status_message[maxLen] = '\0';	
#if ENABLED(OPTION_WIFI_MODULE)
	if(WiFi_Enabled && queue.wifi_M117_message){
		queue.wifi_M117_message = false;
		queue.wifi_Handshake_ok = true;
		HMI_flag.wifi_link_timer = 0;		
		if(DwinMenuID == DWMENU_MAIN)	Draw_ICON_WIFI();
		
	#if ENABLED(OPTION_WIFI_QRCODE)
		bool bgotIPAddress = false;
		uint8_t IPNumber[4] = {0,0,0,0};
		char temp_str[5] = {0};
		uint8_t i, j=0, k=0;
		for(i=0; i <= maxLen; i++) {
			if(status_message[i] == ' ') continue;
			temp_str[j] = status_message[i];
			if(temp_str[j] == '.' || temp_str[j] == 0 || temp_str[j] == '\r' || temp_str[j] == '\n') {
				temp_str[j+1] = '\0';
				IPNumber[k] = (uint8_t)atoi(temp_str);
				if(IPNumber[k] == 255 || (k == 0 && (IPNumber[k] < 192 || IPNumber[k] > 223))) break;	
				if(k >= 3) { bgotIPAddress = true; break;	}
				k++;
				j=0;
			}
			else{
				j++;
				if(j > 3) break;
			}
		}
		char IPAddress_message[50] = {0};
		if(bgotIPAddress)	{
			if(DwinMenuID == DWMENU_CONTROL || DwinMenuID == DWMENU_CONFIG || DwinMenuID == DWMENU_POP_WIFILINK){
				sprintf_P(status_message, PSTR("%d.%d.%d.%d"),IPNumber[0], IPNumber[1], IPNumber[2], IPNumber[3]);
				Popup_Window_WiFiLink(status_message);
			}
			sprintf_P(IPAddress_message, PSTR("Device IP address = %d.%d.%d.%d"),IPNumber[0], IPNumber[1], IPNumber[2], IPNumber[3]);			
			DWIN_Show_Status_Message(COLOR_WHITE,IPAddress_message, 0);
			return;
		}
	#endif
	}
#endif
	DWIN_Show_Status_Message(COLOR_WHITE,status_message, 15);	
}

#if ENABLED(MIXING_EXTRUDER)
inline void DWIN_Show_Extruder_status() {
	static int8_t last_vtool = -1;
	static uint8_t last_mixer_percent[MIXING_STEPPERS] = {0};  
	static bool bupdata = false;
	
	if(DwinMenuID != DWMENU_PRINTING) return;	
	
#if ENABLED(OPTION_MIXING_SWITCH)
	if(!mixer.mixing_enabled){
		//vool changed?
		if(last_vtool != mixer.selected_vtool){
			if(mixer.selected_vtool > E_STEPPERS){
				mixer.selected_vtool = 0;
				mixer.T(0);				
			}
			last_vtool = mixer.selected_vtool;
		}
		DWIN_Refresh_ExtruerFAN_State();
	}
	else	
#endif
	{		
		//vool changed?
		if(last_vtool != mixer.selected_vtool){
			last_vtool = mixer.selected_vtool;
			mixer.update_mix_from_vtool();
			bupdata = true;
		}
		//mixing rate changed?
		if(!bupdata){
			MIXER_STEPPER_LOOP(i){
				if(last_mixer_percent[i] != mixer.percentmix[i]){
					bupdata = true;
					break;
				}
			}
		}
		if(bupdata){
			MIXER_STEPPER_LOOP(i) last_mixer_percent[i] = mixer.percentmix[i];
			DWIN_Refresh_ExtruerFAN_State();
			Draw_Print_ProgressMixModel();		
			bupdata = false;
		}
	}	
}
#endif

inline void DWIN_Update_Variable() {
 #if HAS_HOTEND
  static float last_temp_hotend_target = 0, last_temp_hotend_current = 0;
 #endif
 #if HAS_HEATED_BED
  static float last_temp_bed_target = 0, last_temp_bed_current = 0;
 #endif
 #if HAS_FAN
  static uint8_t last_fan_speed = 0;
 #endif

 static uint8_t flash_mask = 0;

 /* Tune page temperature update */
	if (DwinMenuID == DWMENU_TUNE){
#if HAS_HOTEND
		if(last_temp_hotend_target != thermalManager.degTargetHotend(0))
		  DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(TUNE_CASE_ETEMP + MROWS - DwinMenu_tune.index), thermalManager.degTargetHotend(0));
#endif
#if HAS_HEATED_BED
		if(last_temp_bed_target != thermalManager.degTargetBed())
		  DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(TUNE_CASE_BTEMP + MROWS - DwinMenu_tune.index), thermalManager.degTargetBed());
#endif
#if HAS_FAN
		if(last_fan_speed != thermalManager.fan_speed[0]) {
		  DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(TUNE_CASE_FAN + MROWS - DwinMenu_tune.index), thermalManager.fan_speed[0]);
		  last_fan_speed = thermalManager.fan_speed[0];
		}
#endif
	}

 /* Temperature page temperature update */
	if (DwinMenuID == DWMENU_TEMPERATURE) {
#if HAS_HOTEND
		if (last_temp_hotend_target != thermalManager.degTargetHotend(0))
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(TEMP_CASE_ETEMP + MROWS - DwinMenu_temp.index), thermalManager.degTargetHotend(0));
#endif
#if HAS_HEATED_BED
		if (last_temp_bed_target != thermalManager.degTargetBed())
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(TEMP_CASE_BTEMP + MROWS - DwinMenu_temp.index), thermalManager.degTargetBed());
#endif
#if HAS_FAN
		if (last_fan_speed != thermalManager.fan_speed[0]) {
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(TEMP_CASE_FAN + MROWS - DwinMenu_temp.index), thermalManager.fan_speed[0]);
			last_fan_speed = thermalManager.fan_speed[0];
		}
#endif
	}

 /* Bottom temperature update */
#if HAS_HOTEND
	if (last_temp_hotend_current != thermalManager.degHotend(0)) {
		DWIN_Draw_IntValue_FONT10((thermalManager.degHotend(0) > HOTEND_WARNNING_TEMP)? COLOR_RED : COLOR_WHITE, State_text_extruder_num, State_text_extruder_X, State_text_extruder_Y, thermalManager.degHotend(0));
		last_temp_hotend_current = thermalManager.degHotend(0);
	}
	if (last_temp_hotend_target != thermalManager.degTargetHotend(0)) {
		if(DwinMenuID == DWMENU_POP_FROD_HEAT && ((++flash_mask&0x01) == 0x01))
			DWIN_Draw_UnMaskString_FONT10(State_text_extruder_X + (State_text_extruder_num + 1) * STAT_CHR_W, State_text_extruder_Y, PSTR("    "));		
		else 
			DWIN_Draw_IntValue_FONT10((thermalManager.degTargetHotend(0) > HOTEND_WARNNING_TEMP) ? COLOR_RED : COLOR_WHITE, State_text_extruder_num, State_text_extruder_X + (State_text_extruder_num + 1) * STAT_CHR_W, State_text_extruder_Y, thermalManager.degTargetHotend(0));					
		last_temp_hotend_target = thermalManager.degTargetHotend(0);
	}
#endif
 
 #if HAS_HEATED_BED
	if (last_temp_bed_current != thermalManager.degBed()) {
		DWIN_Draw_IntValue_FONT10(COLOR_WHITE, State_text_bed_num, State_text_bed_X, State_text_bed_Y, thermalManager.degBed());
		last_temp_bed_current = thermalManager.degBed();
	}
	if (last_temp_bed_target != thermalManager.degTargetBed()) {
		DWIN_Draw_IntValue_FONT10(COLOR_WHITE, State_text_bed_num, State_text_bed_X + (State_text_bed_num + 1) * STAT_CHR_W, State_text_bed_Y, thermalManager.degTargetBed());
		last_temp_bed_target = thermalManager.degTargetBed();
	}
 #endif
 
 	/* printing speed*/
	static int16_t last_speed = 0;
	if (last_speed != feedrate_percentage) {
		DWIN_Draw_IntValue_FONT10(COLOR_WHITE, 3, State_text_speed_X, State_text_speed_Y, feedrate_percentage);
		last_speed = feedrate_percentage;
	}
	
	/*Z postion*/
	DWIN_Show_Z_Position(true);

	/*Mixing*/
	TERN_(MIXING_EXTRUDER, DWIN_Show_Extruder_status());
}

void Popup_Window_Temperature(const char *msg, int8_t heaterid) {
	Clear_Dwin_Area(AREA_TITAL|AREA_POPMENU);
	Draw_Popup_Bkgd_105();
	DWIN_Show_ICON(ICON_TEMPTOOHIGH, 102, 165);
	
	if(heaterid == -1)
		dwinLCD.Draw_String(false, true, font10x20, COLOR_RED, COLOR_BG_WINDOW, (272 - (strlen(msg)+8)*10 )/2, 280, PSTR("HEATBED "));
	else
		dwinLCD.Draw_String(false, true, font10x20, COLOR_RED, COLOR_BG_WINDOW, (272 - (strlen(msg)+7)*10)/2, 280, PSTR("HOTEND "));
	dwinLCD.Draw_String(false, true, font10x20, COLOR_RED, COLOR_BG_WINDOW, (272 - (strlen(msg)+8)*10)/2 + 8*10, 280, (char*)msg);
	dwinLCD.Draw_String(false, true, font10x20, COLOR_RED, COLOR_BG_WINDOW, (272 - 16*10)/2, 310, PSTR("Please check it!"));
	dwinLCD.Draw_String(false, true, font10x20, COLOR_RED, COLOR_BG_WINDOW, (272 - 22*10)/2, 340, PSTR("Power off after 5 S !!"));
	dwinLCD.UpdateLCD();
}


#if ENABLED(OPTION_AUTOPOWEROFF)
inline void Draw_Freedown_Machine(){
	dwinLCD.Draw_Rectangle(1, (DwinMenuID==DWMENU_MAIN)?COLOR_BG_BLACK : COLOR_BG_BLUE, 230, 7, 256, 23);
	if(HMI_flag.free_close_timer_rg >= POWERDOWN_MACHINE_TIMER)
		DWIN_Draw_UnMaskString_Default(230, 7, PSTR("   "));
	else if(HMI_flag.free_close_timer_rg >= 100)
		DWIN_Draw_UnMaskIntValue_Default(3, 230, 7, HMI_flag.free_close_timer_rg);
	else if((HMI_flag.free_close_timer_rg < 100)&&(HMI_flag.free_close_timer_rg >= 10))
		DWIN_Draw_IntValue_Default_Color(COLOR_YELLOW, 3, 230, 7, HMI_flag.free_close_timer_rg);		
	else
		DWIN_Draw_IntValue_Default_Color(COLOR_RED, 3, 230, 7, HMI_flag.free_close_timer_rg);
}			

inline void _check_autoshutdown(){
	if(HMI_flag.Autoshutdown_enabled && (DwinMenuID == DWMENU_POP_STOPPRINT || DwinMenuID == DWMENU_MAIN)){
		//is heating?
		if(thermalManager.degTargetHotend(0) > 50 || thermalManager.degTargetBed() > 25){
			HMI_flag.free_close_timer_rg = POWERDOWN_MACHINE_TIMER;
		}
		else if(HMI_flag.free_close_timer_rg == 0){
			queue.inject_P(PSTR("M81"));
			HMI_flag.free_close_timer_rg = POWERDOWN_MACHINE_TIMER;
			return;
		}
		else {
			HMI_flag.free_close_timer_rg--;
			if(HMI_flag.free_close_timer_rg < POWERDOWN_MACHINE_TIPS_TIMER) DWIN_FEEDBACK_TICK();			
		}
		Draw_Freedown_Machine();
	}
	else if(HMI_flag.Is_shutdown) {		
		if(IS_SD_PRINTING() || IS_SD_PAUSED() || IS_SD_FILE_OPEN()){
			HMI_flag.Is_shutdown = false;
		} 
		else if(thermalManager.degHotend(0) < 80){
			HMI_flag.Is_shutdown = false;
			queue.inject_P(PSTR("M81"));
		}
	}
	else
		HMI_flag.free_close_timer_rg = POWERDOWN_MACHINE_TIMER;	
}

#if BOTH(CASE_LIGHT_ENABLE, CASE_LIGHT_SMART) && DISABLED(CASE_LIGHT_NO_BRIGHTNESS)
#include "../../../feature/caselight.h"

_emDWINState old_DWIN_status = ID_SM_IDLE;
/************************
* Smart Case light
* 1. When the machine is not idel, case light shows setting brightness.
* 2. When the machine is idel and hotbed or hotend is hot, the brightness is displayed according to the temperature.
* 3. When the machine is idel and waiting for shut down, and hotbed and hotend are cool, shows the breathing light.
* 4. When the machine is idel and hotbed and hotend are cool, shows settings brightness.
***************************/
inline void SmartCaselightUpdate(){	
	const float brightness_step = (float)caselight.brightness/200;	
	static float BLN_brightness = 0;
	static bool brightness_up = true;
	if(DwinMenuID == DWMENU_SET_CASELIGHTBRIGHTNESS || caselight.brightness == 0) return;
	
	static millis_t next_rts_update_ms = 0;	
	const millis_t ms = millis();	
	if (PENDING(ms, next_rts_update_ms)) 	return;
	next_rts_update_ms = ms + 20;

	///1. not idel
	if(DWIN_status != ID_SM_IDLE){ 
		if(old_DWIN_status == ID_SM_IDLE){
			old_DWIN_status = DWIN_status;
			caselight.update_brightness();
		}
	}
	else{
		if(old_DWIN_status != ID_SM_IDLE)	old_DWIN_status = DWIN_status;
		///2. hotend or hotbed is hot
		
		if(thermalManager.degTargetHotend(0) > 50 || thermalManager.degTargetBed() > 30 || thermalManager.degHotend(0) > 60 || thermalManager.degBed() > 60){
			const uint16_t a = (((uint16_t)thermalManager.degHotend(0))*100)/(HEATER_0_MAXTEMP - HOTEND_OVERSHOOT);
			const uint16_t b = (((uint16_t)thermalManager.degBed())*100)/(BED_MAXTEMP - BED_OVERSHOOT);
			const uint8_t caselight_temp = (_MAX(a, b) * CASE_LIGHT_MAX_PWM)/100;
			caselight.update_brightness_BLN(caselight_temp);
			return;
		}
		else {						
			if(HMI_flag.Autoshutdown_enabled){
				//3. shows breathing light				
				if(brightness_up) {
					BLN_brightness += brightness_step;
					if(BLN_brightness >= caselight.brightness){ BLN_brightness = caselight.brightness; brightness_up = false;}
				}
				else{
					BLN_brightness -= brightness_step;
					if(BLN_brightness <= 0) { BLN_brightness = 0, brightness_up = true;}
				}
				caselight.update_brightness_BLN(round(BLN_brightness));
			}
			else{
				//4. shows default brightness
				if(BLN_brightness != caselight.brightness){
					BLN_brightness = caselight.brightness;
					caselight.update_brightness_BLN(BLN_brightness);
				}
			}
		}
	}
}
#endif

void _reset_shutdown_timer(){
	if(HMI_flag.Autoshutdown_enabled) HMI_flag.free_close_timer_rg = POWERDOWN_MACHINE_TIMER;
}
#endif

void Stop_and_return_mainmenu() {
	HMI_Value.Percentrecord = 0;
	HMI_Value.remain_time = 0;
#ifdef SD_FINISHED_RELEASECOMMAND
  queue.inject_P(PSTR(SD_FINISHED_RELEASECOMMAND));
#endif
	TERN_(POWER_LOSS_RECOVERY, recovery.cancel());
	TERN_(OPTION_AUTOPOWEROFF, _setAutoPowerDown());
	TERN_(MIXING_EXTRUDER, mixer.reset_vtools());	
	HMI_Value.print_speed = feedrate_percentage = 100;
	DWIN_status = ID_SM_IDLE;
	Draw_Main_Menu();
}

void DWIN_HandleScreen() {	
 	switch (DwinMenuID){
		//Main
		case DWMENU_MAIN:    								HMI_MainMenu(); break;
		case DWMENU_FILE:   								HMI_SelectFile(); break;
		case DWMENU_PREPARE:     						HMI_Prepare(); break;
		case DWMENU_CONTROL:     						HMI_Control(); break;
		case DWMENU_INFO:										HMI_Info(); break;

		//print
		case DWMENU_POP_STOPPRINT:  				HMI_Stop_SDPrinting(); break;
		case DWMENU_PRINTING:  							HMI_Printing(); break;
		case DWMENU_TUNE:      							HMI_Tune(); break;
		case DWMENU_TUNE_PRINTSPEED:   			HMI_TunePrintSpeed(); break;
	#if ENABLED(OPTION_FLOWRATE_MENU)
		case DWMENU_TUNE_FLOWRATE:   				HMI_TuneFlowrate(); break;
	#endif
		case DWMENU_POP_PAUSEORSTOP:				HMI_PauseOrStop(); break;
	#if HAS_HEATED_BED
	  case DWMENU_SET_BTMP:   		 				HMI_BedTemp(); break;
	#endif
	#if HAS_FAN
		case DWMENU_SET_FANSPEED:   				HMI_FanSpeed(); break;
	#endif	
  #if ENABLED(BABYSTEPPING)
	  case DWMENU_SET_ZOFFSET:  					HMI_TuneZoffset(); break;
	#endif

		//Prepare
		case DWMENU_HOME:    	 							HMI_Home(); break; 		
		case DWMENU_TEMPERATURE:  	 				HMI_Temperature(); break;		
	#if HAS_PREHEAT
		case DWMENU_PREHEAT_PLA:						HMI_PLAPreheatSetting(); break;
		case DWMENU_PREHEAT_ABS:						HMI_ABSPreheatSetting(); break;
	#endif
		case DWMENU_MOVEAXIS:  							HMI_MoveAxis(); break;			
		case DWMENU_MOVEX:     							HMI_Move_X(); break;
	  case DWMENU_MOVEY:     							HMI_Move_Y(); break;
	  case DWMENU_MOVEZ:     							HMI_Move_Z(); break;
#if HAS_HOTEND
		case DWMENU_SET_ETMP: 							HMI_ETemp(); break;
		case DWMENU_MOVE_EXT1:							HMI_Move_Extr(0); break;
	#if(E_STEPPERS > 1)
		case DWMENU_MOVE_EXT2:							HMI_Move_Extr(1); break;
	#endif
	#if(E_STEPPERS > 2)
		case DWMENU_MOVE_EXT3:							HMI_Move_Extr(2); break;
	#endif
	#if(E_STEPPERS > 3)
		case DWMENU_MOVE_EXT4:							HMI_Move_Extr(3); break;
	#endif
	#if ENABLED(MIXING_EXTRUDER)
		case DWMENU_MOVE_EXTALL:						HMI_Move_AllExtr(); break;	
	#endif   
#endif	
		case DWMENU_FILAMENT:  							HMI_Filament();  break;
		case DWMENU_FILAMENT_PREHEAT: 			HMI_Filament_PretHeat();  break;
		case DWMENU_FILAMENT_EXTRUDER:			HMI_Filament_Extuder();  break;
		case DWMENU_FILAMENT_FEEDLENGTH:  	HMI_Filament_FeedLength();  break;
		case DWMENU_FILAMENT_PURGELENGTH: 	HMI_Filament_PurgeLength();  break;
		case DWMENU_POP_LEVEL_CATCH:				HMI_BedLeveling(); break;
		case DWMENU_LEVELING:								HMI_BedLeveling(); break;
		case DWMENU_LEVEL_DONECONFIRM:			HMI_BedLeveling(); break;
		case DWMENU_LEVEL_SETOFFSET:    		HMI_SetProbZoffset(); break;		
		case DWMENU_LANGUAGE:								HMI_Language(); break;
		case DWMENU_POWERDOWN:							HMI_Powerdown(); break;		
		
		//Control>>Mixer		
	#if ENABLED(MIXING_EXTRUDER)
		case DWMENU_MIXER:     							HMI_Mixer(); break;
		case DWMENU_MIXER_MANUAL: 					HMI_Mixer_Manual(); break;
		case DWMENU_MIXER_GRADIENT: 				HMI_Mixer_Gradient(); break;
		case DWMENU_MIXER_RANDOM: 					HMI_Mixer_Random(); break;
		case DWMENU_MIXER_VTOOL:						HMI_Adjust_Mixer_Vtool(); break;
		case DWMENU_MANUALMIXER_VTOOL:			HMI_Adjust_Mixer_Manual_Vtool(); break;
		case DWMENU_MIXER_EXT1: 						HMI_Adjust_Ext_Percent(0); break;
		case DWMENU_MIXER_EXT2: 						HMI_Adjust_Ext_Percent(1); break;
	#if(MIXING_STEPPERS > 2)		
			case DWMENU_MIXER_EXT3: 					HMI_Adjust_Ext_Percent(2); break;
	#endif
	#if(MIXING_STEPPERS > 3)
			case DWMENU_MIXER_EXT4: 					HMI_Adjust_Ext_Percent(3); break;
	#endif
			
		case DWMENU_MIXER_GRADIENT_ZSTART:	HMI_Adjust_Gradient_Zpos_Start(); break;
		case DWMENU_MIXER_GRADIENT_ZEND:		HMI_Adjust_Gradient_Zpos_End(); break;
		case DWMENU_MIXER_GRADIENT_TSTAR: 	HMI_Adjust_Gradient_VTool_Start(); break;
		case DWMENU_MIXER_GRADIENT_TEND:		HMI_Adjust_Gradient_VTool_End(); break;

		case DWMENU_MIXER_RANDOM_ZSTART:		HMI_Adjust_Random_Zpos_Start(); break;
		case DWMENU_MIXER_RANDOM_ZEND:			HMI_Adjust_Random_Zpos_End(); break;
		case DWMENU_MIXER_RANDOM_HEIGHT:		HMI_Adjust_Random_Height(); break;
		case DWMENU_MIXER_RANDOM_EXTN:			HMI_Adjust_Random_Extruders(); break;
#endif
		//Control>>Config
		case DWMENU_CONFIG:									HMI_Config(); break;	
		//Control>>Config>>Retraction
	#if ENABLED(FWRETRACT) 
		case DWMENU_SET_RETRACT:						HMI_Retract(); break;
		case DWMENU_SET_RETRACT_MM:					HMI_Retract_MM(); break;
		case DWMENU_SET_RETRACT_V:					HMI_Retract_V(); break;
		case DWMENU_SET_RETRACT_ZHOP:				HMI_Retract_ZHOP(); break;		
		case DWMENU_SET_UNRETRACT_MM:				HMI_UnRetract_MM(); break;
		case DWMENU_SET_UNRETRACT_V:				HMI_UnRetract_V(); break;
	#endif	

	#if ENABLED(CASE_LIGHT_MENU) && DISABLED(CASE_LIGHT_NO_BRIGHTNESS)
		case DWMENU_SET_CASELIGHTBRIGHTNESS: HMI_Adjust_Brightness();break;
	#endif

	#if ENABLED(OPTION_HOMEZ_OFFSET)
	  case DWMENU_SET_HOMEZOFFSET:					HMI_Adjust_HomeZOffset(); break;
	#endif

	#if ENABLED(OPTION_HOTENDMAXTEMP)
	  case DWMENU_SET_HOTENDMAXTEMP:			HMI_Adjust_hotend_MaxTemp(); break;
	#endif

	#if ENABLED(PID_EDIT_MENU)
		case DWMENU_PID_TUNE:								HMI_PIDTune(); break;
		case DWMENU_PID_KP:									HMI_PIDTune_KP(); break;
		case DWMENU_PID_KI:									HMI_PIDTune_KI(); break;
		case DWMENU_PID_KD:									HMI_PIDTune_KD(); break;
	#endif
	
	#if ENABLED(PID_AUTOTUNE_MENU)
		case DWMENU_PID_AUTOTUNE:						HMI_PID_AutoTune(); break;
	#endif

	#if ENABLED(OPTION_WIFI_BAUDRATE)
		case DWMENU_SET_WIFIBAUDRATE: 			HMI_Adjust_WiFi_BaudRate(); break;
	#endif

	#if BOTH(OPTION_WIFI_MODULE, OPTION_WIFI_QRCODE)
	case DWMENU_POP_WIFILINK: 						HMI_Pop_WiFiLink(); break;
	#endif

	#if ENABLED(SWITCH_EXTRUDER_MENU)
		case DWMENU_SET_SWITCHEXTRUDER:			HMI_Adjust_Extruder_Sequence(); break;
	#endif
	
	#if ENABLED(BLTOUCH)
		case DWMENU_SET_BLTOUCH: 						HMI_Option_Bltouch(); break;
	#endif

	#if ENABLED(OPTION_REPEAT_PRINTING)
		case DWMENU_SET_REPRINT:						HMI_RepeatPrint(); break;		
		case DWMENU_SET_REPRINT_TIMES:			HMI_Reprint_Times(); break;
		case DWMENU_SET_REPRINT_PUSHLENGTH:	HMI_RePrint_ArmPushLength(); break;
		case DWMENU_SET_REPRINT_BEDTEMP:		HMI_RePrint_BedTemp(); break;
		case DWMENU_SET_REPRINT_ZHEIGTH:		HMI_RepeatPrint_ZHeigth(); break;
	#if HAS_REPEATPRINT_BASE
		case DWMENU_SET_REPRINT_BASEHEIGTH:	HMI_RepeatPrint_BaseHeigth(); break;
	#endif
		case DWMENU_POP_REPEATPRINTING:			HMI_Cancel_RePrint(); break;
	#endif
	
		//Control>>Motion
		case DWMENU_MOTION:    							HMI_Motion(); break;			
	  case DWMENU_SET_MAXSPEED: 					HMI_MaxSpeed(); break;
	  case DWMENU_SET_MAXACC: 						HMI_MaxAcceleration(); break;
	  case DWMENU_SET_MAXJERK:  					HMI_MaxJerk(); break;
	  case DWMENU_SET_STEPPREMM:					HMI_StepPermm(); break;	  
	  case DWMENU_SET_MAXSPEED_VALUE: 		HMI_MaxFeedspeedXYZE(); break;
	  case DWMENU_SET_MAXACC_VALUE: 			HMI_MaxAccelerationXYZE(); break;
	  case DWMENU_SET_MAXJERK_VALUE: 			HMI_MaxJerkXYZE(); break;
	  case DWMENU_SET_STEPPREMM_VALUE: 		HMI_StepXYZE(); break;

		case DWMENU_POP_FROD_OPTION:				HMI_Filament_Runout_Option(); break;
		case DWMENU_POP_FROD_INSERT:				HMI_Filament_Runout_Confirm(); break;
		case DWMENU_POP_FROD_HEAT:					HMI_Filament_Runout_Confirm(); break;

 	#if ENABLED(BABYSTEPPING)
		case DWMENU_TUNE_BABYSTEPS:					HMI_Pop_BabyZstep(); break;
	#endif		
	
	#if ENABLED(OPTION_TEST_MENU)
		case DWMENU_SET_TESTITEM:						HMI_Adjust_Test_item(); break;
	#endif
	
		case DWMENU_POP_WAITING:						HMI_Waiting(); break;

	#if ENABLED(OPTION_NEWS_QRCODE)
		case DWMENU_POP_NEWSLINK:						HMI_Pop_NewsLink(); break;
	#endif
	
	#if ENABLED(OPTION_GUIDE_QRCODE)
		case DWMENU_POP_USERGUIDELINK:			HMI_Pop_UserGuideLink(); break;
	#endif
	
  default: break;
	}
}


#if ENABLED(DEBUG_DWIN_LCD)
void DWIN_Show_Status(){
	switch(DWIN_status){
		case ID_SM_START:
			SERIAL_ECHOLNPGM("DWIN_status = ID_SM_START");
		break;
		default:
		case ID_SM_IDLE:
			SERIAL_ECHOLNPGM("DWIN_status = ID_SM_IDLE");
		break;
		case ID_SM_PRINTING:
			SERIAL_ECHOLNPGM("DWIN_status = ID_SM_PRINTING");
		break;
		case ID_SM_PAUSING:
			SERIAL_ECHOLNPGM("DWIN_status = ID_SM_PAUSING");
		break;
		case ID_SM_PAUSED:
			SERIAL_ECHOLNPGM("DWIN_status = ID_SM_PAUSED");
		break;
		case ID_SM_RESUMING:
			SERIAL_ECHOLNPGM("DWIN_status = ID_SM_RESUMING");
		break;
		case ID_SM_STOPED:
			SERIAL_ECHOLNPGM("DWIN_status = ID_SM_STOPED");
		break;			
	}
}
#endif

//
// Watch for media mount / unmount
//
inline void HMI_SDCardUpdate() {
	if (HMI_flag.lcd_sd_status != card.isMounted()) {
		HMI_flag.lcd_sd_status = card.isMounted();
		// SERIAL_ECHOLNPAIR("HMI_SDCardUpdate: ", int(HMI_flag.lcd_sd_status));
		if(HMI_flag.lcd_sd_status) {
			if(DwinMenuID == DWMENU_FILE) { Redraw_SD_List(); dwinLCD.UpdateLCD();}
		}
		else {
		 // clean file icon
		 if(DwinMenuID == DWMENU_FILE) { Redraw_SD_List();	dwinLCD.UpdateLCD();}
		 else if(DwinMenuID == DWMENU_PRINTING || DwinMenuID == DWMENU_TUNE || printingIsActive()) {
		  	/// TODO: Move card removed abort handling
		  	// to CardReader::manage_media.		  	
		  	wait_for_heatup = wait_for_user = false;
				thermalManager.disable_all_heaters();
				card.flag.abort_sd_printing = true;      		// Let the main loop handle SD abort
			}
		}		
	}
}

void HMI_DWIN_Init() {
	Encoder_Configuration();
	
	dwinLCD.JPG_ShowAndCache(0);
	HMI_SDCardInit();
	
	DWIN_Draw_MaskString_Default_Color(COLOR_WHITE, 15, 300, PSTR("Model: " CUSTOM_MACHINE_NAME));
	DWIN_Draw_MaskString_Default_Color(COLOR_WHITE, 15, 324, PSTR("Firmware Version: " FIRMWARE_VERSION));

	for (uint16_t t = 0; t <= 100; t += 2) {
		DWIN_Show_ICON(ICON_BAR, 15, 260);
		dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 15 + t * 242 / 100, 260, 257, 280);
		dwinLCD.UpdateLCD();
		delay(20);
	}
	HMI_SetLanguage_PicCache();	
	#if BOTH(EEPROM_SETTINGS, OPTION_GUIDE_QRCODE)
	if(HMI_flag.first_power_on){
		Draw_Info_Menu();
		Popup_Window_UserGuideLink();		
		return;
	}
	#endif
	Draw_Main_Menu(true);
}

/***********************************************************
// Multi-language strings for title menu store in 6 and 7
// EN/ES/RU stored in 7.jpg
// FR/PT stored in 6.jpg
***********************************************************/
uint8_t get_title_picID(){	
	switch (HMI_flag.language+1){
		default:
		case LANGUAGE_CASE_EN:
		case LANGUAGE_CASE_SP:
		case LANGUAGE_CASE_RU:
			return 7;
			
		case LANGUAGE_CASE_FR:
		case LANGUAGE_CASE_PT:
			return 6;		
	}
}

void EachMomentUpdate() {	
	
	static millis_t next_rts_update_ms = 0;
	const millis_t ms = millis();	
	if (PENDING(ms, next_rts_update_ms)) 	return;
	next_rts_update_ms = ms + 1000;
	//do it per second
	#if ENABLED(DEBUG_DWIN_LCD)
	if(old_DwinMenuID != DwinMenuID || old_DwinStatus != DWIN_status){
		SERIAL_ECHOLNPAIR("DwinMenuID = ", DwinMenuID);
		DWIN_Show_Status();
		old_DwinMenuID = DwinMenuID;
		old_DwinStatus = DWIN_status;
	}
	#endif

	//variable update
	DWIN_Update_Variable();
	
	//check auto power off
	TERN_(OPTION_AUTOPOWEROFF, _check_autoshutdown());
	
	//check wifi feedback after turn on WiFi module
	TERN_(OPTION_WIFI_MODULE, _check_wifi_feedback());

	//clean the status bar if need
	_check_clean_status_bar();
	
	if(DWIN_status == ID_SM_START){
		//check resume print when power on
		DWIN_status = ID_SM_IDLE;
		//TERN_(POWER_LOSS_RECOVERY, _check_Powerloss_resume());
	}
	else if(DWIN_status == ID_SM_IDLE){
		TERN_(POWER_LOSS_RECOVERY, _check_Powerloss_resume());
	}
#if ENABLED(POWER_LOSS_RECOVERY)
	else if(DWIN_status == ID_SM_RETURN_MAIN){
		if(!queue.has_commands_queued()){
			DWIN_status = ID_SM_IDLE;
			card.mount();
			Draw_Main_Menu(true);
		}
	}
#endif
	else if(DWIN_status == ID_SM_PRINTING){
		TERN_(POWER_LOSS_RECOVERY,recovery.save(false));
		_Update_printing_Timer();		
		
		if(DwinMenuID == DWMENU_TUNE_BABYSTEPS || DwinMenuID == DWMENU_TUNE){
			if(HMI_flag.autoreturntime > 0){
				HMI_flag.autoreturntime--;
				if(HMI_flag.autoreturntime == 0){
					EncoderRate.enabled = false;	
					Draw_Printing_Menu(PRINT_CASE_TUNE, true);
				}
			} 	
		}
	}	
	else if(DWIN_status == ID_SM_RESUMING || DWIN_status == ID_SM_PAUSING){
		_check_kill_times_ElapsedTime();
		#if DISABLED(PARK_HEAD_ON_PAUSE)
		if(HMI_flag.clean_status_delay == 0){			
			if(DWIN_status == ID_SM_RESUMING && printingIsActive() && !runout.filament_ran_out && wait_for_heatup == false){
				DWIN_status = ID_SM_PRINTING;
				Draw_Printing_Menu(PRINT_CASE_PAUSE, true);
			}			
			else if(DWIN_status == ID_SM_PAUSING && printingIsPaused() && !planner.has_blocks_queued() && !runout.filament_ran_out && wait_for_heatup == false){
				DWIN_status = ID_SM_PAUSED;
				Draw_Printing_Menu(PRINT_CASE_PAUSE, true);
			}
		}
		#endif
	}	
	else if(DWIN_status == ID_SM_PAUSED){		
		if(!printingIsPaused()){
				DWIN_status = ID_SM_PRINTING;
				Draw_Printing_Menu(PRINT_CASE_PAUSE, true);
		}
	}	
	else if(DWIN_status == ID_SM_STOPED){
		Stop_and_return_mainmenu();
	}
	else if(DWIN_status == ID_SM_PIDAUTOTUNE){
		char string_Buf[50]={0};
		//ZERO(string_Buf);
		sprintf_P(string_Buf,PSTR("M303 S%3d E0 C8 U1\nM500"),HMI_Value.PIDAutotune_Temp);
		queue.inject(string_Buf);
		DWIN_status = ID_SM_IDLE;
	}
	dwinLCD.UpdateLCD();
}

void DWIN_Update() {
#if ENABLED(DWIN_AUTO_TEST)
	if(HMI_flag.auto_test_flag == 0xaa){
		if(autotest.DWIN_AutoTesting()){
	#if ENABLED(OPTION_TEST_MENU)
			if(!autotest.testflag.autorun)
				Draw_Info_Menu();
			else
	#endif				
				Draw_Main_Menu(true);				
		}
	}
	else
#endif		
	{				
		EachMomentUpdate();  // Status update		
		DWIN_HandleScreen(); // Rotary encoder update
		HMI_SDCardUpdate();	 // SD card update
#if BOTH(CASE_LIGHT_ENABLE, CASE_LIGHT_SMART)
		SmartCaselightUpdate();
#endif
	}
}

#endif // HAS_DWIN_LCD
