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
#if HAS_CUTTER
#include "../../feature/spindle_laser.h"
#endif
#if HAS_DWIN_LCD
#include "../rotary_encoder.h"
#include "../dwin_lcd.h"
#include "dwin_comm.h"
#include "dwin.h"

//Menu item
//
#if ENABLED(MIXING_EXTRUDER)
//Control >> Mixer
enum {
	MIXER_CASE_BACK = 0,
	MIXER_CASE_SETVTOOL,
	MIXER_CASE_CURRENTVTOOL,
#if ENABLED(GRADIENT_MIX)
	MIXER_CASE_GRADIENT,
#endif
#if ENABLED(RANDOM_MIX)
	MIXER_CASE_RANDOM,	
#endif 
	MIXER_CASE_END
};
#define	MIXER_CASE_TOTAL 4

//Control >> Mixer >> Manual
enum {
	SETVTOOL_CASE_BACK = 0,
	SETVTOOL_CASE_VTOOL,
	SETVTOOL_CASE_EXTRUDER1,	//MANUAL_CASE_EXTRUDER2,
	SETVTOOL_CASE_EXTRUDER2,	//MANUAL_CASE_EXTRUDER2,
#if(MIXING_STEPPERS > 2)
	SETVTOOL_CASE_EXTRUDER3,	//MANUAL_CASE_EXTRUDER3,
#endif
#if(MIXING_STEPPERS > 3)
	SETVTOOL_CASE_EXTRUDER4,	//MANUAL_CASE_EXTRUDER4,
#endif
	SETVTOOL_CASE_COMMIT, 		//MANUAL_CASE_COMMIT,
	SETVTOOL_CASE_END,				//MANUAL_CASE_END
};
#define	MANUAL_CASE_TOTAL (MIXING_STEPPERS+2)


//Control >> Mixer >> Gradient
#ifdef GRADIENT_MIX
enum {
	GRADIENT_CASE_BACK = 0,
	GRADIENT_CASE_ZPOS_START,
	GRADIENT_CASE_ZPOS_END,
	GRADIENT_CASE_VTOOL_START,
	GRADIENT_CASE_VTOOL_END,
	GRADIENT_CASE_STOP,
	GRADIENT_CASE_END
};
#define	GRADIENT_CASE_TOTAL 5
#endif



//Control >> Mixer >> Random
#ifdef RANDOM_MIX
enum {
	RANDOM_CASE_BACK = 0,
	RANDOM_CASE_ZPOS_START,
	RANDOM_CASE_ZPOS_END,
	RANDOM_CASE_HEIGHT,
	RANDOM_CASE_EXTRUDERS,
	RANDOM_CASE_STOP,
	RANDOM_CASE_END
};
#define	RANDOM_CASE_TOTAL 5
#endif
#endif

//Control >> configure
enum {
	CONFIG_CASE_BACK = 0,
#if ENABLED(FWRETRACT)
	CONFIG_CASE_RETRACT,
#endif
#if ENABLED(LIN_ADVANCE)
	CONFIG_CASE_LINADVANCE,
#endif
#if ENABLED(OPTION_REPEAT_PRINTING)
	CONFIG_CASE_REPRINT,
#endif
#if ENABLED(FILAMENT_RUNOUT_SENSOR)
	CONFIG_CASE_FILAMENTRUNOUT,
#endif
#if ENABLED(OPTION_ABORT_UNLOADFILAMENT)
	CONFIG_CASE_FILAMENTAUTOUNLOAD,
#endif
#if ENABLED(POWER_LOSS_RECOVERY)
	CONFIG_CASE_POWERLOSS,
#endif
#if ENABLED(OPTION_AUTOPOWEROFF)
	CONFIG_CASE_SHUTDOWN,
#endif
#if ENABLED(CASE_LIGHT_MENU)
	CONFIG_CASE_BACKLIGHT,
#endif
#if ENABLED(OPTION_WIFI_MODULE)
	CONFIG_CASE_WIFI,	
#endif
#if BOTH(OPTION_WIFI_MODULE, OPTION_WIFI_BAUDRATE)
	CONFIG_CASE_WIFISPEED,
#endif
#if ENABLED(OPTION_LASER)
	CONFIG_CASE_LASER,
#endif
#if ENABLED(SPINDLE_FEATURE)
	CONFIG_CASE_SPINDLE,
#endif
#if BOTH(MIXING_EXTRUDER, OPTION_MIXING_SWITCH)
	CONFIG_CASE_MIXERENABLE,
#if ENABLED(SWITCH_EXTRUDER_MENU)
	CONFIG_CASE_SWITCHEXTRUDER,
#endif
#endif	
#if HAS_OFFSET_MENU
	CONFIG_CASE_HOMEOFFSET,
#endif
#if HAS_LEVELING	
#if HAS_PROBE_XY_OFFSET
	CONFIG_CASE_PROBEOFFSET,
#endif
	CONFIG_CASE_LEVELING,	
	CONFIG_CASE_ACTIVELEVEL,
#endif
#if EITHER(PID_EDIT_MENU, PID_AUTOTUNE_MENU)
	CONFIG_CASE_PIDTUNE,
#endif
	CONFIG_CASE_END
};
#define	CONFIG_TUNE_CASE_TOTAL (ENABLED(FWRETRACT)+ENABLED(LIN_ADVANCE)+ENABLED(OPTION_REPEAT_PRINTING)+ENABLED(FILAMENT_RUNOUT_SENSOR)+ENABLED(OPTION_ABORT_UNLOADFILAMENT)+ENABLED(POWER_LOSS_RECOVERY)+ ENABLED(OPTION_AUTOPOWEROFF)+ENABLED(CASE_LIGHT_MENU)+ENABLED(OPTION_WIFI_MODULE))
#define	CONFIG_CASE_TOTAL (CONFIG_TUNE_CASE_TOTAL + BOTH(OPTION_WIFI_MODULE, OPTION_WIFI_BAUDRATE) + ENABLED(OPTION_LASER) + ENABLED(SPINDLE_FEATURE) + ENABLED(SWITCH_EXTRUDER_MENU) + BOTH(MIXING_EXTRUDER, OPTION_MIXING_SWITCH) + HAS_OFFSET_MENU + EITHER(PID_EDIT_MENU, PID_AUTOTUNE_MENU) + HAS_LEVELING*2 + HAS_PROBE_XY_OFFSET)

//Control >> Configure >> Auto retract
#if ENABLED(FWRETRACT)
enum {
	RETRACT_CASE_BACK = 0,
#if ENABLED(FWRETRACT_AUTORETRACT)		
	RETRACT_CASE_AUTO,
#endif
	RETRACT_CASE_RETRACT_MM,
	RETRACT_CASE_RETRACT_V,
	RETRACT_CASE_RETRACT_ZHOP,
	RETRACT_CASE_RECOVER_MM,
	RETRACT_CASE_RECOVER_V,
	RETRACT_CASE_END
};
#define	RETRACT_CASE_TOTAL	(5 + ENABLED(FWRETRACT_AUTORETRACT))
#endif

#if ENABLED(LIN_ADVANCE) && (EXTRUDERS > 1)
enum {
	LINADVANCE_CASE_BACK = 0,
	LINADVANCE_CASE_E0,
	LINADVANCE_CASE_END = LINADVANCE_CASE_E0 + E_STEPPERS
};
#define	LINADVANCE_CASE_TOTAL	E_STEPPERS
#endif


#if HAS_OFFSET_MENU
enum {
	HOMEOFFSET_CASE_BACK = 0,
	HOMEOFFSET_CASE_X,
	HOMEOFFSET_CASE_Y,
	HOMEOFFSET_CASE_Z,
	HOMEOFFSET_CASE_END
};
#endif

#if HAS_PROBE_XY_OFFSET
enum {
	PROBEOFFSET_CASE_BACK = 0,
	PROBEOFFSET_CASE_X,
	PROBEOFFSET_CASE_Y,
	PROBEOFFSET_CASE_Z,
	PROBEOFFSET_CASE_END
};
#endif

//Control >> PID_EDIT_MENU >> Auto retract
#if ENABLED(PID_EDIT_MENU)
enum {
	PIDTUNE_CASE_BACK = 0,	
	PIDTUNE_CASE_KP,
	PIDTUNE_CASE_KI,
	PIDTUNE_CASE_KD,
#if ENABLED(PID_AUTOTUNE_MENU)
	PIDTUNE_CASE_AUTO,
#endif
	PIDTUNE_CASE_END
};
#define	PIDTUNE_CASE_TOTAL	(3 + ENABLED(PID_AUTOTUNE_MENU))
#endif


//Control >> Configure >> BLTouch
#if ENABLED(BLTOUCH)
enum {
	BLTOUCH_CASE_BACK = 0,
	BLTOUCH_CASE_RESET,
	BLTOUCH_CASE_TEST,
	BLTOUCH_CASE_STOW,
	BLTOUCH_CASE_DEPLOY,
	BLTOUCH_CASE_SW,
	BLTOUCH_CASE_END
};
#define	BLTOUCH_CASE_TOTAL	5
#endif

//Control >> Configure >> Preheat
enum {
	PREHEAT_CASE_BACK = 0,
#if HAS_HOTEND
	PREHEAT_CASE_TEMP,
#endif
#if HAS_HEATED_BED
	PREHEAT_CASE_BED,
#endif
#if HAS_FAN
	PREHEAT_CASE_FAN,
#endif
#if ENABLED(EEPROM_SETTINGS)
	PREHEAT_CASE_SAVE,
#endif
	PREHEAT_CASE_END
};
#define	PREHEAT_CASE_TOTAL	(HAS_HOTEND+HAS_HEATED_BED+HAS_FAN+ENABLED(EEPROM_SETTINGS))

//Control >> Configure >> Motion
enum {
	MOTION_CASE_BACK = 0,
	MOTION_CASE_FEEDRATE,
	MOTION_CASE_ACCEL,
#if ENABLED(HAS_CLASSIC_JERK)
	MOTION_CASE_JERK,
#endif	
	MOTION_CASE_STEPS,
	MOTION_CASE_END
};
#define	MOTION_CASE_TOTAL	(3+ENABLED(HAS_CLASSIC_JERK))

enum {
	MAXFEEDRATE_CASE_BACK = 0,
	MAXFEEDRATE_CASE_X,
	MAXFEEDRATE_CASE_Y,
	MAXFEEDRATE_CASE_Z,
	MAXFEEDRATE_CASE_E,
	MAXFEEDRATE_CASE_END = XYZE_N + 1	
};
#define	MAXFEEDRATE_CASE_TOTAL	(XYZE_N)

enum {
	MAXACCL_CASE_BACK = 0,
	MAXACCL_CASE_X,
	MAXACCL_CASE_Y,
	MAXACCL_CASE_Z,
	MAXACCL_CASE_E,
	MAXACCL_CASE_END = XYZE_N + 1	

};
#define	MAXACCL_CASE_TOTAL	(XYZE_N)

enum {
	MAXJERK_CASE_BACK = 0,
	MAXJERK_CASE_X,
	MAXJERK_CASE_Y,
	MAXJERK_CASE_Z,
	MAXJERK_CASE_E,
	MAXJERK_CASE_END
};
#define	MAXJERK_CASE_TOTAL	(MAXJERK_CASE_E)

enum {
	STEPSMM_CASE_BACK = 0,
	STEPSMM_CASE_X,
	STEPSMM_CASE_Y,
	STEPSMM_CASE_Z,
	STEPSMM_CASE_E,	
	STEPSMM_CASE_END = XYZE_N + 1
};
#define	STEPSMM_CASE_TOTAL	(XYZE_N)

//Control
enum {
	CONTROL_CASE_BACK = 0,
#if ENABLED(MIXING_EXTRUDER)		
	CONTROL_CASE_MIXER,
#endif
	CONTROL_CASE_CONFIG,
	CONTROL_CASE_MOTION,
	CONTROL_CASE_SETPLA,
	CONTROL_CASE_SETABS,
#if ENABLED(BLTOUCH)	
	CONTROL_CASE_BLTOUCH,
#endif
#if ENABLED(EEPROM_SETTINGS)
	CONTROL_CASE_SAVE,
	CONTROL_CASE_LOAD,
	CONTROL_CASE_RESET,
#endif	
	CONTROL_CASE_END
};
#define	CONTROL_CASE_TOTAL	(4+ENABLED(MIXING_EXTRUDER)+ENABLED(BLTOUCH)+3*ENABLED(EEPROM_SETTINGS))

int8_t mixing_menu_adjust();
void Draw_Control_Menu(const uint8_t MenuItem = 0);

		
#if ENABLED(MIXING_EXTRUDER)
void Draw_Mixer_Menu(const uint8_t MenuItem = 0);
void HMI_Adjust_Mixer_SetVtool_Vtool();
void HMI_Adjust_Ext_Percent(uint8_t Extruder_Number);
void HMI_Adjust_Mixer_Vtool();
void HMI_Mixer_SetVtool();
		
#if ENABLED(GRADIENT_MIX)
void HMI_Adjust_Gradient_Zpos_Start(); 
void HMI_Adjust_Gradient_Zpos_End(); 
void HMI_Adjust_Gradient_VTool_Start();
void HMI_Adjust_Gradient_VTool_End(); 
void HMI_Mixer_Gradient(); 
#endif
		
#if ENABLED(RANDOM_MIX)
void HMI_Adjust_Random_Zpos_Start();
void HMI_Adjust_Random_Zpos_End();
void HMI_Adjust_Random_Height();
void HMI_Adjust_Random_Extruders();
void HMI_Mixer_Random();
#endif
void HMI_Mixer(); 
#endif//MIXING_EXTRUDER

void Draw_Config_Menu(const uint8_t MenuItem = 0);

#if ENABLED(FWRETRACT)
void HMI_Retract_MM();
void HMI_Retract_V();
void HMI_Retract_ZHOP();
void HMI_UnRetract_MM();
void HMI_UnRetract_V();
void HMI_Retract(); 
#endif

#if ENABLED(LIN_ADVANCE)
#if (EXTRUDERS > 1)
void HMI_Set_LinearAdvance(uint8_t e);
void HMI_LinearAdvance();
#else
void HMI_Set_LinearAdvance();
#endif
#endif


#if ENABLED(CASE_LIGHT_MENU) && DISABLED(CASE_LIGHT_NO_BRIGHTNESS)
void HMI_Adjust_Brightness();
#endif

void HMI_Adjust_MaxFeedrate(); 
void HMI_MaxFeedrate();


void HMI_Adjust_MaxAcceleration(); 
void HMI_MaxAcceleration(); 

void HMI_Adjust_MaxJerk(); 
void HMI_MaxJerk(); 

void HMI_Adjust_Steppermm(); 
void HMI_StepPermm(); 

void HMI_Motion();


#if ENABLED(BLTOUCH)
void HMI_Option_Bltouch();
#endif



#if HAS_OFFSET_MENU
void HMI_Adjust_HomeOffset();
void HMI_Adjust_HomeOffset_X();
void HMI_Adjust_HomeOffset_Y();
void HMI_Adjust_HomeOffset_Z();
#endif

#if HAS_PROBE_XY_OFFSET
void HMI_Adjust_ProbeOffset();
void HMI_Adjust_ProbeOffset_X();
void HMI_Adjust_ProbeOffset_Y();
void HMI_Adjust_ProbeOffset_Z();
#endif


#if ENABLED(SWITCH_EXTRUDER_MENU)
void HMI_Adjust_Extruder_Sequence();
#endif

#if ENABLED(PID_EDIT_MENU) 
void HMI_PIDTune();
void HMI_PIDTune_KP();
void HMI_PIDTune_KI();
void HMI_PIDTune_KD();
void Draw_PIDTune_Menu();
#endif

#if ENABLED(PID_AUTOTUNE_MENU)
void HMI_PID_AutoTune();
void DWIN_Show_PIDAutoTune(uint8_t cycles, uint8_t ncycles);
void DWIN_Show_PIDAutoTuneDone();
#endif


#if ENABLED(OPTION_WIFI_MODULE)
#if ENABLED(OPTION_WIFI_BAUDRATE)
void HMI_Adjust_WiFi_BaudRate();
#endif
#if ENABLED(OPTION_WIFI_QRCODE)
void Popup_Window_WiFiLink(char *strIPAddress);
void HMI_Pop_WiFiLink();
#endif
#endif

void HMI_Config();

#if HAS_PREHEAT
void HMI_PLAPreheatSetting();
void HMI_ABSPreheatSetting();  
#endif

void HMI_Control(); 

#endif
