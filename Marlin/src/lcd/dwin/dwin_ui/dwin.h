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
#pragma once
#include "../../../inc/MarlinConfig.h"
#if HAS_DWIN_LCD
#include <WString.h>
#include <stdio.h>
#include <string.h>
#include "../../../module/temperature.h"
#include "../../../module/printcounter.h"
#include "../../../module/motion.h"
#include "../../../module/planner.h"
#include "../../../module/tool_change.h"
#include "../../thermistornames.h"
#include "../../../module/settings.h"
#include "../../../libs/buzzer.h"

#include "../../fontutils.h"
#include "../../ultralcd.h"
#include "../../../sd/cardreader.h"
#include "../../../MarlinCore.h"
#include "../../../core/serial.h"
#include "../../../core/macros.h"
#include "../../../gcode/queue.h"

#if ENABLED(HOST_ACTION_COMMANDS)
  #include "../../../feature/host_actions.h"
#endif
#if ENABLED(FILAMENT_RUNOUT_SENSOR)
#include "../../../feature/runout.h"
#endif
#include "../../../feature/pause.h"
#if HAS_BED_PROBE
  #include "../../../module/probe.h"
#endif
#if ENABLED(BABYSTEPPING)
  #include "../../../feature/babystep.h"
#endif
#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../../../feature/powerloss.h"
#endif
#if HAS_LEVELING
  #include "../../../feature/bedlevel/bedlevel.h"
#endif
#if ENABLED(BLTOUCH)
  #include "../../../feature/bltouch.h"
#endif
#if ENABLED(FWRETRACT)
  #include "../../../feature/fwretract.h"
#endif
#if ENABLED(MIXING_EXTRUDER)
  #include "../../../feature/mixing.h"
#endif

//#define	DEBUG_DWIN_LCD
#define	DWIN_AUTO_TEST
//#define	SDCARD_TEST

#include "../language/dwin_multi_language.h"
#include "../dwin_lcd.h"
#include "../rotary_encoder.h"
#include "dwin_comm.h"
#include "autotest.h"
#include "DwinMenu_main.h"
#include "DwinMenu_Infor.h"
#include "DwinMenu_Control.h"
#include "DwinMenu_Prepare.h"
#include "DwinMenu_Print.h"

#if ENABLED(OPTION_REPEAT_PRINTING)
#include "DwinMenu_RepeatPrint.h"
#endif

#if USE_BEEPER
#define	DWIN_FEEDBACK_TICK()				do{buzzer.tone( 10,  500);} while(0)
#define	DWIN_FEEDBACK_TIPS()				do{buzzer.tone( 10, 3000);} while(0)
#define	DWIN_FEEDBACK_CONFIRM()			do{buzzer.tone( 50, 3000);buzzer.tone( 50, 0);buzzer.tone( 50, 1000);} while(0)
#define	DWIN_FEEDBACK_WARNNING()		do{buzzer.tone(200, 4000);buzzer.tone(100, 0);buzzer.tone(200, 4000);} while(0)
#define	DWIN_FEEDBACK_WARNNING2()		do{buzzer.tone(400, 4000);buzzer.tone(400, 2000);} while(0)
#else
#define	DWIN_FEEDBACK_TICK()				NOOP
#define	DWIN_FEEDBACK_TIPS()				NOOP
#define	DWIN_FEEDBACK_CONFIRM()			NOOP
#define	DWIN_FEEDBACK_WARNNING()		NOOP
#define	DWIN_FEEDBACK_WARNNING2()		NOOP
#endif

#define DWIN_REMAIN_TIME_UPDATE_INTERVAL 	10000
#define POWERDOWN_MACHINE_TIMER 					900	//seconds
#define	POWERDOWN_MACHINE_TIPS_TIMER			10//seconds

typedef enum{
	ID_SM_START = 0,
	ID_SM_IDLE,
	ID_SM_PRINTING,	
	ID_SM_PAUSING,				//pause >> wait paused
	ID_SM_PAUSED,
	ID_SM_RESUMING,				//Resuming
	ID_SM_STOPED,					//	
	ID_SM_RUNOUTING,
#if ENABLED(OPTION_REPEAT_PRINTING)	
	ID_SM_REPEATPRITING,	
#endif
#if ENABLED(PID_AUTOTUNE_MENU)
	ID_SM_PIDAUTOTUNE,
	ID_SM_PIDAUTOTUNING,
#endif
#if ENABLED(DWIN_AUTO_TEST)
	ID_SM_AUTOTESTING,
#endif
#if ENABLED(SDCARD_TEST)
	ID_SM_SDCARDTESTING,
#endif	
	ID_SM_RETURN_MAIN
}_emDWINState;

extern _emDWINState DWIN_status;

typedef enum {
  // Process ID
  DWMENU_MAIN = 0,
  DWMENU_FILE,
  DWMENU_PREPARE,
  DWMENU_CONTROL,
  DWMENU_INFO,
  
  //Print Menu
  DWMENU_PRINTING,
  DWMENU_TUNE,

	//Prepare menu
  DWMENU_HOME,
  DWMENU_TEMPERATURE,
  DWMENU_PREHEAT_PLA,
	DWMENU_PREHEAT_ABS = 10,	
  DWMENU_MOVEAXIS,
  DWMENU_MOVEX,
  DWMENU_MOVEY,
  DWMENU_MOVEZ,
	DWMENU_MOVE_EXT1,
	DWMENU_MOVE_EXT2,
	DWMENU_MOVE_EXT3,
	DWMENU_MOVE_EXT4,
	DWMENU_MOVE_EXTALL,	
	
	DWMENU_FILAMENT = 20,
	DWMENU_FILAMENT_PREHEAT,
	DWMENU_FILAMENT_EXTRUDER,
	DWMENU_FILAMENT_FEEDLENGTH,
	DWMENU_FILAMENT_PURGELENGTH,
	DWMENU_LEVELING,
	DWMENU_LEVEL_CATCHPROBEZOFFSET,
	DWMENU_LEVEL_SETHOMEOFFSET,
	DWMENU_LEVEL_SETPROBEOFFSET,
	DWMENU_LEVEL_BEDLEVELING,
	DWMENU_LEVEL_DONECONFIRM,
		
  //Control menu  
  DWMENU_MIXER = 31,
  DWMENU_CONFIG,
  DWMENU_MOTION,
  DWMENU_POWERDOWN,
	DWMENU_LANGUAGE,
	DWMENU_SET_ETMP,
	DWMENU_SET_BTMP,
  DWMENU_SET_FANSPEED,  
	
	//Tune
	DWMENU_TUNE_PRINTSPEED = 40,
	DWMENU_TUNE_FLOWRATE,
	DWMENU_TUNE_BABYSTEPS,
  
  //Control>>Motion
  DWMENU_SET_MAXSPEED = 50,
  DWMENU_SET_MAXSPEED_VALUE,
  DWMENU_SET_MAXACC,
  DWMENU_SET_MAXACC_VALUE,
  DWMENU_SET_MAXJERK,
  DWMENU_SET_MAXJERK_VALUE,
  DWMENU_SET_STEPPREMM,
  DWMENU_SET_STEPPREMM_VALUE,
  DWMENU_SET_ZOFFSET,

	//Control>>Mixer
	DWMENU_MIXER_SETVTOOL = 60,
	DWMENU_MIXER_GRADIENT,
	DWMENU_MIXER_RANDOM,
	DWMENU_MIXER_VTOOL,
	//Control>>Mixer>>manual
	DWMENU_SETVTOOL_VTOOL,
	DWMENU_MIXER_EXT1,
	DWMENU_MIXER_EXT2,
	DWMENU_MIXER_EXT3,
	DWMENU_MIXER_EXT4,
	//Control>>Mixer>>Gradient
	DWMENU_MIXER_GRADIENT_ZSTART,
	DWMENU_MIXER_GRADIENT_ZEND,
	DWMENU_MIXER_GRADIENT_TSTAR,
	DWMENU_MIXER_GRADIENT_TEND,	
	DWMENU_MIXER_GRADIENT_STOP,	
	//Control>>Mixer>>Random
	DWMENU_MIXER_RANDOM_ZSTART,
	DWMENU_MIXER_RANDOM_ZEND,
	DWMENU_MIXER_RANDOM_HEIGHT,
	DWMENU_MIXER_RANDOM_EXTN,	
	DWMENU_MIXER_RANDOM_STOP,	
 	//config
 	DWMENU_SET_BLTOUCH  = 80,
 	DWMENU_SET_RETRACT,
 	///config>>Retract
 	DWMENU_SET_RETRACT_MM,
 	DWMENU_SET_RETRACT_V,
 	DWMENU_SET_RETRACT_ZHOP,
 	DWMENU_SET_UNRETRACT_MM,
 	DWMENU_SET_UNRETRACT_V,	 	
#if ENABLED(LIN_ADVANCE) 	
 	///config>>Linear Advance
 	DWMENU_SET_LINADVANCE,
 	DWMENU_SET_LINADVANCE_E0,
 	DWMENU_SET_LINADVANCE_EN = (DWMENU_SET_LINADVANCE_E0 + EXTRUDERS - 1),
#endif
 	///config>>Home Offset
 	DWMENU_SET_HOMEOFFSET,
 	DWMENU_SET_HOMEOFFSET_X,
 	DWMENU_SET_HOMEOFFSET_Y,
	DWMENU_SET_HOMEOFFSET_Z,
	///config>>Probe Offset
	DWMENU_SET_PROBEOFFSET,
	DWMENU_SET_PROBEOFFSET_X,
 	DWMENU_SET_PROBEOFFSET_Y,
	DWMENU_SET_PROBEOFFSET_Z,
 	///config>>Case light brightness
 	DWMENU_SET_CASELIGHTBRIGHTNESS,	
	///config>>PID Tune
	DWMENU_PID_TUNE,
	DWMENU_PID_KP,
	DWMENU_PID_KI,
	DWMENU_PID_KD,
	DWMENU_PID_AUTOTUNE,
	///
	DWMENU_SET_WIFIBAUDRATE,
	///
	DWMENU_SET_SWITCHEXTRUDER,	
	//Repeat printing
	DWMENU_SET_REPRINT,	
	DWMENU_SET_REPRINT_TIMES,
	DWMENU_SET_REPRINT_PUSHLENGTH,
	DWMENU_SET_REPRINT_BEDTEMP,
	DWMENU_SET_REPRINT_ZHEIGTH,
	DWMENU_SET_REPRINT_BASEHEIGTH,	
	//Auto test
	DWMENU_SET_TESTITEM,
	DWMENU_SDDCARDTEST,

	// Pop Menu
	DWMENU_POP_HOME = 200,
	DWMENU_POP_REMOVE_GLASS,
	DWMENU_POP_STOPPRINT,
	DWMENU_POP_FROD_OPTION,
	DWMENU_POP_FROD_INSERT,
	DWMENU_POP_FROD_HEAT,
	DWMENU_POP_PAUSEORSTOP,	
	DWMENU_POP_WAITING,
	DWMENU_POP_REPEATPRINTING,
	DWMENU_POP_USERGUIDELINK,
	DWMENU_POP_NEWSLINK,
	DWMENU_POP_FAQLINK,
	DWMENU_POP_WIFILINK,
	DWMENU_POP_HEATRUNAWAY,
	DWMENU_POP_ZOFFSETWIZARD,
	DWMENU_POP_ADJHOMEZOFFSET,
	
	DWMENU_END
}_emDWIN_MENUID_;

extern _emDWIN_MENUID_ DwinMenuID;


#define	TITLE_X		14
#define	TITLE_Y		7

//
// logo  offset define
//
#define LOGO_OFFSET_X         		20
#define LOGO_OFFSET_Y         		45

//
// Status Area offset define
//
#define State_space_Y         		20
#define State_icon_offset_X    		13
#define State_icon_offset_Y    		381
#define State_text_offset_X    		State_icon_offset_X + STAT_CHR_W*2
#define State_text_offset_Y    		State_icon_offset_Y + 1
#define State_string_offset_X    	State_icon_offset_X + STAT_CHR_W*5
#define State_string_offset_Y    	State_icon_offset_Y + 2

#define State_text_extruder_num		3
#define State_icon_extruder_X 		State_icon_offset_X
#define State_icon_extruder_Y		State_icon_offset_Y
#define State_text_extruder_X		State_text_offset_X
#define State_text_extruder_Y		State_text_offset_Y
#define State_string_extruder_X		State_string_offset_X
#define State_string_extruder_Y		State_string_offset_Y

#define State_text_bed_num			3
#define State_icon_bed_X 			State_icon_offset_X + DWIN_WIDTH/2
#define State_icon_bed_Y			State_icon_offset_Y
#define State_text_bed_X			State_text_offset_X	+ DWIN_WIDTH/2
#define State_text_bed_Y			State_text_offset_Y
#define State_string_bed_X			State_string_offset_X + DWIN_WIDTH/2
#define State_string_bed_Y			State_string_offset_Y

#define State_text_speed_num		3
#define State_icon_speed_X 			State_icon_offset_X
#define State_icon_speed_Y			State_icon_offset_Y + STAT_CHR_H + State_space_Y
#define State_text_speed_X			State_text_offset_X
#define State_text_speed_Y			State_text_offset_Y + STAT_CHR_H + State_space_Y
#define State_string_speed_X		State_string_offset_X
#define State_string_speed_Y		State_string_offset_Y + STAT_CHR_H + State_space_Y

#define State_text_Zoffset_inum		3
#define State_text_Zoffset_fnum		2
#define State_icon_Zoffset_X 		State_icon_offset_X + DWIN_WIDTH/2
#define State_icon_Zoffset_Y		State_icon_offset_Y + STAT_CHR_H + State_space_Y
#define State_text_Zoffset_X		State_text_offset_X + DWIN_WIDTH/2
#define State_text_Zoffset_Y		State_text_offset_Y + STAT_CHR_H + State_space_Y
#define State_string_Zoffset_X		State_string_offset_X + DWIN_WIDTH/2
#define State_string_Zoffset_Y		State_string_offset_Y + STAT_CHR_H + State_space_Y

#define State_text_vtool_num		2

#define State_text_mix_num			3
#define State_icon_mix_X 			13
#define State_icon_mix_Y			45
#define State_text_mix_X			13
#define STATE_TEXT_MIX_Y			State_icon_mix_Y + 25

//
// Menu Area offset define
//
#define Menu_control_start_temp_X		57
#define Menu_control_start_temp_Y		104
#define Menu_control_end_temp_X			84
#define Menu_control_end_temp_Y			116

#define Menu_control_start_motion_X		Menu_control_start_temp_X + 30
#define Menu_control_start_motion_Y		Menu_control_start_temp_Y
#define Menu_control_end_motion_X		Menu_control_end_temp_X + 30
#define Menu_control_end_motion_Y		Menu_control_end_temp_Y

#define Menu_control_start_mixer_X		Menu_control_start_motion_X + 30
#define Menu_control_start_mixer_Y		Menu_control_start_motion_Y
#define Menu_control_end_mixer_X		Menu_control_end_motion_X + 30
#define Menu_control_end_mixer_Y		Menu_control_end_motion_Y

#define Menu_control_start_store_X		Menu_control_start_mixer_X + 30
#define Menu_control_start_store_Y		Menu_control_start_mixer_Y
#define Menu_control_end_store_X		Menu_control_end_mixer_X + 30
#define Menu_control_end_store_Y		Menu_control_end_mixer_Y

#define Menu_control_start_read_X		Menu_control_start_store_X + 30
#define Menu_control_start_read_Y		Menu_control_start_store_Y
#define Menu_control_end_read_X			Menu_control_end_store_X + 30
#define Menu_control_end_read_Y			Menu_control_end_store_Y

#define Menu_control_start_reset_X		Menu_control_start_read_X + 30
#define Menu_control_start_reset_Y		Menu_control_start_read_Y
#define Menu_control_end_reset_X		Menu_control_end_read_X + 30
#define Menu_control_end_reset_Y		Menu_control_end_read_Y

#define Menu_control_start_info_X		Menu_control_start_reset_X + 30
#define Menu_control_start_info_Y		Menu_control_start_reset_Y
#define Menu_control_end_info_X			Menu_control_end_reset_X + 30
#define Menu_control_end_info_Y			Menu_control_end_reset_Y

#if ENABLED(MIXING_EXTRUDER)
typedef struct Mixer_Display_cfg{
	uint16_t Extruder_X_Coordinate[E_STEPPERS] = {0};
	uint8_t Extruder_Int_Number[E_STEPPERS] = {0};
	uint16_t VTool_X_Coordinate = 0;
	uint8_t VTool_Int_Number = 0;
}MIXER_DIS;
extern MIXER_DIS MixerDis;
#endif

#if ENABLED(SWITCH_EXTRUDER_MENU)
typedef enum{
	SE_DEFAULT = 0,
#if (MIXING_STEPPERS == 2)
	SE_E1E2 = SE_DEFAULT,
	SE_E2E1,
#elif (MIXING_STEPPERS == 3)
	SE_E1E2E3 = SE_DEFAULT,	
	SE_E1E3E2,
	SE_E2E1E3,
	SE_E2E3E1,
	SE_E3E1E2,
	SE_E3E2E1,
#elif (MIXING_STEPPERS == 4)
	SE_E1E2E3E4 = SE_DEFAULT,
	SE_E1E2E4E3,	//E3<-->E4
	SE_E1E3E2E4,	//E2<-->E3
	SE_E1E3E4E2,	//E2<-->E3
	SE_E1E4E2E3,	//	
	SE_E1E4E3E2,	//
	
	SE_E2E1E3E4,	//
	SE_E2E1E4E3,	//	
	SE_E2E3E1E4,	//
	SE_E2E3E4E1,	//
	SE_E2E4E1E3,	//
	SE_E2E4E3E1,	//
	
	SE_E3E1E2E4,	//	
	SE_E3E1E4E2,	//		
	SE_E3E2E1E4,	//
	SE_E3E2E4E1,	//	
	SE_E3E4E1E2,	//
	SE_E3E4E2E1,	//

	SE_E4E1E2E3,	//
	SE_E4E1E3E2,	//
	SE_E4E2E1E3,	//
	SE_E4E2E3E1,	//
	SE_E4E3E1E2,	//
	SE_E4E3E2E1,	//
#endif	
	SE_END
}_emSwitchExtruder;
#endif


#ifndef PID_AUTOTUNE_CYCLES
#define PID_AUTOTUNE_CYCLES		4
#endif

typedef struct {
  TERN_(HAS_HOTEND,     		int16_t E_Temp    = EXTRUDE_MINTEMP);
  TERN_(HAS_HEATED_BED, 		int16_t Bed_Temp  = 30);
  TERN_(HAS_PREHEAT,    		int16_t Fan_speed = 0);
	TERN_(PID_AUTOTUNE_MENU,	int16_t PIDAutotune_Temp = 200);
	TERN_(PID_AUTOTUNE_MENU,	uint8_t PIDAutotune_cycles = PID_AUTOTUNE_CYCLES);
  int16_t print_speed     	= 100;
	int16_t flowrate     			= 100;
  int16_t Max_Feedrate     = 0;
  int16_t Max_Acceleration  = 0;
  int16_t Max_Jerk          = 0;
  int16_t Steps_per_mm      = 0;
  int16_t Move_X_scale      = 0;
  int16_t Move_Y_scale      = 0;
  int16_t Move_Z_scale      = 0;	
  int16_t Auto_Zstart_scale  = 0;
  int16_t Auto_Zend_scale    = 0;
  int16_t Random_Zstart_scale  = 0;
  int16_t Random_Zend_scale    = 0;
  int16_t Random_Height = 0;
	
	#if HAS_OFFSET_MENU
	int16_t HomeOffsetX_scale = 0;
	int16_t HomeOffsetY_scale = 0;
	int16_t HomeOffsetZ_scale = 0;
	#endif
	
	#if HAS_PROBE_XY_OFFSET
	int16_t ProbeOffsetX_scale = 0;
	int16_t ProbeOffsetY_scale = 0;
	int16_t ProbeOffsetZ_scale = 0;
	#endif

	#if ENABLED(FWRETRACT)
  int16_t Retract_MM_scale      = 0;
  int16_t Retract_V_scale      	= 0;
	int16_t Retract_ZHOP_scale    = 0;
  int16_t unRetract_MM_scale    = 0;
  int16_t unRetract_V_scale     = 0;
	#endif

	#if ENABLED(LIN_ADVANCE)
	int16_t extruder_advance_K[EXTRUDERS];
	#endif
	
	#if ENABLED(PID_EDIT_MENU) 	
	int16_t PIDKp      = 0;
	int16_t PIDKi      = 0;
	int16_t PIDKd      = 0;
	#endif
	
	int16_t babyZoffset_Scale  = 0;
	int16_t ProbeZoffset_Scale = 0;
	
  #if HAS_HOTEND	
	int16_t Current_E_Scale[E_STEPPERS];
	float Last_E_Coordinate[E_STEPPERS];
	
	//load and unload filament
	uint8_t	load_extruder = 1;
	int16_t nozzle_Temp    = 200;
	int16_t	feedlength = FILAMENT_CHANGE_FAST_LOAD_LENGTH;
	int16_t	purgelength = FILAMENT_CHANGE_SLOW_LOAD_LENGTH;
	
	#if ENABLED(MIXING_EXTRUDER)
	int8_t old_mix_mode = -1;
	int16_t Current_EAll_Scale    = 0;
	float Last_EAll_Coordinate    = 0;
	int8_t current_vtool = 0;
	mixer_perc_t mix_percent[MIXING_STEPPERS] = {0};
	#endif
  #endif
	
	#if (ENABLED(OPTION_REPEAT_PRINTING) && HAS_REPEATPRINT_BASE)
	int16_t RePrintBaseHeigth  = 0;
	#endif
		
	uint8_t Percentrecord = 0;
	uint32_t remain_time = 0;
	millis_t dwin_heat_time = 0;
	uint32_t elapsed_value = 0;
	
	TERN_(SWITCH_EXTRUDER_MENU, int8_t switchExtruder = SE_DEFAULT);

} HMI_value_t;
extern HMI_value_t HMI_Value;

typedef enum{
	SHOWED_TUNE = 0,
	SHOWED_TEMPERATURE,	
	SHOWED_PEHEAT_PLA,
	SHOWED_PEHEAT_ABS
}_emShowMode;


typedef struct {  
  bool 	select_flag:1,
   			lcd_sd_status:1,
   			Is_purged:1,
   			Is_retracted:1,
   			Is_shutdown:1,
   			Is_temperror:1,
   			needshutdown:1
   			
		#if ENABLED(OPTION_GUIDE_QRCODE)
			 ,first_power_on:1
		#endif		
		#if (HAS_LEVELING)
				,Leveling_Menu_Fg:1
				,AutoLeveling_Fg:1
				,NeedAutoLeveling:1
		#endif
		#if ENABLED(OPTION_AUTOPOWEROFF)
				,Autoshutdown_enabled:1
		#endif
		#if ENABLED(FILAMENT_RUNOUT_SENSOR)
				,IS_cancel_runout:1
		#endif
		#if ENABLED(OPTION_ABORT_UNLOADFILAMENT)   
				,AutoUnload_enabled:1
   			,AutoUnloadFilament_on:1
   	#endif
		;

	uint16_t	clean_status_delay = 0;
	uint8_t language;
	
  #if ENABLED(OPTION_WIFI_MODULE)  
  uint8_t wifi_link_timer;
  #endif
  
	#if ENABLED(LCD_BED_LEVELING)	 
  uint8_t Leveling_Case_Total = LEVELING_CASE_HOMEALL;
  #endif
  
	uint8_t killtimes = 0;
	uint8_t killElapsedTime = 0;
	int8_t temperrorBeepTimes = 0;
 	
  #if ENABLED(OPTION_AUTOPOWEROFF)
		uint16_t free_close_timer_rg = POWERDOWN_MACHINE_TIMER;
  #endif 

	_emShowMode show_mode  = SHOWED_TUNE;
	uint8_t autoreturntime = 0;
	
  AxisEnum axis;
} HMI_Flag_t;
extern HMI_Flag_t HMI_flag;

class DwinMenu{
	private:
			
	public:		 	
		static uint8_t now, last;	
		static uint8_t index;
  	void set(uint8_t v) { now = last = v; }
  	void reset() { set(0); index = MROWS;}
  	bool changed() { bool c = (now != last); if (c) last = now; return c; }
  	bool dec() { if (now) now--; return changed(); }
  	bool inc(uint8_t v) { if (now < (v - 1)) now++; else now = (v - 1); return changed(); }
};

extern DwinMenu DwinMenu_main;

extern DwinMenu DwinMenu_prepare;
extern DwinMenu DwinMenu_home;
extern DwinMenu DwinMenu_move;
extern DwinMenu DwinMenu_temp;
extern DwinMenu DwinMenu_filament;
extern DwinMenu DwinMenu_leveling;
extern DwinMenu DwinMenu_powerdown;
extern DwinMenu DwinMenu_HomeZoffsetWizard;
extern DwinMenu DwinMenu_language;

extern DwinMenu DwinMenu_control;
extern DwinMenu DwinMenu_motion;
extern DwinMenu DwinMenu_PreheatPLA;
extern DwinMenu DwinMenu_PreheatABS;
extern DwinMenu DwinMenu_feedrate;
extern DwinMenu DwinMenu_accel;
extern DwinMenu DwinMenu_jerk;
extern DwinMenu DwinMenu_step;
#if ENABLED(MIXING_EXTRUDER)
extern DwinMenu DwinMenu_mixer;
extern DwinMenu DwinMenu_manualmix;
extern DwinMenu DwinMenu_GradientMix;
extern DwinMenu DwinMenu_RandomMix;
#endif
#if ENABLED(BLTOUCH)
extern DwinMenu DwinMenu_bltouch;
#endif
#if ENABLED(FWRETRACT)
extern DwinMenu DwinMenu_fwretract;
#endif
#if ENABLED(LIN_ADVANCE) && (EXTRUDERS > 1)
extern DwinMenu DwinMenu_LinAdvance;
#endif
#if ENABLED(PID_EDIT_MENU)
extern DwinMenu DwinMenu_PIDTune;
#endif
extern DwinMenu DwinMenu_configure;

extern DwinMenu DwinMenu_print;
extern DwinMenu DwinMenu_file;
extern DwinMenu DwinMenu_RunoutOption;
extern DwinMenu DwinMenu_tune;

extern DwinMenu DwinMenu_infor;

#if ENABLED(OPTION_REPEAT_PRINTING)
extern DwinMenu DwinMenu_reprint;
#endif

#if HAS_OFFSET_MENU
extern DwinMenu DwinMenu_Homeoffset;
#endif
#if HAS_PROBE_XY_OFFSET
extern DwinMenu DwinMenu_Probeoffset;
#endif

#if ENABLED(SDCARD_TEST)
extern DwinMenu DwinMenu_SDTest;
#endif


#if ENABLED(OPTION_AUTOPOWEROFF)
void _reset_shutdown_timer();
#endif
void set_status_msg_showtime(const uint16_t t);
uint8_t get_title_picID();

typedef enum{	
	ERROR_MAX_TEMP = 0
	,ERROR_THERMAL_RUNAWAY
	,ERROR_HEATING_FAILED
	,ERROR_MIN_TEMP	
#if HAS_PID_HEATING
	,ERROR_PID_TEMP_TOO_HIGH
	,ERROR_PID_TIMEOUT
#endif	
#if ENABLED(TEMP_SENSOR_1_AS_REDUNDANT)
	,ERROR_REDUNDANT_TEMP
#endif
	,ERROR_NOOP
}_emTempErrorID;

void Popup_Temperature_Runaway(heater_id_t heaterid, _emTempErrorID errorid);
void Stop_and_return_mainmenu();
void Abort_SD_Printing();
void DWIN_Show_logo();
void HMI_DWIN_Init();
void DWIN_Update();
void EachMomentUpdate();
void DWIN_HandleScreen();
void DWIN_Show_M117(const char * const message);
#endif
