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
 * DwinMenu_Prepare.cpp
 */

 #include "../../../inc/MarlinConfig.h"

#if HAS_DWIN_LCD
#include "dwin.h"

#if ENABLED(OPTION_ZOFFSETWIZARD)	
int16_t old_HomeZoffset_Scale = 0;
#endif

void DWIN_PopMenu_HomeDone() {
	if(DwinMenuID == DWMENU_POP_HOME) Draw_Home_Menu(false);
	else if(DwinMenuID == DWMENU_LEVELING) Draw_Leveling_Menu(false);	
#if ENABLED(OPTION_ZOFFSETWIZARD)		
	else if(DwinMenuID == DWMENU_POP_ZOFFSETWIZARD || DwinMenuID == DWMENU_POP_ADJHOMEZOFFSET) DWIN_PopMenu_HomeZOffsetSetup();	
#endif
#if ENABLED(AUTO_UPDATA_PROBE_Z_OFFSET)
	else if(DwinMenuID == DWMENU_LEVEL_CATCHPROBEZOFFSET) DWIN_PopMenu_CatchProbeZOffset();
#endif	
	else
		Draw_Home_Menu(true);
	dwinLCD.UpdateLCD();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Prepare Main menu
//
static void Item_Prepare_Move(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MOVE, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_AXIS);
	Draw_More_Icon(row);
}

static void Item_Prepare_Temp(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_TEMPERATURE, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_TEMPERATURE);
	Draw_More_Icon(row);
}

static void Item_Prepare_Home(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_AUTO_HOME, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_HOMING);
	Draw_More_Icon(row);
}

static void Item_Prepare_Filament(const uint8_t row) {
#if (DWINLCD_MENU_VERSION >= 2)
	DWIN_Show_MultiLanguage_String(MTSTRING_FILAMENT, LBLX, MBASE(row));
#else
	DWIN_Draw_UnMaskString_FONT10(LBLX, MBASE(row), PSTR("Filament"));
#endif
	Draw_Menu_Line(row,ICON_CURSOR);
	Draw_More_Icon(row);
}

static void Item_Prepare_Leveling(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_BED, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_LEVELING, LBLX+get_MultiLanguageString_Width(MTSTRING_BED)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_LEVELING0);
	Draw_More_Icon(row);
}

static void Item_Prepare_Disable(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_DISABLE_STEPPERS, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_CLOSEMOTOR);
}

static void Item_Prepare_Lang(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_LANGUAGE, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_LANGUAGE);
	Draw_More_Icon(row);
}

#if (HAS_SUICIDE || ENABLED(PSU_CONTROL))	
static void Item_Prepare_Powerdown(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_POWER_OUTAGE, LBLX, MBASE(row));
	if(HMI_flag.language == 0)
		DWIN_Show_MultiLanguage_String(MTSTRING_POWER_OFF, LBLX+56, MBASE(row));
	Draw_Menu_Line(row, ICON_POWERDOWN);
}
#endif

void Draw_Prepare_Menu(const uint8_t MenuItem) {
	DwinMenuID = DWMENU_PREPARE;
	DwinMenu_prepare.set(MenuItem);
	DwinMenu_prepare.index = _MAX(DwinMenu_prepare.now, MROWS);

#if (PREPARE_CASE_TOTAL > MROWS)	
	const int8_t scrol = MROWS - DwinMenu_prepare.index;
	#define PSCROL(L) (scrol + (L))
#else
	#define PSCROL(L) (L)
#endif
	#define PVISI(L) WITHIN(PSCROL(L), 0, MROWS)

	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_STATUS);

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_PREPARE, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
 
	if (PVISI(PREPARE_CASE_BACK)) Draw_Back_First(DwinMenu_prepare.now == PREPARE_CASE_BACK); // < Back 
	
	if (PVISI(PREPARE_CASE_HOME)) Item_Prepare_Home(PSCROL(PREPARE_CASE_HOME));   					// Auto Home	
	if (PVISI(PREPARE_CASE_MOVE)) Item_Prepare_Move(PSCROL(PREPARE_CASE_MOVE));   					// Move
	if (PVISI(PREPARE_CASE_TEMP)) Item_Prepare_Temp(PSCROL(PREPARE_CASE_TEMP));   					// Temperature
	if (PVISI(PREPARE_CASE_FILAMENT)) Item_Prepare_Filament(PSCROL(PREPARE_CASE_FILAMENT)); // filament
	if (PVISI(PREPARE_CASE_LEVELING)) Item_Prepare_Leveling(PSCROL(PREPARE_CASE_LEVELING)); // Leveling 
	if (PVISI(PREPARE_CASE_DISA)) Item_Prepare_Disable(PSCROL(PREPARE_CASE_DISA)); 					 // Disable Stepper 
	if (PVISI(PREPARE_CASE_LANG)) Item_Prepare_Lang(PSCROL(PREPARE_CASE_LANG));   					 // Language CN/EN
#if (HAS_SUICIDE || ENABLED(PSU_CONTROL))	
	if (PVISI(PREPARE_CASE_POWERDOWN)) Item_Prepare_Powerdown(PSCROL(PREPARE_CASE_POWERDOWN));   // Powerdown
#endif	
	if (DwinMenu_prepare.now) Draw_Menu_Cursor(PSCROL(DwinMenu_prepare.now));
	Draw_Status_Area();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Prepare >> HOME
//
inline void Popup_Window_HomeAll(const bool parking=false) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * (parking ? 7 : 10)) / 2, 230, parking ? PSTR("Parking") : PSTR("Homing XYZ"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 23) / 2, 260, PSTR("Please wait until done."));
}

#if ENABLED(COMBINE_HOMEINGXY_MENU)
inline void Popup_Window_HomeXY(const bool parking=false) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * (parking ? 7 : 10)) / 2, 230, parking ? PSTR("Parking") : PSTR("Homing X&Y"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 23) / 2, 260, PSTR("Please wait until done."));
}
#else
inline void Popup_Window_HomeX(const bool parking=false) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * (parking ? 7 : 10)) / 2, 230, parking ? PSTR("Parking") : PSTR("Homing X"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 23) / 2, 260, PSTR("Please wait until done."));
}

inline void Popup_Window_HomeY(const bool parking=false) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * (parking ? 7 : 10)) / 2, 230, parking ? PSTR("Parking") : PSTR("Homing Y"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 23) / 2, 260, PSTR("Please wait until done."));
}
#endif

inline void Popup_Window_HomeZ(const bool parking=false) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * (parking ? 7 : 10)) / 2, 230, parking ? PSTR("Parking") : PSTR("Homing Z"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 23) / 2, 260, PSTR("Please wait until done."));
}

inline void Item_Home_All(const uint8_t row){
	DWIN_Show_MultiLanguage_String(MTSTRING_HOME, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_ALL, LBLX+get_MultiLanguageString_Width(MTSTRING_HOME)+10, MBASE(row));
	Draw_Menu_Line(row,ICON_HOME_ALL);
}

#if ENABLED(COMBINE_HOMEINGXY_MENU)
inline void Item_Home_XY(const uint8_t row){
	DWIN_Show_MultiLanguage_String(MTSTRING_HOME, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_X, LBLX+get_MultiLanguageString_Width(MTSTRING_HOME)+10, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Y, LBLX+get_MultiLanguageString_Width(MTSTRING_HOME)+24, MBASE(row));
	Draw_Menu_Line(row,ICON_HOME_ALL);
}
#else
inline void Item_Home_X(const uint8_t row){
	DWIN_Show_MultiLanguage_String(MTSTRING_HOME, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_X, LBLX+get_MultiLanguageString_Width(MTSTRING_HOME)+10, MBASE(row));
	Draw_Menu_Line(row,ICON_HOME_X);
}

inline void Item_Home_Y(const uint8_t row){
	DWIN_Show_MultiLanguage_String(MTSTRING_HOME, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Y, LBLX+get_MultiLanguageString_Width(MTSTRING_HOME)+10, MBASE(row));
	Draw_Menu_Line(row,ICON_HOME_Y);
}
#endif
inline void Item_Home_Z(const uint8_t row){
	DWIN_Show_MultiLanguage_String(MTSTRING_HOME, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z, LBLX+get_MultiLanguageString_Width(MTSTRING_HOME)+10, MBASE(row));
	Draw_Menu_Line(row,ICON_HOME_Z);
}


void Draw_Home_Menu(bool reset_item) {
	DwinMenuID = DWMENU_HOME;
	if(reset_item) DwinMenu_home.reset();
	//DwinMenu_home.index = _MAX(DwinMenu_home.now, MROWS);
	
#if (HOME_CASE_TOTAL > MROWS)
	const int8_t scrol = MROWS - DwinMenu_home.index;
	#define HSCROL(L) (scrol + (L))
#else
	#define HSCROL(L) (L)
#endif
	#define HVISI(L) WITHIN(HSCROL(L), 0, MROWS)
	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_HOME, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	if(HVISI(HOME_CASE_BACK)) Draw_Back_First(DwinMenu_home.now == HOME_CASE_BACK);
	
	if(HVISI(HOME_CASE_BACK)) Item_Home_All(HSCROL(HOME_CASE_ALL));
#if ENABLED(COMBINE_HOMEINGXY_MENU)
	if(HVISI(HOME_CASE_XY)) Item_Home_XY(HSCROL(HOME_CASE_XY));	
#else
	if(HVISI(HOME_CASE_X)) Item_Home_X(HSCROL(HOME_CASE_X));
	if(HVISI(HOME_CASE_Y)) Item_Home_Y(HSCROL(HOME_CASE_Y));
#endif
	if(HVISI(HOME_CASE_Z)) Item_Home_Z(HSCROL(HOME_CASE_Z));
	
	if(DwinMenu_home.now) Draw_Menu_Cursor(HSCROL(DwinMenu_home.now));
}

void HMI_Home() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_home.inc(HOME_CASE_END)) {
			Move_Highlight(1, DwinMenu_home.now);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_home.dec()) {
			Move_Highlight(-1, DwinMenu_home.now);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_home.now) {
			case 0: 										// Back
				Draw_Prepare_Menu(PREPARE_CASE_HOME);
			break;

			case HOME_CASE_ALL: 										
				DwinMenuID = DWMENU_POP_HOME;
				DwinMenu_home.index = MROWS;
				queue.inject_P(PSTR("G28")); 	// G28 will set home_flag
				Popup_Window_HomeAll();
			break;

		#if ENABLED(COMBINE_HOMEINGXY_MENU)
			case HOME_CASE_XY: 										
				DwinMenuID = DWMENU_POP_HOME;
				DwinMenu_home.index = MROWS;
				TERN(HOME_Y_BEFORE_X, queue.inject_P(PSTR("G28 YX")), queue.inject_P(PSTR("G28 XY")));				
				Popup_Window_HomeXY();
			break;
		#else
			case HOME_CASE_X:
				DwinMenuID = DWMENU_POP_HOME;
				DwinMenu_home.index = MROWS;
				queue.inject_P(PSTR("G28 X0"));
				Popup_Window_HomeX();
			break;

			case HOME_CASE_Y: 										
				DwinMenuID = DWMENU_POP_HOME;
				DwinMenu_home.index = MROWS;
				queue.inject_P(PSTR("G28 Y0"));
				Popup_Window_HomeY();
			break;
		#endif

			case HOME_CASE_Z: 										
				DwinMenuID = DWMENU_POP_HOME;
				DwinMenu_home.index = MROWS;
				queue.inject_P(PSTR("G28 Z0"));
				Popup_Window_HomeZ();
			break;

			default:break;
		} 
	}
	dwinLCD.UpdateLCD();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Prepare >> Temperature
//
static void Item_Temperature_ETemp(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_NOZZLE, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_TEMP, LBLX+get_MultiLanguageString_Width(MTSTRING_NOZZLE)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_SETENDTEMP);
  DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), thermalManager.degTargetHotend(0));
}


static void Item_Temperature_BTemp(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_BED, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_TEMP, LBLX+get_MultiLanguageString_Width(MTSTRING_BED)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_SETENDTEMP);
  DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), thermalManager.degTargetBed());
}


static void Item_Temperature_FANSpeed(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_FANSPEED, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_FANSPEED);
  DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), thermalManager.fan_speed[0]);
}

#if HAS_PREHEAT
 static void Item_Temperature_Cool(const uint8_t row) {
 	DWIN_Show_MultiLanguage_String(MTSTRING_COOLDOWN, LBLX, MBASE(row));
  Draw_Menu_Line(row, ICON_COOL);
 }
#endif

static void Item_Temperature_PLA(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_PREHEAT, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_PLA, LBLX+get_MultiLanguageString_Width(MTSTRING_PREHEAT)+10, MBASE(row));
	Draw_Menu_Line(row, ICON_PLAPREHEAT);
}

static void Item_Temperature_ABS(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_PREHEAT, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_ABS, LBLX+get_MultiLanguageString_Width(MTSTRING_PREHEAT)+10, MBASE(row));
	Draw_Menu_Line(row, ICON_ABSPREHEAT);
}

void Draw_Temperature_Menu() {
	DwinMenuID = DWMENU_TEMPERATURE;
	DwinMenu_temp.reset();
	//DwinMenu_temp.index = _MAX(DwinMenu_temp.now, MROWS);
	
#if (TEMP_CASE_TOTAL > MROWS)
	const int8_t scrol = MROWS - DwinMenu_temp.index;
	#define TCSCROL(L) (scrol + (L))
#else
	#define TCSCROL(L) (L)
#endif
 	#define TCVISI(L) WITHIN(TCSCROL(L), 0, MROWS)

	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_TEMPERATURE, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);

	if (TCVISI(TEMP_CASE_BACK)) Draw_Back_First(DwinMenu_temp.now == TEMP_CASE_BACK);// < Back 
	
	if (TCVISI(TEMP_CASE_ETEMP)) Item_Temperature_ETemp(TCSCROL(TEMP_CASE_ETEMP));
	if (TCVISI(TEMP_CASE_BTEMP)) Item_Temperature_BTemp(TCSCROL(TEMP_CASE_BTEMP));
	if (TCVISI(TEMP_CASE_FAN)) Item_Temperature_FANSpeed(TCSCROL(TEMP_CASE_FAN));
	if (TCVISI(TEMP_CASE_PREHEATPLA)) Item_Temperature_PLA(TCSCROL(TEMP_CASE_PREHEATPLA));
	if (TCVISI(TEMP_CASE_PREHEATABS)) Item_Temperature_ABS(TCSCROL(TEMP_CASE_PREHEATABS));
	if (TCVISI(TEMP_CASE_COOL)) Item_Temperature_Cool(TCSCROL(TEMP_CASE_COOL));

	if (DwinMenu_temp.now) Draw_Menu_Cursor(TCSCROL(DwinMenu_temp.now));
}

void HMI_Temperature() {
 ENCODER_DiffState encoder_diffState = get_encoder_state();
 if (encoder_diffState == ENCODER_DIFF_NO) return;

 // Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_temp.inc(TEMP_CASE_END)){
			if (DwinMenu_temp.now > MROWS && DwinMenu_temp.now > DwinMenu_temp.index) {
				DwinMenu_temp.index = DwinMenu_temp.now;
				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);
				if (DwinMenu_temp.index == TEMP_CASE_COOL) Item_Temperature_Cool(MROWS);
			}
			else {
				Move_Highlight(1, DwinMenu_temp.now + MROWS - DwinMenu_temp.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_temp.dec()) {
			if (DwinMenu_temp.now < DwinMenu_temp.index - MROWS) {
		    DwinMenu_temp.index--;
		    Scroll_Menu(DWIN_SCROLL_DOWN);
		    if(DwinMenu_temp.index - MROWS == TEMP_CASE_BACK) Draw_Back_First();				
				else if(DwinMenu_temp.index - MROWS == TEMP_CASE_ETEMP) Item_Temperature_ETemp(0);
				else if(DwinMenu_temp.index - MROWS == TEMP_CASE_BTEMP ) Item_Temperature_BTemp(0);
			}
			else {
				Move_Highlight(-1, DwinMenu_temp.now + MROWS - DwinMenu_temp.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
	  switch (DwinMenu_temp.now) {
	   case 0: // Back
	    Draw_Prepare_Menu(PREPARE_CASE_TEMP);
	    break;
			
	   #if HAS_HOTEND
	    case TEMP_CASE_ETEMP: // Nozzle temperature
	     DwinMenuID = DWMENU_SET_ETMP;			 
	     HMI_Value.E_Temp = _MAX(thermalManager.degTargetHotend(0), EXTRUDE_MINTEMP);
			 if(HMI_Value.E_Temp > HOTEND_WARNNING_TEMP)
				 DWIN_Draw_Warn_IntValue_Default(3, MENUVALUE_X+8, MBASE(TEMP_CASE_ETEMP + MROWS - DwinMenu_temp.index), HMI_Value.E_Temp);
			 else
	     	 DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(TEMP_CASE_ETEMP + MROWS - DwinMenu_temp.index), HMI_Value.E_Temp);
	     EncoderRate.enabled = true;
	     break;
	   #endif
		 
	   #if HAS_HEATED_BED
	    case TEMP_CASE_BTEMP: // Bed temperature
	     DwinMenuID = DWMENU_SET_BTMP;
	     HMI_Value.Bed_Temp = _MAX(thermalManager.degTargetBed(), 50);
	     DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(TEMP_CASE_BTEMP + MROWS - DwinMenu_temp.index), HMI_Value.Bed_Temp);
	     EncoderRate.enabled = true;
	     break;
	   #endif
		 
	   #if HAS_FAN
	    case TEMP_CASE_FAN: // Fan speed
	     DwinMenuID = DWMENU_SET_FANSPEED;
	     HMI_Value.Fan_speed = thermalManager.fan_speed[0];
	     DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(TEMP_CASE_FAN + MROWS - DwinMenu_temp.index), HMI_Value.Fan_speed);
	     EncoderRate.enabled = true;
	     break;
	   #endif
		 
		#if (HAS_HOTEND && HAS_PREHEAT)
			case TEMP_CASE_PREHEATPLA: // PLA preheat
				thermalManager.setTargetHotend(ui.material_preset[0].hotend_temp, 0);
				thermalManager.setTargetBed(ui.material_preset[0].bed_temp);
				thermalManager.set_fan_speed(0, ui.material_preset[0].fan_speed);
			break;
						
			case TEMP_CASE_PREHEATABS: // ABS preheat
				thermalManager.setTargetHotend(ui.material_preset[1].hotend_temp, 0);
				thermalManager.setTargetBed(ui.material_preset[1].bed_temp);
				thermalManager.set_fan_speed(0, ui.material_preset[1].fan_speed);
			break;
	 #endif
	 
		#if HAS_PREHEAT
			case TEMP_CASE_COOL: // Cool
				TERN_(HAS_FAN, thermalManager.zero_fan_speeds());
				thermalManager.disable_all_heaters();
			break;
		#endif
		}
	}
	dwinLCD.UpdateLCD();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Prepare >> Move
//
float last_Extr_scale[E_STEPPERS];
static void _init_Move_Extr(){
	#define EXTR_STEPPER_LOOP(VAR) for (uint_fast8_t VAR = 0; VAR < E_STEPPERS; VAR++)
	EXTR_STEPPER_LOOP(i){ 
		last_Extr_scale[i] = 0;
		HMI_Value.Current_E_Scale[i] = 0;
		HMI_Value.Last_E_Coordinate[i] = 0.0;
	}
}

static void Item_Axis_MoveX(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MOVE, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_X, LBLX+get_MultiLanguageString_Width(MTSTRING_MOVE)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_MOVEX);
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), current_position.x * MINUNITMULT);
}

static void Item_Axis_MoveY(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MOVE, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Y, LBLX+get_MultiLanguageString_Width(MTSTRING_MOVE)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_MOVEY);
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), current_position.y * MINUNITMULT);
}

static void Item_Axis_MoveZ(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MOVE, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z, LBLX+get_MultiLanguageString_Width(MTSTRING_MOVE)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_MOVEZ);
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), current_position.z * MINUNITMULT);
}

#if HAS_HOTEND
static void Item_Axis_MoveEX1(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_EXTRUDER, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_1, LBLX+get_MultiLanguageString_Width(MTSTRING_EXTRUDER)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_EXTRUDER1);
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), HMI_Value.Current_E_Scale[0]);
}

#if (E_STEPPERS > 1)
static void Item_Axis_MoveEX2(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_EXTRUDER, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_2, LBLX+get_MultiLanguageString_Width(MTSTRING_EXTRUDER)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_EXTRUDER2);
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), HMI_Value.Current_E_Scale[1]);
}
#endif

#if (E_STEPPERS > 2)
static void Item_Axis_MoveEX3(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_EXTRUDER, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_3, LBLX+get_MultiLanguageString_Width(MTSTRING_EXTRUDER)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_EXTRUDER3);
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), HMI_Value.Current_E_Scale[2]);
}
#endif

#if (E_STEPPERS > 3)
static void Item_Axis_MoveEX4(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_EXTRUDER, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_4, LBLX+get_MultiLanguageString_Width(MTSTRING_EXTRUDER)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_EXTRUDER4);
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), HMI_Value.Current_E_Scale[3]);
}
#endif

#if ENABLED(MIXING_EXTRUDER)
static void Item_Axis_MoveEXAll(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_EXTRUDER, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_ALL, LBLX+get_MultiLanguageString_Width(MTSTRING_EXTRUDER)+6, MBASE(row));
	Draw_Menu_Line(row, ICON_LANGUAGE);
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), HMI_Value.Current_EAll_Scale);
}
#endif
#endif

void Draw_Move_Menu() {	
	DwinMenuID = DWMENU_MOVEAXIS;
	DwinMenu_move.reset();
	//DwinMenu_move.index = _MAX(DwinMenu_move.now, MROWS);

#if (AXISMOVE_CASE_TOTAL > MROWS)
	const int8_t scrol = MROWS - DwinMenu_move.index;
	#define MSCROL(L) (scrol + (L))
#else
	#define MSCROL(L) (L)
#endif
	#define MVISI(L) WITHIN(MSCROL(L), 0, MROWS)

	_init_Move_Extr();
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_MOVE, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	if (MVISI(AXISMOVE_CASE_BACK)) Draw_Back_First(DwinMenu_move.now == AXISMOVE_CASE_BACK); // < Back
	
	if (MVISI(AXISMOVE_CASE_MOVEX)) Item_Axis_MoveX(MSCROL(AXISMOVE_CASE_MOVEX));
	if (MVISI(AXISMOVE_CASE_MOVEY)) Item_Axis_MoveY(MSCROL(AXISMOVE_CASE_MOVEY));
	if (MVISI(AXISMOVE_CASE_MOVEZ)) Item_Axis_MoveZ(MSCROL(AXISMOVE_CASE_MOVEZ));	
	if (MVISI(AXISMOVE_CASE_EX1)) Item_Axis_MoveEX1(MSCROL(AXISMOVE_CASE_EX1));
#if(E_STEPPERS > 1)
	if (MVISI(AXISMOVE_CASE_EX2)) Item_Axis_MoveEX2(MSCROL(AXISMOVE_CASE_EX2));
#endif
#if(E_STEPPERS > 2)
	if (MVISI(AXISMOVE_CASE_EX3)) Item_Axis_MoveEX3(MSCROL(AXISMOVE_CASE_EX3));
#endif
#if(E_STEPPERS > 3)
	if (MVISI(AXISMOVE_CASE_EX4)) Item_Axis_MoveEX4(MSCROL(AXISMOVE_CASE_EX4));
#endif	
#if ENABLED(MIXING_EXTRUDER)
	if (MVISI(AXISMOVE_CASE_EXALL)) Item_Axis_MoveEXAll(MSCROL(AXISMOVE_CASE_EXALL));
#endif

	if (DwinMenu_move.now) Draw_Menu_Cursor(MSCROL(DwinMenu_move.now));
}

void HMI_Move_X() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Move_X_scale)) {
			if(!planner.is_full()) {		 
				// Wait for planner moves to finish!
				planner.synchronize();
				planner.buffer_line(current_position, MMM_TO_MMS(HOMING_FEEDRATE_XY), active_extruder);

				DwinMenuID = DWMENU_MOVEAXIS;
				EncoderRate.enabled = false;
				DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_MOVEX), HMI_Value.Move_X_scale);
			}			
		}
		else{
			NOLESS(HMI_Value.Move_X_scale, (X_MIN_POS) * MINUNITMULT);
			NOMORE(HMI_Value.Move_X_scale, (X_MAX_POS) * MINUNITMULT);
			current_position.x = (float)HMI_Value.Move_X_scale / MINUNITMULT;
			DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_MOVEX), HMI_Value.Move_X_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Move_Y() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Move_Y_scale)) {			
			if(!planner.is_full()) {		 
				// Wait for planner moves to finish!
				planner.synchronize();
				planner.buffer_line(current_position, MMM_TO_MMS(HOMING_FEEDRATE_XY), active_extruder);

				DwinMenuID = DWMENU_MOVEAXIS;
				EncoderRate.enabled = false;
				DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_MOVEY), HMI_Value.Move_Y_scale);
			}			
		}
		else{
			NOLESS(HMI_Value.Move_Y_scale, (Y_MIN_POS) * MINUNITMULT);
			NOMORE(HMI_Value.Move_Y_scale, (Y_MAX_POS) * MINUNITMULT);
			current_position.y = (float)HMI_Value.Move_Y_scale / MINUNITMULT;
			DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_MOVEY), HMI_Value.Move_Y_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Move_Z() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Move_Z_scale)) {			
			if(!planner.is_full()) {		 
				//Wait for planner moves to finish!
				planner.synchronize();
				planner.buffer_line(current_position, MMM_TO_MMS(HOMING_FEEDRATE_Z), active_extruder);
				
				DwinMenuID = DWMENU_MOVEAXIS;
				EncoderRate.enabled = false;
				DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_MOVEZ), HMI_Value.Move_Z_scale);
			}
		}
		else{
			NOLESS(HMI_Value.Move_Z_scale, Z_MIN_POS * MINUNITMULT);
			NOMORE(HMI_Value.Move_Z_scale, Z_MAX_POS * MINUNITMULT);
			current_position.z = (float)HMI_Value.Move_Z_scale / MINUNITMULT;
			DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_MOVEZ), HMI_Value.Move_Z_scale);
		}
		dwinLCD.UpdateLCD();		
	}
}


#if HAS_HOTEND
void HMI_Move_Extruder(uint8_t extr) {	
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Current_E_Scale[extr])) {			
			if(!planner.is_full()) {
				char gcode_string[50] = {0};
				planner.synchronize();
				ZERO(gcode_string);
				float temp_E_Coordinate = (float)HMI_Value.Current_E_Scale[extr]/MINUNITMULT;
			#if ENABLED(MIXING_EXTRUDER)
				const uint8_t old_tool = mixer.selected_vtool;
				MIXER_STEPPER_LOOP(i) mixer.set_collector(i, (extr == i?1.0:0.0));
				mixer.normalize(MIXER_DIRECT_SET_TOOL);
				mixer.T(MIXER_DIRECT_SET_TOOL);
				//DEBUG_DWIN_Show_Mix_parameters(1);
				if(temp_E_Coordinate < HMI_Value.Last_E_Coordinate[extr])
					sprintf_P(gcode_string, PSTR("G92 E0\nG1 E-%.1f F100\nG92 E0\nT%d"), ABS(temp_E_Coordinate - HMI_Value.Last_E_Coordinate[extr]), old_tool);
				else
					sprintf_P(gcode_string, PSTR("G92 E0\nG1 E%.1f F100\nG92 E0\nT%d"), ABS(temp_E_Coordinate - HMI_Value.Last_E_Coordinate[extr]), old_tool);				
			#else
				const uint8_t old_tool = active_extruder;
				if(temp_E_Coordinate < HMI_Value.Last_E_Coordinate[extr])
					sprintf_P(gcode_string, PSTR("T%1d\nG92 E0\nG1 E-%.1f F100\nG92 E0\nT%d"), extr, ABS(temp_E_Coordinate - HMI_Value.Last_E_Coordinate[extr]), old_tool);
				else
					sprintf_P(gcode_string, PSTR("T%1d\nG92 E0\nG1 E%.1f F100\nG92 E0\nT%d"), extr, ABS(temp_E_Coordinate - HMI_Value.Last_E_Coordinate[extr]), old_tool);				
			#endif				
				HMI_Value.Last_E_Coordinate[extr] = temp_E_Coordinate;
				queue.inject(gcode_string);
				
				DwinMenuID = DWMENU_MOVEAXIS;
				EncoderRate.enabled = false;
				last_Extr_scale[extr] = HMI_Value.Current_E_Scale[extr];
				DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_EX1 + extr), HMI_Value.Current_E_Scale[extr]);
			}		
		}
		else {
			if ((HMI_Value.Current_E_Scale[extr] - last_Extr_scale[extr]) > (EXTRUDE_MAXLENGTH) * MINUNITMULT)
				HMI_Value.Current_E_Scale[extr] = last_Extr_scale[extr] + (EXTRUDE_MAXLENGTH) * MINUNITMULT;
			else if ((last_Extr_scale[extr] - HMI_Value.Current_E_Scale[extr]) > (EXTRUDE_MAXLENGTH) * MINUNITMULT)
				HMI_Value.Current_E_Scale[extr] = last_Extr_scale[extr] - (EXTRUDE_MAXLENGTH) * MINUNITMULT;
			DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_EX1 + extr), HMI_Value.Current_E_Scale[extr]);
		}
		dwinLCD.UpdateLCD();
	}
}

#if ENABLED(MIXING_EXTRUDER)
void HMI_Move_AllExtr() {
	static float last_EALL_scale = 0;
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
	 	if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Current_EAll_Scale)) {	  	
	  	if(!planner.is_full()) {
		    planner.synchronize(); // Wait for planner moves to finish!
		    char gcode_string[50] = {0};
				ZERO(gcode_string);
				float temp_E_Coordinate = (float)HMI_Value.Current_EAll_Scale/MINUNITMULT;
				const uint8_t old_tool = mixer.selected_vtool;
				MIXER_STEPPER_LOOP(i) mixer.set_collector(i, 1.0);
				mixer.normalize(MIXER_DIRECT_SET_TOOL);
				mixer.T(MIXER_DIRECT_SET_TOOL);
				//DEBUG_DWIN_Show_Mix_parameters(1);
				if(temp_E_Coordinate < HMI_Value.Last_EAll_Coordinate)
			   	sprintf_P(gcode_string, PSTR("G92 E0\nG1 E-%.2f F120\nG92 E0\nT%d"), ABS(temp_E_Coordinate - HMI_Value.Last_EAll_Coordinate) * MIXING_STEPPERS, old_tool);
				else
				 	sprintf_P(gcode_string, PSTR("G92 E0\nG1 E%.2f F120\nG92 E0\nT%d"), ABS(temp_E_Coordinate - HMI_Value.Last_EAll_Coordinate) * MIXING_STEPPERS, old_tool);
				HMI_Value.Last_EAll_Coordinate = temp_E_Coordinate;
				queue.inject(gcode_string);

				DwinMenuID = DWMENU_MOVEAXIS;
	  		EncoderRate.enabled = false;
	  		last_EALL_scale = HMI_Value.Current_EAll_Scale;
				DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_EXALL), HMI_Value.Current_EAll_Scale);
	  	}	  	
	 	}
		else {
		 	if ((HMI_Value.Current_EAll_Scale - last_EALL_scale) > (EXTRUDE_MAXLENGTH) * MINUNITMULT)
		  	HMI_Value.Current_EAll_Scale = last_EALL_scale + (EXTRUDE_MAXLENGTH) * MINUNITMULT;
		 	else if ((last_EALL_scale - HMI_Value.Current_EAll_Scale) > (EXTRUDE_MAXLENGTH) * MINUNITMULT)
		  	HMI_Value.Current_EAll_Scale = last_EALL_scale - (EXTRUDE_MAXLENGTH) * MINUNITMULT;
			
		 	DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_EXALL), HMI_Value.Current_EAll_Scale);
		}
	 	dwinLCD.UpdateLCD();
	}
}
#endif//MIXING_EXTRUDER
#endif//HAS_HOTEND

void HMI_MoveAxis() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;
 	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_move.inc(AXISMOVE_CASE_END)) {
			if (DwinMenu_move.now > MROWS && DwinMenu_move.now > DwinMenu_move.index) {
				DwinMenu_move.index = DwinMenu_move.now;
				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);

		#if HAS_HOTEND
			if(DwinMenu_move.index == AXISMOVE_CASE_EX1)	Item_Axis_MoveEX1(MROWS);	
			#if(E_STEPPERS > 1)
				else if(DwinMenu_move.index == AXISMOVE_CASE_EX2)	Item_Axis_MoveEX2(MROWS);
			#endif			
			#if (E_STEPPERS > 2)
				else if(DwinMenu_move.index == AXISMOVE_CASE_EX3)	Item_Axis_MoveEX3(MROWS);
			#endif
			#if (E_STEPPERS > 3)
				else if(DwinMenu_move.index == AXISMOVE_CASE_EX4) Item_Axis_MoveEX4(MROWS);
			#endif
			#if ENABLED(MIXING_EXTRUDER)
				else if(DwinMenu_move.index == AXISMOVE_CASE_EXALL) Item_Axis_MoveEXAll(MROWS);
			#endif
		#endif
			}
			else {
				Move_Highlight(1, DwinMenu_move.now + MROWS - DwinMenu_move.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_move.dec()) {
			if (DwinMenu_move.now < DwinMenu_move.index - MROWS) {
				DwinMenu_move.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);

				if(DwinMenu_move.index - MROWS == AXISMOVE_CASE_BACK) Draw_Back_First();				
				else if(DwinMenu_move.index - MROWS == AXISMOVE_CASE_MOVEX) Item_Axis_MoveX(0);
				else if(DwinMenu_move.index - MROWS == AXISMOVE_CASE_MOVEY) Item_Axis_MoveY(0);
				else if(DwinMenu_move.index - MROWS == AXISMOVE_CASE_MOVEZ) Item_Axis_MoveZ(0);
			#if HAS_HOTEND
				else if(DwinMenu_move.index - MROWS == AXISMOVE_CASE_EX1) Item_Axis_MoveEX1(0);
				#if(E_STEPPERS > 1)
				else if(DwinMenu_move.index - MROWS == AXISMOVE_CASE_EX2) Item_Axis_MoveEX2(0);
				#endif
				#if(E_STEPPERS > 2)
				else if(DwinMenu_move.index - MROWS == AXISMOVE_CASE_EX3) Item_Axis_MoveEX3(0);
				#endif
				#if(E_STEPPERS > 3)
				else if(DwinMenu_move.index - MROWS == AXISMOVE_CASE_EX4) Item_Axis_MoveEX4(0);
				#endif
			#endif
			}
			else {
				Move_Highlight(-1, DwinMenu_move.now + MROWS - DwinMenu_move.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_move.now) {
			case 0: // Back
				Draw_Prepare_Menu(PREPARE_CASE_MOVE);
			break;
			
			case AXISMOVE_CASE_MOVEX: // X axis move
				DwinMenuID = DWMENU_MOVEX;
				HMI_Value.Move_X_scale = current_position.x * MINUNITMULT;
				DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_MOVEX), HMI_Value.Move_X_scale);
				EncoderRate.enabled = true;
			break;
				
			case AXISMOVE_CASE_MOVEY: // Y axis move
				DwinMenuID = DWMENU_MOVEY;
				HMI_Value.Move_Y_scale = current_position.y * MINUNITMULT;
				DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_MOVEY), HMI_Value.Move_Y_scale);
				EncoderRate.enabled = true;
			break;
			
			case AXISMOVE_CASE_MOVEZ: // Z axis move
				DwinMenuID = DWMENU_MOVEZ;
				HMI_Value.Move_Z_scale = current_position.z * MINUNITMULT;
				DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_MOVEZ), HMI_Value.Move_Z_scale);
				EncoderRate.enabled = true;
			break;

	#if HAS_HOTEND
			case AXISMOVE_CASE_EX1: // Extruder1
				// window tips
			#if ENABLED(PREVENT_COLD_EXTRUSION)
				if (thermalManager.degHotend(0) < EXTRUDE_MINTEMP) {
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Nozzle is too cool!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
			#endif		
				DwinMenuID = DWMENU_MOVE_EXT1;
				HMI_Value.Current_E_Scale[active_extruder] = HMI_Value.Last_E_Coordinate[0]*MINUNITMULT;
				DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + DwinMenu_move.now), HMI_Value.Current_E_Scale[0]);
				EncoderRate.enabled = true;
			break;

		#if(E_STEPPERS > 1)
			case AXISMOVE_CASE_EX2: // Extruder2
			// window tips
			#if ENABLED(PREVENT_COLD_EXTRUSION)
				if (thermalManager.degHotend(0) < EXTRUDE_MINTEMP) {
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Nozzle is too cool!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
			#endif
				DwinMenuID = DWMENU_MOVE_EXT2;
				HMI_Value.Current_E_Scale[1] = HMI_Value.Last_E_Coordinate[1]*MINUNITMULT;
				DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_EX2), HMI_Value.Current_E_Scale[1]);
				EncoderRate.enabled = true;
			break;
		#endif//(E_STEPPERS > 1)

		#if(E_STEPPERS > 2)
			case AXISMOVE_CASE_EX3: // Extruder3
			// window tips
			#if ENABLED(PREVENT_COLD_EXTRUSION)
				if (thermalManager.degHotend(0) < EXTRUDE_MINTEMP) {
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Nozzle is too cool!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
			#endif
				DwinMenuID = DWMENU_MOVE_EXT3;
				HMI_Value.Current_E_Scale[2] = HMI_Value.Last_E_Coordinate[2]*MINUNITMULT;
				DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + AXISMOVE_CASE_EX3), HMI_Value.Current_E_Scale[2]);
				EncoderRate.enabled = true;
			break;
		#endif//(E_STEPPERS > 2)

		#if(E_STEPPERS > 3)
			case AXISMOVE_CASE_EX4: // Extruder4
			// window tips
			#if ENABLED(PREVENT_COLD_EXTRUSION)
				if (thermalManager.degHotend(0) < EXTRUDE_MINTEMP) {
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Nozzle is too cool!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
			#endif
				DwinMenuID = DWMENU_MOVE_EXT4;
				HMI_Value.Current_E_Scale[3] = HMI_Value.Last_E_Coordinate[3]*MINUNITMULT;
				DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_move.index + DwinMenu_move.now), HMI_Value.Current_E_Scale[3]);
				EncoderRate.enabled = true;
			break;
		#endif//(E_STEPPERS > 3)

		#if ENABLED(MIXING_EXTRUDER)
			case AXISMOVE_CASE_EXALL: // Extruderall
			// window tips
			#if ENABLED(PREVENT_COLD_EXTRUSION)
				if (thermalManager.degHotend(0) < EXTRUDE_MINTEMP) {
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Nozzle is too cool!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
			#endif
				DwinMenuID = DWMENU_MOVE_EXTALL;
				HMI_Value.Current_EAll_Scale = HMI_Value.Last_EAll_Coordinate*MINUNITMULT;
				DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS - DwinMenu_move.index + DwinMenu_move.now), HMI_Value.Current_EAll_Scale);
				EncoderRate.enabled = true;
			break;
		#endif //end if enable(MIXING_EXTRUDER)
	#endif //end HAS_HOTEND
		}
	}
	dwinLCD.UpdateLCD();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Prepare >> Filament
//
extern fil_change_settings_t fc_settings[EXTRUDERS];
static void Item_Filament_Preheat(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_PREHEAT, LBLX, MBASE(row));	
	DWIN_Show_MultiLanguage_String(MTSTRING_NOZZLE, LBLX+get_MultiLanguageString_Width(MTSTRING_PREHEAT)+5, MBASE(row));
	if(HMI_Value.E_Temp > HOTEND_WARNNING_TEMP)
		DWIN_Draw_Warn_IntValue_Default(3, MENUVALUE_X+8, MBASE(TEMP_CASE_ETEMP + MROWS - DwinMenu_temp.index), HMI_Value.nozzle_Temp);
	else
		DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(TEMP_CASE_ETEMP + MROWS - DwinMenu_temp.index), HMI_Value.nozzle_Temp);
	Draw_Menu_Line(row, ICON_CURSOR);
}

static void Item_Filament_Extruder(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_EXTRUDER, LBLX, MBASE(row));
	if(HMI_Value.load_extruder <= E_STEPPERS)
		DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), HMI_Value.load_extruder);
	else
		DWIN_Draw_MaskString_Default(MENUVALUE_X+8, MBASE(row), PSTR("All"));
	Draw_Menu_Line(row,ICON_CURSOR);	
}

static void Item_Filament_Purgelength(const uint8_t row) {	
#if (DWINLCD_MENU_VERSION >= 2)
	DWIN_Show_MultiLanguage_String(MTSTRING_SLOWLY, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_LENGTH, LBLX+get_MultiLanguageString_Width(MTSTRING_SLOWLY)+5, MBASE(row));
#else
	DWIN_Draw_UnMaskString_Default(LBLX, MBASE(row), PSTR("Slowly Length(mm)"));
#endif
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), HMI_Value.purgelength);
	Draw_Menu_Line(row,ICON_CURSOR);	
}

static void Item_Filament_Feedlength(const uint8_t row) {
#if (DWINLCD_MENU_VERSION >= 2)
	DWIN_Show_MultiLanguage_String(MTSTRING_QUICKLY, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_LENGTH, LBLX+get_MultiLanguageString_Width(MTSTRING_QUICKLY)+5, MBASE(row));
#else
	DWIN_Draw_UnMaskString_Default(LBLX, MBASE(row), PSTR("Quckly Length(mm)"));
#endif
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), HMI_Value.feedlength);
	Draw_Menu_Line(row,ICON_CURSOR);	
}

static void Item_Filament_Pugre(const uint8_t row) {
#if (DWINLCD_MENU_VERSION >= 2)
	DWIN_Show_MultiLanguage_String(MTSTRING_SLOWLY, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_FILAMENT_LOAD, LBLX+get_MultiLanguageString_Width(MTSTRING_SLOWLY)+5, MBASE(row));
#else	
	DWIN_Draw_UnMaskString_Default(LBLX, MBASE(row), PSTR("Slowly Load"));	
#endif
	Draw_Menu_Line(row,ICON_CURSOR);
	//Draw_More_Icon(row);
}

static void Item_Filament_Retract(const uint8_t row) {
#if (DWINLCD_MENU_VERSION >= 2)
	DWIN_Show_MultiLanguage_String(MTSTRING_SLOWLY, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_FILAMENT_UNLOAD, LBLX+get_MultiLanguageString_Width(MTSTRING_SLOWLY)+5, MBASE(row));
#else
	DWIN_Draw_UnMaskString_Default(LBLX, MBASE(row), PSTR("Slowly Unload"));	
#endif
	Draw_Menu_Line(row,ICON_CURSOR);
	//Draw_More_Icon(row);
}


inline void Item_Filament_load(const uint8_t row) {
#if (DWINLCD_MENU_VERSION >= 2)
	DWIN_Show_MultiLanguage_String(MTSTRING_QUICKLY, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_FILAMENT_LOAD, LBLX+get_MultiLanguageString_Width(MTSTRING_QUICKLY)+5, MBASE(row));
#else	
	DWIN_Draw_UnMaskString_Default(LBLX, MBASE(row), PSTR("Quickly Load"));
#endif
	Draw_Menu_Line(row,ICON_CURSOR);
	//Draw_More_Icon(row);
}

inline void Item_Filament_unload(const uint8_t row) {
#if (DWINLCD_MENU_VERSION >= 2)
	DWIN_Show_MultiLanguage_String(MTSTRING_QUICKLY, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_FILAMENT_UNLOAD, LBLX+get_MultiLanguageString_Width(MTSTRING_QUICKLY)+5, MBASE(row));
#else		
	DWIN_Draw_UnMaskString_Default(LBLX, MBASE(row), PSTR("Quickly Unload"));	
#endif
	Draw_Menu_Line(row,ICON_CURSOR);
	//Draw_More_Icon(row);
}


void Draw_Filament_Menu() {
	DwinMenuID = DWMENU_FILAMENT;
	DwinMenu_filament.reset();
	//DwinMenu_filament.index = _MAX(DwinMenu_filament.now, MROWS);

#if (FILAMENT_CASE_TOTAL > MROWS)
	const int8_t scrol = MROWS - DwinMenu_filament.index;
	#define FSCROL(L) (scrol + (L))
#else
	#define FSCROL(L) (L)
#endif
	#define FVISI(L) WITHIN(FSCROL(L), 0, MROWS)
		
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	HMI_flag.Is_purged = false;
	HMI_flag.Is_retracted = false;
	HMI_Value.load_extruder = 1;

#if (DWINLCD_MENU_VERSION >= 2)
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_FILAMENT, TITLE_X, TITLE_Y);
#else
	DWIN_Draw_UnMaskString_FONT10(TITLE_X, TITLE_Y, PSTR("Filament"));
#endif
	if(FVISI(FILAMENT_CASE_BACK)) Draw_Back_First(DwinMenu_filament.now == FILAMENT_CASE_BACK);

	if(FVISI(FILAMENT_CASE_PREHEAT)) Item_Filament_Preheat(FSCROL(FILAMENT_CASE_PREHEAT));
	if(FVISI(FILAMENT_CASE_EXTRUDER)) Item_Filament_Extruder(FSCROL(FILAMENT_CASE_EXTRUDER));
	if(FVISI(FILAMENT_CASE_PURGELENGTH)) Item_Filament_Purgelength(FSCROL(FILAMENT_CASE_PURGELENGTH));
	if(FVISI(FILAMENT_CASE_FEEDLENGTH)) Item_Filament_Feedlength(FSCROL(FILAMENT_CASE_FEEDLENGTH));	
	if(FVISI(FILAMENT_CASE_PURGE)) Item_Filament_Pugre(FSCROL(FILAMENT_CASE_PURGE));
	if(FVISI(FILAMENT_CASE_RETRACT)) Item_Filament_Retract(FSCROL(FILAMENT_CASE_RETRACT));
	if(FVISI(FILAMENT_CASE_LOAD)) Item_Filament_load(FSCROL(FILAMENT_CASE_LOAD));
	if(FVISI(FILAMENT_CASE_UNLOAD)) Item_Filament_unload(FSCROL(FILAMENT_CASE_UNLOAD));
	
	if (DwinMenu_filament.now) Draw_Menu_Cursor(FSCROL(DwinMenu_filament.now));
}

void HMI_Filament_PretHeat() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.nozzle_Temp)) {
			DwinMenuID = DWMENU_FILAMENT;
			if(HMI_Value.nozzle_Temp > HOTEND_WARNNING_TEMP)
				DWIN_Draw_Warn_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_PREHEAT + MROWS - DwinMenu_filament.index), HMI_Value.nozzle_Temp);				
			else
				DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_PREHEAT + MROWS - DwinMenu_filament.index), HMI_Value.nozzle_Temp);
			thermalManager.setTargetHotend(HMI_Value.nozzle_Temp, 0);
		}
		else {
			NOLESS(HMI_Value.nozzle_Temp, EXTRUDE_MINTEMP);
			NOMORE(HMI_Value.nozzle_Temp, (HEATER_0_MAXTEMP - HOTEND_OVERSHOOT));
			if(HMI_Value.nozzle_Temp > HOTEND_WARNNING_TEMP)
				DWIN_Draw_Warn_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_EXTRUDER + MROWS - DwinMenu_filament.index), HMI_Value.nozzle_Temp);				
			else
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_PREHEAT + MROWS - DwinMenu_filament.index), HMI_Value.nozzle_Temp);
		}
		dwinLCD.UpdateLCD();
	}	
}


void HMI_Filament_Extuder() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_uint8(encoder_diffState, &HMI_Value.load_extruder)) {
			DwinMenuID = DWMENU_FILAMENT;
			if(HMI_Value.load_extruder <= E_STEPPERS)
				DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_EXTRUDER + MROWS - DwinMenu_filament.index), HMI_Value.load_extruder);
			else
				DWIN_Draw_MaskString_Default(MENUVALUE_X+8, MBASE(FILAMENT_CASE_EXTRUDER + MROWS - DwinMenu_filament.index), PSTR("All"));
		}
		else {
			NOLESS(HMI_Value.load_extruder, 1);		
		#if 0//BOTH(MIXING_EXTRUDER, OPTION_MIXING_SWITCH)
		//don't allow to use all extruder on no-mixing hotend
			if(!mixer.mixing_enabled){
				NOMORE(HMI_Value.load_extruder, E_STEPPERS);
			}
			else
		#endif
			{
				NOMORE(HMI_Value.load_extruder, (E_STEPPERS+1));
			}
			if(HMI_Value.load_extruder <= E_STEPPERS)
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_EXTRUDER + MROWS - DwinMenu_filament.index), HMI_Value.load_extruder);
			else
				DWIN_Draw_MaskString_Default_Color(SELECT_COLOR, MENUVALUE_X+8, MBASE(FILAMENT_CASE_EXTRUDER + MROWS - DwinMenu_filament.index), PSTR("All"));			
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Filament_FeedLength() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		int16_t temp_feedlength = (HMI_Value.feedlength/10);
		if (Apply_Encoder_int16(encoder_diffState, &temp_feedlength)) {			
			DwinMenuID = DWMENU_FILAMENT;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_FEEDLENGTH + MROWS - DwinMenu_filament.index), HMI_Value.feedlength);
		}
		else {
			HMI_Value.feedlength = temp_feedlength*10;
			NOLESS(HMI_Value.feedlength, 10);
			NOMORE(HMI_Value.feedlength, 999);
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_FEEDLENGTH + MROWS - DwinMenu_filament.index), HMI_Value.feedlength);
		}
		dwinLCD.UpdateLCD();
	}
}


void HMI_Filament_PurgeLength() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.purgelength)) {
			DwinMenuID = DWMENU_FILAMENT;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_PURGELENGTH + MROWS - DwinMenu_filament.index), HMI_Value.purgelength);			
		}
		else {
			NOLESS(HMI_Value.purgelength, 1);
			NOMORE(HMI_Value.purgelength, 800);
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_PURGELENGTH + MROWS - DwinMenu_filament.index), HMI_Value.purgelength);
		}
		dwinLCD.UpdateLCD();
	}
}

static void Dwin_filament_action(uint8_t action){
	const feedRate_t old_feedrate = feedrate_mm_s;
	static uint8_t mux = 1; 
	static uint16_t t = 2;
	char statusbar_str[40]={0};
	
#if ENABLED(MIXING_EXTRUDER)
	const uint8_t extruder = HMI_Value.load_extruder-1;
	const uint8_t old_vtool = mixer.selected_vtool;
#else
	const uint8_t old_extruder = active_extruder;
#endif

#if ENABLED(PREVENT_COLD_EXTRUSION)
	if(thermalManager.degHotend(0) < EXTRUDE_MINTEMP)	{ 
		DWIN_Show_Status_Message(COLOR_RED, PSTR("Nozzle is too cool!"));
		DWIN_FEEDBACK_WARNNING();
		return;
	}
	else 
#endif
	if(HMI_flag.clean_status_delay > 0) { 
		DWIN_FEEDBACK_WARNNING();	
		return;
	}
#if ENABLED(MIXING_EXTRUDER)
	mixer.T(MIXER_DIRECT_SET_TOOL);
	if(extruder < E_STEPPERS){
		MIXER_STEPPER_LOOP(i) mixer.set_collector(i, ((i == extruder) ? 1.0 : 0.0));	
		mux = 1;
	}
	else{	
		MIXER_STEPPER_LOOP(i) mixer.set_collector(i, 1.0);
		mux = E_STEPPERS;
	}
	mixer.normalize();
#endif
	if(action == FILAMENT_CASE_PURGE){
		//purge	== slolyw load
		current_position.e += HMI_Value.purgelength*mux;
		feedrate_mm_s = FILAMENT_CHANGE_SLOW_LOAD_FEEDRATE;
		t = HMI_Value.purgelength*mux/FILAMENT_CHANGE_SLOW_LOAD_FEEDRATE;		
		if(t < 60)
			sprintf_P(statusbar_str, PSTR("Loading, please wait %2d seconds..."), t);
		else
			sprintf_P(statusbar_str, PSTR("Loading, please wait %1dm%2ds ..."), t/60, t%60);
					
		DWIN_Show_Status_Message(COLOR_WHITE, statusbar_str, t<2?2:t);
		planner.buffer_line(current_position, feedrate_mm_s, active_extruder);
		planner.synchronize();
		HMI_flag.Is_purged = true;
		HMI_flag.Is_retracted = false;
	}
	else if(action == FILAMENT_CASE_RETRACT){		
		//unload == slolyw unload
		current_position.e -= HMI_Value.purgelength*mux;
		feedrate_mm_s = FILAMENT_CHANGE_SLOW_LOAD_FEEDRATE*mux;
		t = HMI_Value.purgelength/FILAMENT_CHANGE_SLOW_LOAD_FEEDRATE;
		if(t < 60)
			sprintf_P(statusbar_str, PSTR("Unloading, please wait %2d seconds..."), t);
		else
			sprintf_P(statusbar_str, PSTR("Unloading, please wait %1dm%2ds ..."), t/60, t%60);
		DWIN_Show_Status_Message(COLOR_WHITE, statusbar_str, t<2?2:t);
		set_status_msg_showtime(t<2?2:t);
		planner.buffer_line(current_position, feedrate_mm_s, active_extruder);
		planner.synchronize();
		HMI_flag.Is_retracted = true;
	} 
	else if(action == FILAMENT_CASE_LOAD){		
		//load == quickly load
		current_position.e += HMI_Value.feedlength*mux;
		feedrate_mm_s = FILAMENT_CHANGE_FAST_LOAD_FEEDRATE*mux;
		t = HMI_Value.feedlength/FILAMENT_CHANGE_FAST_LOAD_FEEDRATE;
		if(t < 60)
			sprintf_P(statusbar_str, PSTR("Loading, please wait %2d seconds..."), t);
		else
			sprintf_P(statusbar_str, PSTR("Loading, please wait %1dm%2ds ..."), t/60, t%60);
		DWIN_Show_Status_Message(COLOR_WHITE, statusbar_str, t<2?2:t);
		planner.buffer_line(current_position, feedrate_mm_s, active_extruder);
		planner.synchronize();
		HMI_flag.Is_retracted = false;
	}
	else if(action == FILAMENT_CASE_UNLOAD){
		//quickly unload		
		//purge
		if(!HMI_flag.Is_purged){
			current_position.e += FILAMENT_UNLOAD_PURGE_LENGTH*mux;
			feedrate_mm_s = FILAMENT_CHANGE_SLOW_LOAD_FEEDRATE;
			t = FILAMENT_UNLOAD_PURGE_LENGTH*mux/FILAMENT_CHANGE_SLOW_LOAD_FEEDRATE;			
			DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Purge a little first..."), t<2?2:t);
			planner.buffer_line(current_position, feedrate_mm_s, active_extruder);		
			planner.synchronize();
		}
		//retract
		if(!HMI_flag.Is_retracted){
			current_position.e -= PAUSE_PARK_RETRACT_LENGTH*mux;
			feedrate_mm_s = PAUSE_PARK_RETRACT_FEEDRATE*mux;
			DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Fast retracting..."));		
			planner.buffer_line(current_position, feedrate_mm_s, active_extruder);
			planner.synchronize();
			HMI_flag.Is_retracted = true;
		}
		//unload
		current_position.e -= HMI_Value.feedlength*mux;
		feedrate_mm_s = FILAMENT_CHANGE_UNLOAD_FEEDRATE*mux;		
		t = HMI_Value.feedlength/FILAMENT_CHANGE_UNLOAD_FEEDRATE;
		if(t < 60)
			sprintf_P(statusbar_str, PSTR("Unloading, please wait %2d seconds..."), t);
		else
			sprintf_P(statusbar_str, PSTR("Unloading, please wait %1dm%2ds ..."), t/60, t%60);
		DWIN_Show_Status_Message(COLOR_WHITE, statusbar_str, t<2?2:t);
		planner.buffer_line(current_position, feedrate_mm_s, active_extruder);
		planner.synchronize();		
	}
#if ENABLED(MIXING_EXTRUDER)
	mixer.T(old_vtool);
#else
	active_extruder = old_extruder;
#endif
	feedrate_mm_s = old_feedrate;	
	disable_e_steppers();
}

void HMI_Filament() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;
  // Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_filament.inc(FILAMENT_CASE_END)) {
			if (DwinMenu_filament.now > MROWS && DwinMenu_filament.now > DwinMenu_filament.index) {
				DwinMenu_filament.index = DwinMenu_filament.now;
				Scroll_Menu(DWIN_SCROLL_UP);
				if(DwinMenu_filament.index == FILAMENT_CASE_RETRACT) Item_Filament_Retract(MROWS);
				else if(DwinMenu_filament.index == FILAMENT_CASE_LOAD) Item_Filament_load(MROWS);
				else if(DwinMenu_filament.index == FILAMENT_CASE_UNLOAD) Item_Filament_unload(MROWS);			
			}
			else 
				Move_Highlight(1, DwinMenu_filament.now + MROWS - DwinMenu_filament.index);
		}
	}	
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if(DwinMenu_filament.dec()) {
			if(DwinMenu_filament.now < DwinMenu_filament.index - MROWS) {
				DwinMenu_filament.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if (DwinMenu_filament.index - MROWS == FILAMENT_CASE_BACK) Draw_Back_First();					
				else if(DwinMenu_filament.index - MROWS == FILAMENT_CASE_PREHEAT) Item_Filament_Preheat(0);
				else if(DwinMenu_filament.index - MROWS == FILAMENT_CASE_EXTRUDER) Item_Filament_Extruder(0);				
				else if(DwinMenu_filament.index - MROWS == FILAMENT_CASE_PURGELENGTH) Item_Filament_Purgelength(0);
			}
			else 
				Move_Highlight(-1, DwinMenu_filament.now + MROWS - DwinMenu_filament.index);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_filament.now) {
			case 0: // Back
				Draw_Prepare_Menu(PREPARE_CASE_FILAMENT);
			break;

			case FILAMENT_CASE_PREHEAT:
				DwinMenuID = DWMENU_FILAMENT_PREHEAT;
				if(HMI_Value.nozzle_Temp > HOTEND_WARNNING_TEMP)
					DWIN_Draw_Warn_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_EXTRUDER + MROWS - DwinMenu_filament.index), HMI_Value.nozzle_Temp);
				else
					DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_PREHEAT + MROWS - DwinMenu_filament.index), HMI_Value.nozzle_Temp);					
				EncoderRate.enabled = true;
			break;
			
			case FILAMENT_CASE_EXTRUDER:
				DwinMenuID = DWMENU_FILAMENT_EXTRUDER;
				if(HMI_Value.load_extruder <= E_STEPPERS)
					DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_EXTRUDER + MROWS - DwinMenu_filament.index), HMI_Value.load_extruder);				
				else
					DWIN_Draw_MaskString_Default_Color(SELECT_COLOR, MENUVALUE_X+8, MBASE(FILAMENT_CASE_EXTRUDER + MROWS - DwinMenu_filament.index), PSTR("All"));				
				EncoderRate.enabled = false;
			break;					
		
			case FILAMENT_CASE_PURGELENGTH:
				DwinMenuID = DWMENU_FILAMENT_PURGELENGTH;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_PURGELENGTH + MROWS - DwinMenu_filament.index), HMI_Value.purgelength);
				EncoderRate.enabled = true;
			break;

			case FILAMENT_CASE_FEEDLENGTH:
				DwinMenuID = DWMENU_FILAMENT_FEEDLENGTH;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(FILAMENT_CASE_FEEDLENGTH + MROWS - DwinMenu_filament.index), HMI_Value.feedlength);
				EncoderRate.enabled = true;
			break;

			case FILAMENT_CASE_PURGE:
			case FILAMENT_CASE_RETRACT:
			case FILAMENT_CASE_LOAD:
			case FILAMENT_CASE_UNLOAD:
				Dwin_filament_action(DwinMenu_filament.now);		
			break;
		}
	}
	dwinLCD.UpdateLCD();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Prepare >> Bed Leveling
//
#if (HAS_LEVELING && GRID_MAX_POINTS_X <= 7)
#define	TABLE_BOTTOM	330
#define	TABLE_LEFT		14
#define	TABLE_RIGHT		258
#define	TABLE_WIDTH		244
#if (GRID_MAX_POINTS_X == 3)
#define	FONT_G29TABLE	font10x20
#define	TABLE_TOP			176
#define	TABLE_HEIGTH	(TABLE_BOTTOM-TABLE_TOP)
#define	BAR_WIDTH 		72
#define	BAR_GAP_X 		8
#define	STR_START_X		(BAR_WIDTH-5*10)/2
#define	ADJUST_X			((TABLE_WIDTH-BAR_WIDTH*GRID_MAX_POINTS_X-BAR_GAP_X*(GRID_MAX_POINTS_X-1))/2)
#define	BAR_HEIGTH		32
#define	BAR_GAP_Y			12	
#define	STR_START_Y		(BAR_HEIGTH-20)/2
#define	ADJUST_Y			((TABLE_HEIGTH - BAR_HEIGTH*GRID_MAX_POINTS_Y - BAR_GAP_Y*(GRID_MAX_POINTS_Y-1))/2)
#elif (GRID_MAX_POINTS_X == 4)
#define	FONT_G29TABLE	font10x20
#define	TABLE_TOP			176
#define	TABLE_HEIGTH	(TABLE_BOTTOM-TABLE_TOP)
#define	BAR_WIDTH 		56
#define	BAR_GAP_X 		6
#define	STR_START_X		(BAR_WIDTH-5*10)/2
#define	ADJUST_X			((TABLE_WIDTH-BAR_WIDTH*GRID_MAX_POINTS_X-BAR_GAP_X*(GRID_MAX_POINTS_X-1))/2)
#define	BAR_HEIGTH		30
#define	BAR_GAP_Y			8
#define	STR_START_Y		(BAR_HEIGTH-20)/2
#define	ADJUST_Y			((TABLE_HEIGTH - BAR_HEIGTH*GRID_MAX_POINTS_Y - BAR_GAP_Y*(GRID_MAX_POINTS_Y-1))/2)
#elif (GRID_MAX_POINTS_X == 5)
#define	FONT_G29TABLE	font8x16
#define	TABLE_TOP			150
#define	TABLE_HEIGTH	(TABLE_BOTTOM-TABLE_TOP)
#define	BAR_WIDTH 		46
#define	BAR_GAP_X 		3
#define	STR_START_X		(BAR_WIDTH-5*8)/2
#define	ADJUST_X			((TABLE_WIDTH-BAR_WIDTH*GRID_MAX_POINTS_X-BAR_GAP_X*(GRID_MAX_POINTS_X-1))/2)
#define	BAR_HEIGTH		28
#define	BAR_GAP_Y			8
#define	STR_START_Y		(BAR_HEIGTH-16)/2
#define	ADJUST_Y			((TABLE_HEIGTH - BAR_HEIGTH*GRID_MAX_POINTS_Y - BAR_GAP_Y*(GRID_MAX_POINTS_Y-1))/2)
#elif (GRID_MAX_POINTS_X == 6)
#define	FONT_G29TABLE	font6x12
#define	TABLE_TOP			150
#define	TABLE_HEIGTH	(TABLE_BOTTOM-TABLE_TOP)
#define	BAR_WIDTH 		36
#define	BAR_GAP_X 		5
#define	STR_START_X		(BAR_WIDTH-5*6)/2
#define	ADJUST_X			((TABLE_WIDTH-BAR_WIDTH*GRID_MAX_POINTS_X-BAR_GAP_X*(GRID_MAX_POINTS_X-1))/2)
#define	BAR_HEIGTH		24
#define	BAR_GAP_Y			6
#define	STR_START_Y		(BAR_HEIGTH-12)/2
#define	ADJUST_Y			((TABLE_HEIGTH - BAR_HEIGTH*GRID_MAX_POINTS_Y - BAR_GAP_Y*(GRID_MAX_POINTS_Y-1))/2)
#elif (GRID_MAX_POINTS_X == 7)
#define	FONT_G29TABLE	font6x12
#define	TABLE_TOP			150
#define	TABLE_HEIGTH	(TABLE_BOTTOM-TABLE_TOP)
#define	BAR_WIDTH 		32
#define	BAR_GAP_X 		3
#define	STR_START_X		(BAR_WIDTH-5*6)/2
#define	ADJUST_X			((TABLE_WIDTH-BAR_WIDTH*GRID_MAX_POINTS_X-BAR_GAP_X*(GRID_MAX_POINTS_X-1))/2)
#define	BAR_HEIGTH		20
#define	BAR_GAP_Y			6
#define	STR_START_Y		(BAR_HEIGTH-12)/2
#define	ADJUST_Y			((TABLE_HEIGTH - BAR_HEIGTH*GRID_MAX_POINTS_Y - BAR_GAP_Y*(GRID_MAX_POINTS_Y-1))/2)
#endif

static void DWIN_G29_Show_ValueTable(bool bFrame, const uint8_t idx, const int16_t value){	
	//#define	FORCE_START_FROM_TOP
	//#define	FORCE_START_FROM_RIGHT	

	uint16_t	x_start, y_start;	
#if (defined(FORCE_START_FROM_RIGHT) || GRID_MAX_POINTS_X == 2 || GRID_MAX_POINTS_X == 4 || GRID_MAX_POINTS_X == 6)
	//2,4,6
	x_start = TABLE_RIGHT - BAR_WIDTH - ADJUST_X;
#else
	//3,5,7
	x_start = TABLE_LEFT + ADJUST_X;		
#endif
#ifdef FORCE_START_FROM_TOP	
	y_start = TABLE_TOP + ADJUST_Y;	
#else
	y_start = TABLE_BOTTOM - BAR_HEIGTH - ADJUST_Y;	
#endif
	LOOP_L_N(i, GRID_MAX_POINTS_Y){		
		LOOP_L_N(j, GRID_MAX_POINTS_X){			
			if(bFrame)
				dwinLCD.Draw_Rectangle(1, COLOR_BG_DEEPBLUE, x_start, y_start, x_start+BAR_WIDTH, y_start+BAR_HEIGTH);			
			else if(idx == i*GRID_MAX_POINTS_Y+j+1){
				dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, x_start+1, y_start+1, x_start+BAR_WIDTH-1, y_start+BAR_HEIGTH-1);			
				if(ABS(value) > 199)
					dwinLCD.Draw_String(false, true, FONT_G29TABLE, COLOR_RED, COLOR_BG_BLACK, x_start+STR_START_X,  y_start+STR_START_Y, PSTR("Error"));
				else if(ABS(value) > 149)
					dwinLCD.Draw_SignedFloatValue(FONT_G29TABLE, COLOR_RED, COLOR_BG_BLACK, 1, 2, x_start+STR_START_X,  y_start+STR_START_Y, value);
				else if(ABS(value) > 100)
					dwinLCD.Draw_SignedFloatValue(FONT_G29TABLE, COLOR_YELLOW, COLOR_BG_BLACK, 1, 2, x_start+STR_START_X,  y_start+STR_START_Y, value);	
				else if(ABS(value) > 50)
					dwinLCD.Draw_SignedFloatValue(FONT_G29TABLE, COLOR_WHITE, COLOR_BG_BLACK, 1, 2, x_start+STR_START_X,  y_start+STR_START_Y, value);
				else
					dwinLCD.Draw_SignedFloatValue(FONT_G29TABLE, COLOR_GREEN, COLOR_BG_BLACK, 1, 2, x_start+STR_START_X,  y_start+STR_START_Y, value);
			}
			if(j<(GRID_MAX_POINTS_X-1)){
			#if(defined(FORCE_START_FROM_RIGHT) || GRID_MAX_POINTS_X == 2 || GRID_MAX_POINTS_X == 4 || GRID_MAX_POINTS_X == 6)
				if(i%2 == 0) 
			#else
				if(i%2 == 1)
			#endif
					x_start -= (BAR_WIDTH+BAR_GAP_X);
				else 
					x_start += (BAR_WIDTH+BAR_GAP_X);			
			}
		}
	#ifdef FORCE_START_FROM_TOP
		y_start += (BAR_HEIGTH+BAR_GAP_Y);
	#else
		y_start -= (BAR_HEIGTH+BAR_GAP_Y);
	#endif
	}	
}

void DWIN_G29_Show_Messge(const _emDWIN_G29_MSG message/*=G29_LEVLE_DEFAULT*/,const int pt_index,const int all_points,const float fvalue){
	char string_buf[60]={0};
#if ENABLED(AUTO_UPDATA_PROBE_Z_OFFSET)
	if(DwinMenuID != DWMENU_LEVEL_CATCHPROBEZOFFSET) DwinMenuID = DWMENU_LEVEL_BEDLEVELING;
#else
	DwinMenuID = DWMENU_LEVEL_BEDLEVELING;
#endif	
	switch(message){
		case G29_CATCH_START:	
			DWIN_Show_Status_Message(COLOR_WHITE, PSTR("To catch offset"), 0);
			break;
		case G29_CATCH_NORMAL:
			sprintf_P(string_buf, PSTR("Catch Point: %d/%d"), pt_index, all_points);
			DWIN_Show_Status_Message(COLOR_WHITE, string_buf, 0);
			break;
		case G29_CATCH_FAIL1:
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Fail! Move down Probe"), 0);
			break;
		case G29_CATCH_FAIL2:
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Over range, manual level!"), 0);
			break;
		case G29_CATCH_DONE:			
			sprintf_P(string_buf, PSTR("Catched! Probe Z offset = %2.2f"), fvalue);
			DWIN_Show_Status_Message(COLOR_GREEN, string_buf, 0);
			break;			
		case G29_MESH_START:
			DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Start Probing!"), 0);
			break;
		case G29_MESH_READY:
			Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
			dwinLCD.JPG_CacheTo1(get_title_picID());
			DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_LEVELING, TITLE_X, TITLE_Y);
			dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
			Draw_Popup_Bkgd_60();
		#if (GRID_MAX_POINTS_X <= 7)
			#if (GRID_MAX_POINTS_X > 4)
			DWIN_Show_ICON(ICON_AUTOLEVELING, 101, 80);
			#else
			DWIN_Show_ICON(ICON_AUTOLEVELING, 101, 105);
			#endif			
			DWIN_G29_Show_ValueTable(true, 0, 0);
		#else
			DWIN_Show_ICON(ICON_AUTOLEVELING, 101, 105);
		#endif
			break;
		case G29_MESH_PROBING:
			sprintf_P(string_buf, PSTR("Point:%d/%d; Z offset= %2.2f"), pt_index, all_points, fvalue);
			DWIN_Show_Status_Message(COLOR_WHITE, string_buf, 0);
			break;
		case G29_MESH_VALUE:
		#if (GRID_MAX_POINTS_X <= 7)
			DWIN_G29_Show_ValueTable(false, pt_index, fvalue*MAXUNITMULT);
		#else			
			sprintf_P(string_buf, PSTR("Z offset= %2.2f"), fvalue);
			DWIN_Show_Status_Message(COLOR_RED, string_buf, 0);
		#endif
			break;
		case G29_MESH_DONE:
			DWIN_Show_Status_Message(COLOR_GREEN, PSTR("Bed Leveling finished!"), 10);						
			dwinLCD.UpdateLCD();
			break;
		default:
			Clear_Dwin_Area(AREA_BOTTOM);
			break;
	}
	dwinLCD.UpdateLCD();
}

void DWIN_PopMenu_LevelingDone() {
	if(IS_SD_PRINTING() || IS_SD_PAUSED())
	{
		Draw_Printing_Menu(PRINT_CASE_PAUSE, true);	
	}
	else
	{
		if(DwinMenuID == DWMENU_LEVEL_BEDLEVELING) {
			DWIN_G29_Show_Messge(G29_MESH_DONE);		
			DwinMenuID = DWMENU_LEVEL_DONECONFIRM;
			DWIN_Show_ICON(ICON_CONFIRM_E, 86, 332);		
		}
		#if ENABLED(AUTO_UPDATA_PROBE_Z_OFFSET)
		else if(DwinMenuID == DWMENU_LEVEL_CATCHPROBEZOFFSET){			
			Draw_Leveling_Menu();	
		}
		#endif
	}
}
#endif//#if (HAS_LEVELING && GRID_MAX_POINTS_X <= 7)

static void Item_Leveling_Home(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_HOME, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_ALL, LBLX+get_MultiLanguageString_Width(MTSTRING_HOME)+10, MBASE(row));
	Draw_Menu_Line(row,ICON_HOME_ALL);	
}

#if ENABLED(LEVEL_BED_CORNERS)	
static void Item_Leveling_Point1(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_POINT, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_1, LBLX+get_MultiLanguageString_Width(MTSTRING_POINT)+6, MBASE(row));	
	Draw_Menu_Line(row, ICON_LEVELING_POINT1);
}

static void Item_Leveling_Point2(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_POINT, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_2, LBLX+get_MultiLanguageString_Width(MTSTRING_POINT)+6, MBASE(row));	
	Draw_Menu_Line(row, ICON_LEVELING_POINT2);
}

static void Item_Leveling_Point3(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_POINT, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_3, LBLX+get_MultiLanguageString_Width(MTSTRING_POINT)+6, MBASE(row));	
	Draw_Menu_Line(row, ICON_LEVELING_POINT3);
}

static void Item_Leveling_Point4(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_POINT, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_4, LBLX+get_MultiLanguageString_Width(MTSTRING_POINT)+6, MBASE(row));	
	Draw_Menu_Line(row, ICON_LEVELING_POINT4);
}
#endif

#if HAS_LEVELING
#if ENABLED(OPTION_NEWAUTOLEVELING)
static void Item_Leveling_CatchZoffset(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_CATCH, LBLX, MBASE(row));	
	DWIN_Show_MultiLanguage_String(MTSTRING_Z_OFFSET, LBLX+get_MultiLanguageString_Width(MTSTRING_CATCH)+5, MBASE(row));
	Draw_Menu_Line(row, ICON_LEVELING_SAVE);
}

static void Item_Leveling_HomeZoffset(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_HOME, LBLX, MBASE(row));	
	DWIN_Show_MultiLanguage_String(MTSTRING_Z_OFFSET, LBLX+get_MultiLanguageString_Width(MTSTRING_HOME)+5, MBASE(row));
	DWIN_Draw_Small_Float22(MENUVALUE_X, MBASE(row), home_offset.z*MAXUNITMULT);
	Draw_Menu_Line(row, ICON_LEVELING_SAVE);
}

#elif ENABLED(AUTO_UPDATA_PROBE_Z_OFFSET)
static void Item_Leveling_CatchZoffset(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_CATCH, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_PROBE, LBLX+get_MultiLanguageString_Width(MTSTRING_CATCH)+5, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z_OFFSET, LBLX+get_MultiLanguageString_Width(MTSTRING_CATCH)+get_MultiLanguageString_Width(MTSTRING_PROBE)+10, MBASE(row));	
	Draw_Menu_Line(row, ICON_LEVELING_SAVE);
}
#endif
static void Item_Leveling_ProbeZoffset(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_PROBE, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z_OFFSET, LBLX+get_MultiLanguageString_Width(MTSTRING_PROBE)+6, MBASE(row));	
	DWIN_Draw_Small_Float22(MENUVALUE_X, MBASE(row), probe.offset.z*MAXUNITMULT);
 	Draw_Menu_Line(row, ICON_ZOFFSET);
}
static void Item_Leveling_Action(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_AUTO, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_LEVEL, LBLX+get_MultiLanguageString_Width(MTSTRING_AUTO)+6, MBASE(row));	
	//if(planner.leveling_active)
	//	DWIN_Draw_MaskString_Default_Color(COLOR_GREEN, 196, MBASE(row), PSTR("Actived"));
	//else
	//	DWIN_Draw_MaskString_Default_Color(COLOR_RED, 196, MBASE(row), PSTR(" --NA--"));
	Draw_Menu_Line(row, ICON_LEVELING_SAVE);
}
#endif

#if ENABLED(LCD_BED_LEVELING)
static void Popup_Window_Leveling() {
	DwinMenuID = DWMENU_LEVEL_BEDLEVELING;
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_BOTTOM);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_AUTOLEVELING, 101, 105);
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 17) / 2, 230, PSTR("Bed auto leveling"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 23) / 2, 260, PSTR("Please wait until done."));	
}

#if ENABLED(AUTO_UPDATA_PROBE_Z_OFFSET)
void DWIN_PopMenu_CatchProbeZOffset() {	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_BOTTOM);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_AUTOLEVELING, 101, 105);
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 23) / 2, 230, PSTR("Catching Probe Z Offset"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 23) / 2, 260, PSTR("Please wait until done."));
	
	DwinMenuID = DWMENU_LEVEL_CATCHPROBEZOFFSET;
}
#endif

#if ENABLED(OPTION_ZOFFSETWIZARD)
void DWIN_PopMenu_ZOffsetWizard() {	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_BOTTOM);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_AUTOLEVELING, 101, 85);
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 21) / 2, 190, PSTR("Z Offset Setup Wizard"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 21) / 2, 220, PSTR("Set Z HOME OFFSET for"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 22) / 2, 250, PSTR("first time starting up"));	
	DWIN_Show_ICON(ICON_YES_SEL, 26, 280);
	DWIN_Show_ICON(ICON_NO_UNSEL, 146, 280);
	
	DwinMenuID = DWMENU_POP_ZOFFSETWIZARD;		
}

inline void ICON_YESorNO_280(uint8_t Option){
	if (Option == 0) {
  	DWIN_Show_ICON(ICON_YES_SEL, 26, 280);
		DWIN_Show_ICON(ICON_NO_UNSEL, 146, 280);
 	}
	else{
		DWIN_Show_ICON(ICON_YES_UNSEL, 26, 280);
		DWIN_Show_ICON(ICON_NO_SEL, 146, 280);
	}
	DWIN_FEEDBACK_TICK();
}


void HMI_ZoffsetWizard() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();

	if (encoder_diffState == ENCODER_DIFF_NO) return;

	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_HomeZoffsetWizard.inc(2)) ICON_YESorNO_280(DwinMenu_HomeZoffsetWizard.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_HomeZoffsetWizard.dec()) ICON_YESorNO_280(DwinMenu_HomeZoffsetWizard.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_HomeZoffsetWizard.now) {
			case 0:				
				set_bed_leveling_enabled(false);
				home_offset.z = 0;
				queue.inject_P(PSTR("G28\nG1 Z5 F300\nG1 X150 Y150 F3000\n"));
				DwinMenuID = DWMENU_POP_ADJHOMEZOFFSET;
				Popup_Window_HomeAll();
			break;
			
			case 1:
				Draw_Main_Menu(true);
				break;
				
			default: 
				break;
		}
	}
	dwinLCD.UpdateLCD();
}
#endif

#if ENABLED(OPTION_NEWAUTOLEVELING)
float old_posz = 0.0;
void DWIN_PopMenu_HomeZOffsetSetup() {	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_BOTTOM);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_AUTOLEVELING, 101, 85);
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 21) / 2, 200, PSTR("Z Offset Setup Wizard"));
	DWIN_Draw_MaskString_Small_PopMenu( (272 - 8 * 27) / 2, 230, PSTR("Rotate knob to move the bed"));
	DWIN_Draw_MaskString_Small_PopMenu( (272 - 8 * 27) / 2, 254, PSTR("until it touched the nozzle"));	
	DWIN_Show_ICON(ICON_CONFIRM_E, 96, 280); 

	DwinMenuID = DWMENU_POP_ADJHOMEZOFFSET;		
	dwinLCD.UpdateLCD();
	
	planner.synchronize();
	old_posz = current_position.z;
	old_HomeZoffset_Scale = HMI_Value.HomeOffsetZ_scale = old_posz*MAXUNITMULT;	
}

void HMI_AdjustHomeZoffset() {	
	char statusbar_str[40]={0};	
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.HomeOffsetZ_scale))
		{
			home_offset.z = (old_posz - current_position.z);
			EncoderRate.enabled = false;
			HMI_AudioFeedback(settings.save());
			sprintf_P(statusbar_str, PSTR("Home Z Offset = %2.2f"), home_offset.z);
			DWIN_Show_Status_Message(COLOR_WHITE, statusbar_str, 3);
			
			set_bed_leveling_enabled(false);
			DWIN_PopMenu_CatchProbeZOffset();
			DWIN_G29_Show_Messge(G29_CATCH_START);					
			queue.inject_P(PSTR("G28\nG29 N\n"));			
		}
		else
		{
			NOLESS(HMI_Value.HomeOffsetZ_scale, (-20) * MAXUNITMULT);
			NOMORE(HMI_Value.HomeOffsetZ_scale, (20) * MAXUNITMULT);			
			current_position.z += ((float)(HMI_Value.HomeOffsetZ_scale - old_HomeZoffset_Scale))/MAXUNITMULT;
			old_HomeZoffset_Scale = HMI_Value.HomeOffsetZ_scale;			
			planner.buffer_line(current_position, MMM_TO_MMS(HOMING_FEEDRATE_Z), 0);	
			planner.synchronize();
			sprintf_P(statusbar_str, PSTR("Current Z = %2.2f"), current_position.z);
			DWIN_Show_Status_Message(COLOR_WHITE, statusbar_str, 3);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_SetHomeZoffset() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	uint8_t zoff_line = DwinMenu_leveling.now + MROWS - DwinMenu_leveling.index;
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.HomeOffsetZ_scale)){
			home_offset.z = (float)HMI_Value.HomeOffsetZ_scale/MAXUNITMULT;
			EncoderRate.enabled = false;
			HMI_AudioFeedback(settings.save());
			
			DWIN_Draw_Small_Float22(MENUVALUE_X, MBASE(zoff_line), HMI_Value.HomeOffsetZ_scale);
			DwinMenuID = DWMENU_LEVELING;			
		}
		else{
			NOLESS(HMI_Value.HomeOffsetZ_scale, (-20) * MAXUNITMULT);
			NOMORE(HMI_Value.HomeOffsetZ_scale, (20) * MAXUNITMULT);		
			DWIN_Draw_Selected_Small_Float22(MENUVALUE_X, MBASE(zoff_line), HMI_Value.HomeOffsetZ_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

#endif

#if (HAS_OFFSET_MENU  && ENABLED(OPTION_GLASS_BED))
static void Popup_Remove_Glass() {
	DwinMenuID = DWMENU_POP_REMOVE_GLASS;
	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_BOTTOM);
	Draw_Popup_Bkgd_60();

	DWIN_Show_ICON(ICON_AUTOLEVELING, 101, 105);
	dwinLCD.Draw_String(false, true, font8x16, COLOR_BG_RED, COLOR_BG_WINDOW, (272 - 8 * 13) / 2, 180, PSTR("!!Attention!!"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 19) / 2, 212, PSTR("Remove glass on bed"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 15) / 2, 232, PSTR("before catching"));	
	DWIN_Show_ICON(ICON_CONFIRM_E, 96, 280);
}
#endif // (HAS_OFFSET_MENU  && ENABLED(OPTION_GLASS_BED))
#endif //LCD_BED_LEVELING

void Draw_Leveling_Menu(bool reset_item) {
	DwinMenuID = DWMENU_LEVELING;
	if(reset_item)	DwinMenu_leveling.reset();
	
#if (LEVELING_CASE_TOTAL > MROWS)
	const int8_t scrol = MROWS - DwinMenu_leveling.index;	
	#define LESCROL(L) (scrol + (L))
#else
	#define LESCROL(L) (L)
#endif
	#define LEVISI(L) WITHIN(LESCROL(L), 0, MROWS)

 	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_STATUS);
#if HAS_LEVELING
	 HMI_flag.Leveling_Case_Total = HMI_flag.Leveling_Menu_Fg?LEVELING_CASE_TOTAL : LEVELING_CASE_HOMEALL;
#endif

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_LEVELING, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);

	if(LEVISI(LEVELING_CASE_BACK)) Draw_Back_First(DwinMenu_leveling.now == LEVELING_CASE_BACK);    	// < Back 

#if ENABLED(LEVEL_BED_CORNERS)	
	if(LEVISI(LEVELING_CASE_POINT1)) Item_Leveling_Point1(LESCROL(LEVELING_CASE_POINT1));   					// point 1
	if(LEVISI(LEVELING_CASE_POINT2)) Item_Leveling_Point2(LESCROL(LEVELING_CASE_POINT2));   					// point 2
	if(LEVISI(LEVELING_CASE_POINT3)) Item_Leveling_Point3(LESCROL(LEVELING_CASE_POINT3));   					// point 3
	if(LEVISI(LEVELING_CASE_POINT4)) Item_Leveling_Point4(LESCROL(LEVELING_CASE_POINT4));   					// point 4	
#endif
	if(LEVISI(LEVELING_CASE_HOMEALL)) Item_Leveling_Home(LESCROL(LEVELING_CASE_HOMEALL));   					// Home All

#if HAS_LEVELING
 if(HMI_flag.Leveling_Menu_Fg){			
	#if ENABLED(AUTO_UPDATA_PROBE_Z_OFFSET)
		if(LEVISI(LEVELING_CASE_CATCHOFFSET)) Item_Leveling_CatchZoffset(LESCROL(LEVELING_CASE_CATCHOFFSET));
	#endif
		if(LEVISI(LEVELING_CASE_ACTION)) Item_Leveling_Action(LESCROL(LEVELING_CASE_ACTION));
	#if ENABLED(OPTION_NEWAUTOLEVELING)
		if(LEVISI(LEVELING_CASE_HOMEZOFFSET)) Item_Leveling_HomeZoffset(LESCROL(LEVELING_CASE_HOMEZOFFSET));
	#endif
		if(LEVISI(LEVELING_CASE_PROBEZOFFSET)) Item_Leveling_ProbeZoffset(LESCROL(LEVELING_CASE_PROBEZOFFSET));		
	}
#endif 
	if (DwinMenu_leveling.now) Draw_Menu_Cursor(LESCROL(DwinMenu_leveling.now));
	Draw_Status_Area();
}

#if HAS_LEVELING
void HMI_SetProbeZoffset() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	uint8_t zoff_line = DwinMenu_leveling.now + MROWS - DwinMenu_leveling.index;
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.ProbeZoffset_Scale)){
			probe.offset.z = (float)HMI_Value.ProbeZoffset_Scale/MAXUNITMULT;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float22(MENUVALUE_X, MBASE(zoff_line), HMI_Value.ProbeZoffset_Scale);
			DwinMenuID = DWMENU_LEVELING;			
		}
		else{
			NOLESS(HMI_Value.ProbeZoffset_Scale, (Z_PROBE_OFFSET_RANGE_MIN) * MAXUNITMULT);
			NOMORE(HMI_Value.ProbeZoffset_Scale, (Z_PROBE_OFFSET_RANGE_MAX) * MAXUNITMULT);		
			DWIN_Draw_Selected_Small_Float22(MENUVALUE_X, MBASE(zoff_line), HMI_Value.ProbeZoffset_Scale);
		}
		dwinLCD.UpdateLCD();
	}
}
#endif // HAS_LEVELING

#if ENABLED(LEVEL_BED_CORNERS)
constexpr uint16_t lfrb[4] = LEVEL_CORNERS_INSET_LFRB;
#endif
void HMI_BedLeveling() {
#if DISABLED(OPTION_NEWAUTOLEVELING)	
	char gcode_string[100]={0};
#endif
	static bool last_leveling_status = false;	
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;
 
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_leveling.inc(1 + HMI_flag.Leveling_Case_Total)) {
			if (DwinMenu_leveling.now > MROWS && DwinMenu_leveling.now > DwinMenu_leveling.index) {
				DwinMenu_leveling.index = DwinMenu_leveling.now;
				Scroll_Menu(DWIN_SCROLL_UP);
			#if HAS_LEVELING				
				if(DwinMenu_leveling.index == LEVELING_CASE_PROBEZOFFSET) Item_Leveling_ProbeZoffset(MROWS);
			#if ENABLED(OPTION_NEWAUTOLEVELING)
				else if(DwinMenu_leveling.index == LEVELING_CASE_HOMEZOFFSET) Item_Leveling_HomeZoffset(MROWS);						
			#endif
				else if(DwinMenu_leveling.index == LEVELING_CASE_ACTION) Item_Leveling_Action(MROWS);
			#if ENABLED(AUTO_UPDATA_PROBE_Z_OFFSET)
				else if(DwinMenu_leveling.index == LEVELING_CASE_CATCHOFFSET) Item_Leveling_CatchZoffset(MROWS);
			#endif				
				else if(DwinMenu_leveling.index == LEVELING_CASE_HOMEALL) Item_Leveling_Home(MROWS);
				#if ENABLED(LEVEL_BED_CORNERS)
				else if(DwinMenu_leveling.index == LEVELING_CASE_POINT4) Item_Leveling_Point4(MROWS);
				else if(DwinMenu_leveling.index == LEVELING_CASE_POINT3) Item_Leveling_Point3(MROWS);
				else if(DwinMenu_leveling.index == LEVELING_CASE_POINT2) Item_Leveling_Point2(MROWS);
				else if(DwinMenu_leveling.index == LEVELING_CASE_POINT1) Item_Leveling_Point1(MROWS);
				#endif
				
			#endif
				
			}
			else {
				Move_Highlight(1, DwinMenu_leveling.now + MROWS - DwinMenu_leveling.index);
			}
		}
	}
	else if(encoder_diffState == ENCODER_DIFF_CCW) {
		if(DwinMenu_leveling.dec()) {
			if(DwinMenu_leveling.now < DwinMenu_leveling.index - MROWS) {
				DwinMenu_leveling.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if (DwinMenu_leveling.index - MROWS == LEVELING_CASE_BACK) Draw_Back_First();				
			#if ENABLED(LEVEL_BED_CORNERS)
				else if(DwinMenu_leveling.index - MROWS == LEVELING_CASE_POINT1) Item_Leveling_Point1(0);
				else if(DwinMenu_leveling.index - MROWS == LEVELING_CASE_POINT2) Item_Leveling_Point2(0);
				else if(DwinMenu_leveling.index - MROWS == LEVELING_CASE_POINT3) Item_Leveling_Point3(0);
				else if(DwinMenu_leveling.index - MROWS == LEVELING_CASE_POINT4) Item_Leveling_Point4(0);
			#endif
				else if(DwinMenu_leveling.index - MROWS == LEVELING_CASE_HOMEALL) Item_Leveling_Home(0);
			#if HAS_LEVELING
			#if ENABLED(AUTO_UPDATA_PROBE_Z_OFFSET)
				else if(DwinMenu_leveling.index - MROWS == LEVELING_CASE_CATCHOFFSET) Item_Leveling_CatchZoffset(0);
			#endif			
				else if(DwinMenu_leveling.index - MROWS == LEVELING_CASE_ACTION) Item_Leveling_Action(0);
			#if ENABLED(OPTION_NEWAUTOLEVELING)
				else if(DwinMenu_leveling.index - MROWS == LEVELING_CASE_HOMEZOFFSET) Item_Leveling_HomeZoffset(0);
			#endif
				else if(DwinMenu_leveling.index - MROWS == LEVELING_CASE_PROBEZOFFSET) Item_Leveling_ProbeZoffset(0);				
			#endif
			}
			else {
				Move_Highlight(-1, DwinMenu_leveling.now + MROWS - DwinMenu_leveling.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		if(DwinMenuID == DWMENU_LEVEL_DONECONFIRM){
			encoder_diffState = ENCODER_DIFF_NO;			
			if(IS_SD_PRINTING() || IS_SD_PAUSED()){
				Draw_Printing_Menu(PRINT_CASE_PAUSE, true);
			}
			else
			{				
				Draw_Leveling_Menu(true);
			}
			return;
		}
		switch(DwinMenu_leveling.now) {
			case 0: 										// Back
				Clear_Dwin_Area(AREA_BOTTOM);
				Draw_Prepare_Menu(PREPARE_CASE_LEVELING);
	    break;

		#if ENABLED(LEVEL_BED_CORNERS)		
			case LEVELING_CASE_POINT1: 										
				DwinMenuID = DWMENU_LEVELING;								
				Clear_Dwin_Area(AREA_BOTTOM);			
				last_leveling_status = planner.leveling_active;
				set_bed_leveling_enabled(false);
				ZERO(gcode_string);
				if(axes_should_home())	
					sprintf_P(gcode_string,PSTR("G28\nG91\nG1 Z10 F1500\nG90\nG1 X%d Y%d F3000\nG1 Z0 F500"),lfrb[0],lfrb[1]);
				else
					sprintf_P(gcode_string,PSTR("G28O\nG91\nG1 Z10 F1500\nG90\nG1 X%d Y%d F3000\nG1 Z0 F500"),lfrb[0],lfrb[1]);
				queue.inject(gcode_string);
				planner.synchronize();
				set_bed_leveling_enabled(last_leveling_status);
			break;
				
		 case LEVELING_CASE_POINT2: 										
				DwinMenuID = DWMENU_LEVELING;				
				Clear_Dwin_Area(AREA_BOTTOM);
				last_leveling_status = planner.leveling_active;
				set_bed_leveling_enabled(false);
				ZERO(gcode_string);
				if(axes_should_home())
					sprintf_P(gcode_string,PSTR("G28\nG91\nG1 Z10 F1500\nG90\nG1 X%d Y%d F3000\nG1 Z0 F500"),X_BED_SIZE-lfrb[2],lfrb[1]);	
				else
					sprintf_P(gcode_string,PSTR("G28O\nG91\nG1 Z10 F1500\nG90\nG1 X%d Y%d F3000\nG1 Z0 F500"),X_BED_SIZE-lfrb[2],lfrb[1]);	
				queue.inject(gcode_string);
				planner.synchronize();
				set_bed_leveling_enabled(last_leveling_status);
	    break;
				
			case LEVELING_CASE_POINT3: 										
				DwinMenuID = DWMENU_LEVELING;				
				Clear_Dwin_Area(AREA_BOTTOM);				
				last_leveling_status = planner.leveling_active;
				set_bed_leveling_enabled(false);
				ZERO(gcode_string);
				if(axes_should_home())
					sprintf_P(gcode_string,PSTR("G28\nG91\nG1 Z10 F1500\nG90\nG1 X%d Y%d F3000\nG1 Z0 F500"),X_BED_SIZE-lfrb[2],Y_BED_SIZE-lfrb[3]);
				else
					sprintf_P(gcode_string,PSTR("G28O\nG91\nG1 Z10 F1500\nG90\nG1 X%d Y%d F3000\nG1 Z0 F500"),X_BED_SIZE-lfrb[2],Y_BED_SIZE-lfrb[3]);
				queue.inject(gcode_string);
				planner.synchronize();
				set_bed_leveling_enabled(last_leveling_status);
	    break;
		
			case LEVELING_CASE_POINT4: 										
				DwinMenuID = DWMENU_LEVELING;								
				Clear_Dwin_Area(AREA_BOTTOM);
				last_leveling_status = planner.leveling_active;
				set_bed_leveling_enabled(false);
				ZERO(gcode_string);
				if(axes_should_home())
					sprintf_P(gcode_string,PSTR("G28\nG91\nG1 Z10 F1500\nG90\nG1 X%d Y%d F3000\nG1 Z0 F500"),lfrb[0],Y_BED_SIZE-lfrb[3]);
				else
					sprintf_P(gcode_string,PSTR("G28O\nG91\nG1 Z10 F1500\nG90\nG1 X%d Y%d F3000\nG1 Z0 F500"),lfrb[0],Y_BED_SIZE-lfrb[3]);
				queue.inject(gcode_string);
				planner.synchronize();
				set_bed_leveling_enabled(last_leveling_status);
			break;
		#endif
		
			case LEVELING_CASE_HOMEALL: 										
				DwinMenuID = DWMENU_LEVELING;
				last_leveling_status = planner.leveling_active;
				set_bed_leveling_enabled(false);
				queue.inject_P(PSTR("G28"));					
				Popup_Window_HomeAll();
				set_bed_leveling_enabled(last_leveling_status);
			break;
		
	#if HAS_LEVELING
		#if ENABLED(OPTION_ZOFFSETWIZARD)
			case LEVELING_CASE_CATCHOFFSET:
				set_bed_leveling_enabled(false);				
				home_offset.z = 0;
				queue.inject_P(PSTR("G28\nG1 Z5 F300\nG1 X155 Y155 Z5 F3000\n"));
				DwinMenuID = DWMENU_POP_ADJHOMEZOFFSET;
				Popup_Window_HomeAll();
			break;
			
		#elif ENABLED(AUTO_UPDATA_PROBE_Z_OFFSET)
		case LEVELING_CASE_CATCHOFFSET:
			#if (HAS_OFFSET_MENU && ENABLED(OPTION_GLASS_BED))
				#if HAS_OFFSET_MENU
				if((DwinMenuID == DWMENU_POP_REMOVE_GLASS) || (home_offset.z > -2))
				#endif
			#endif
				{					
					set_bed_leveling_enabled(false);					
					DWIN_PopMenu_CatchProbeZOffset();
					DWIN_G29_Show_Messge(G29_CATCH_START);					
					queue.inject_P(PSTR("G28\nG29 N\n"));
				}
			#if (HAS_OFFSET_MENU  && ENABLED(OPTION_GLASS_BED))
	  	 	else
				{					
					Popup_Remove_Glass();
	  	 	}
			#endif
				planner.synchronize();
			break;
		#endif

		#if ENABLED(OPTION_NEWAUTOLEVELING)
		case LEVELING_CASE_HOMEZOFFSET:
			DwinMenuID = DWMENU_LEVEL_SETHOMEOFFSET;
    	HMI_Value.HomeOffsetZ_scale= home_offset.z*MAXUNITMULT;
    	DWIN_Draw_Selected_Small_Float22(MENUVALUE_X, MBASE(LEVELING_CASE_HOMEZOFFSET + MROWS - DwinMenu_leveling.index), HMI_Value.HomeOffsetZ_scale);
    	EncoderRate.enabled = true;
		break;
		#endif
		
		case LEVELING_CASE_PROBEZOFFSET:
			DwinMenuID = DWMENU_LEVEL_SETPROBEOFFSET;
    	HMI_Value.ProbeZoffset_Scale = probe.offset.z*MAXUNITMULT;
    	DWIN_Draw_Selected_Small_Float22(MENUVALUE_X, MBASE(LEVELING_CASE_PROBEZOFFSET + MROWS - DwinMenu_leveling.index), HMI_Value.ProbeZoffset_Scale);
    	EncoderRate.enabled = true;
		break;
		
	 	case LEVELING_CASE_ACTION:			
			set_bed_leveling_enabled(false);
			Popup_Window_Leveling();
			DWIN_G29_Show_Messge(G29_MESH_START);
			queue.inject_P(PSTR("G28\nG29\n"));			
			planner.synchronize();
		break;
 #endif
		
		default:break;
		} 
	}
	dwinLCD.UpdateLCD();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Prepare>>Language
//
static void Item_Language_EN(const uint8_t row) {
 DWIN_Show_MultiLanguage_String(MTSTRING_LANGUAGE_EN, LBLX, MBASE(row));
 Draw_Menu_Line(row, ICON_EN);
}

static void Item_Language_SP(const uint8_t row) {
 DWIN_Show_MultiLanguage_String(MTSTRING_LANGUAGE_SP, LBLX, MBASE(row));
 Draw_Menu_Line(row, ICON_SP);
}

static void Item_Language_RU(const uint8_t row) {
 DWIN_Show_MultiLanguage_String(MTSTRING_LANGUAGE_RU, LBLX, MBASE(row));
 Draw_Menu_Line(row, ICON_RU);
}

static void Item_Language_FR(const uint8_t row) {
 DWIN_Show_MultiLanguage_String(MTSTRING_LANGUAGE_FR, LBLX, MBASE(row));
 Draw_Menu_Line(row, ICON_FR);
}

static void Item_Language_PT(const uint8_t row) {
 DWIN_Show_MultiLanguage_String(MTSTRING_LANGUAGE_PT, LBLX, MBASE(row));
 Draw_Menu_Line(row, ICON_PT);
}

/*
static void Item_Language_ZH(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_LANGUAGE_ZH, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_CH);
}
*/

inline void Draw_Language_Menu() {
	DwinMenuID = DWMENU_LANGUAGE;
	DwinMenu_language.reset();
	//DwinMenu_language.index = _MAX(DwinMenu_language.now, MROWS);

#if (LANGUAGE_CASE_TOTAL > MROWS)
	const int8_t scroll = MROWS - DwinMenu_language.index;
	#define LASCROL(L) (scroll + (L))
#else
	#define LASCROL(L) (L)
#endif
	#define LAVISI(L) WITHIN(LASCROL(L), 0, MROWS)
	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_LANGUAGE, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	if(LAVISI(LANGUAGE_CASE_BACK)) Draw_Back_First(DwinMenu_language.now == LANGUAGE_CASE_BACK);
	
	if(LAVISI(LANGUAGE_CASE_EN)) Item_Language_EN(LASCROL(LANGUAGE_CASE_EN));
	if(LAVISI(LANGUAGE_CASE_SP)) Item_Language_SP(LASCROL(LANGUAGE_CASE_SP));
	if(LAVISI(LANGUAGE_CASE_RU)) Item_Language_RU(LASCROL(LANGUAGE_CASE_RU));
	if(LAVISI(LANGUAGE_CASE_FR)) Item_Language_FR(LASCROL(LANGUAGE_CASE_FR));
	if(LAVISI(LANGUAGE_CASE_PT)) Item_Language_PT(LASCROL(LANGUAGE_CASE_PT));
	//if(LAVISI(LANGUAGE_CASE_ZH)) Item_Language_ZH(LASCROL(LANGUAGE_CASE_ZH));
	
	if(DwinMenu_language.now) Draw_Menu_Cursor(LASCROL(DwinMenu_language.now));		
}

void HMI_SetLanguage_PicCache() {
	dwinLCD.JPG_CacheTo1(HMI_flag.language + 1);
	
}

void HMI_Language() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;
 
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_language.inc(LANGUAGE_CASE_END)) {
			if (DwinMenu_language.now > MROWS && DwinMenu_language.now > DwinMenu_language.index) {
				DwinMenu_language.index = DwinMenu_language.now;

				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);
				Draw_Menu_Icon(MROWS, LANGUAGE_CASE_EN + DwinMenu_language.now - 1);
				//if(DwinMenu_language.index == LANGUAGE_CASE_ZH) Item_Language_ZH(MROWS);
				if(DwinMenu_language.index == LANGUAGE_CASE_PT) Item_Language_PT(MROWS);
			}
			else {
				Move_Highlight(1, DwinMenu_language.now + MROWS - DwinMenu_language.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
	  if (DwinMenu_language.dec()) {
			if (DwinMenu_language.now < DwinMenu_language.index - MROWS) {
		    DwinMenu_language.index--;
		    Scroll_Menu(DWIN_SCROLL_DOWN);				
				if (DwinMenu_language.index - MROWS == LANGUAGE_CASE_BACK)	Draw_Back_First();			
				else if(DwinMenu_language.index - MROWS == LANGUAGE_CASE_EN) Item_Language_EN(0);
			}
			else {
		    Move_Highlight(-1, DwinMenu_language.now + MROWS - DwinMenu_language.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_language.now) {
			case 0: 										// Back
				Draw_Prepare_Menu(PREPARE_CASE_LANG);
			break;

		 case LANGUAGE_CASE_EN: 
		 case LANGUAGE_CASE_SP:
		 case LANGUAGE_CASE_RU:
			HMI_flag.language = DwinMenu_language.now - 1;
			dwinLCD.JPG_CacheToN(1,HMI_flag.language+1);
			HMI_AudioFeedback(settings.save());
			Draw_Prepare_Menu(PREPARE_CASE_LANG);
	   break;
			
		case LANGUAGE_CASE_FR: 
		case LANGUAGE_CASE_PT:
			HMI_flag.language = DwinMenu_language.now - 1;
			dwinLCD.JPG_CacheToN(1,HMI_flag.language+1);
			HMI_AudioFeedback(settings.save());
			Draw_Prepare_Menu(PREPARE_CASE_LANG);
		break; 	
			
		//case LANGUAGE_CASE_ZH: 
		//break;
	 
		default:break;
		} 
	}
	dwinLCD.UpdateLCD();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Prepare>>PowerDown
//
#if (HAS_SUICIDE || ENABLED(PSU_CONTROL))
static void Popup_Window_Powerdown() {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();	
	DWIN_Show_MultiLanguage_String(MTSTRING_POWER_OUTAGE, TITLE_X, TITLE_Y);
	DWIN_Show_ICON(ICON_POWER_DOWN, 86, 95);
	ICON_YESorNO(DwinMenu_powerdown.now);
	DWIN_Draw_MaskString_Default_PopMenu((DWIN_WIDTH - 10 * 17) / 2, 290, PSTR("Confirm Shutdown?"));
}

void HMI_Powerdown() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();

	if (HMI_flag.Is_shutdown || encoder_diffState == ENCODER_DIFF_NO) return;

	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_powerdown.inc(2)) ICON_YESorNO(DwinMenu_powerdown.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_powerdown.dec()) ICON_YESorNO(DwinMenu_powerdown.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_powerdown.now) {
			case 0:
				thermalManager.disable_all_heaters();
				if(thermalManager.degHotend(0) > 80){
					HMI_flag.Is_shutdown = true;
					dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 14, 228, 258, 330);
					DWIN_FEEDBACK_CONFIRM();
					DWIN_Draw_MaskString_Default_PopMenu((DWIN_WIDTH - 10 * 22) / 2, 240, PSTR("Machine will shut down"));
					DWIN_Draw_MaskString_Default_PopMenu((DWIN_WIDTH - 10 * 21) / 2, 270, PSTR("after hotend is cool."));
				}
				else{
					//queue.inject_P(PSTR("M81"));
				#if HAS_SUICIDE
					suicide();					
				#elif ENABLED(PSU_CONTROL)
					PSU_OFF();
				#endif
				}
			break;
			
			case 1: 
				Draw_Prepare_Menu(PREPARE_CASE_POWERDOWN);
				break;
			default: break;
		}
	}
	dwinLCD.UpdateLCD();
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Prepare
//
void HMI_Prepare() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_prepare.inc(PREPARE_CASE_END)) {
			if (DwinMenu_prepare.now > MROWS && DwinMenu_prepare.now > DwinMenu_prepare.index) {
				DwinMenu_prepare.index = DwinMenu_prepare.now;

				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);				
				if(DwinMenu_prepare.index == PREPARE_CASE_LANG) Item_Prepare_Lang(MROWS);		
			#if (HAS_SUICIDE || ENABLED(PSU_CONTROL))				
				else if(DwinMenu_prepare.index == PREPARE_CASE_POWERDOWN) Item_Prepare_Powerdown(MROWS);
			#endif
				else if(DwinMenu_prepare.index == PREPARE_CASE_DISA) Item_Prepare_Disable(MROWS);
				//else if(DwinMenu_prepare.index == PREPARE_CASE_LEVELING) Item_Prepare_Leveling(MROWS);				
				//else if(DwinMenu_prepare.index == PREPARE_CASE_TEMP) Item_Prepare_Temp(MROWS);
				//else if(DwinMenu_prepare.index == PREPARE_CASE_MOVE) Item_Prepare_Move(MROWS);
				//else if(DwinMenu_prepare.index == PREPARE_CASE_HOME) Item_Prepare_Home(MROWS);
			}
			else {
				Move_Highlight(1, DwinMenu_prepare.now + MROWS - DwinMenu_prepare.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_prepare.dec()) {
			if (DwinMenu_prepare.now < DwinMenu_prepare.index - MROWS) {
				DwinMenu_prepare.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);

				if(DwinMenu_prepare.index - MROWS ==  PREPARE_CASE_BACK)	Draw_Back_First();				
				else if(DwinMenu_prepare.index - MROWS == PREPARE_CASE_HOME) Item_Prepare_Home(0);				
				else if(DwinMenu_prepare.index - MROWS == PREPARE_CASE_MOVE) Item_Prepare_Move(0);
				else if(DwinMenu_prepare.index - MROWS == PREPARE_CASE_TEMP ) Item_Prepare_Temp(0);
				//else if(DwinMenu_prepare.index - MROWS == PREPARE_CASE_LEVELING) Item_Prepare_Leveling(0);
				//else if(DwinMenu_prepare.index - MROWS == PREPARE_CASE_DISA) Item_Prepare_Disable(0);				
			#if (HAS_SUICIDE || ENABLED(PSU_CONTROL))
				//else if(DwinMenu_prepare.index - MROWS == PREPARE_CASE_POWERDOWN) Item_Prepare_Powerdown(0);		
			#endif
			}
			else {
				Move_Highlight(-1, DwinMenu_prepare.now + MROWS - DwinMenu_prepare.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_prepare.now) {
			case 0: // Back
				Draw_Main_Menu(false, MAIN_CASE_PREPARE);
			break;

			case PREPARE_CASE_HOME: // Homing
				Draw_Home_Menu();	   
			break;

			case PREPARE_CASE_MOVE: // Axis move				
				Draw_Move_Menu();			
				DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(AXISMOVE_CASE_MOVEX), current_position.x * MINUNITMULT);
				DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(AXISMOVE_CASE_MOVEY), current_position.y * MINUNITMULT);
				DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(AXISMOVE_CASE_MOVEZ), current_position.z * MINUNITMULT);
		#if HAS_HOTEND	   
				DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(AXISMOVE_CASE_EX1), 0);
				DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(AXISMOVE_CASE_EX2), 0);
		#endif
			break;

			case PREPARE_CASE_TEMP: // Temperature				
		#if SPINDLE_USED_BEDPIN
				if(!cutter.Spindle_Enabled){
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Can't work on spindle mode!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
		#endif
				HMI_flag.show_mode = SHOWED_TEMPERATURE;
				Draw_Temperature_Menu();
			break;
			
			case PREPARE_CASE_FILAMENT: // filament						
		#if SPINDLE_USED_BEDPIN
				if(!cutter.Spindle_Enabled){
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Can't work on spindle mode!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
		#endif
				Draw_Filament_Menu();
			break;

			case PREPARE_CASE_LEVELING: 		// Leveling				
				set_axis_never_homed(Z_AXIS);
				Draw_Leveling_Menu(true);
			break;

			case PREPARE_CASE_DISA: // Disable steppers
				queue.inject_P(PSTR("M84"));				
			break;

			case PREPARE_CASE_LANG: // Toggle Language			
				Draw_Language_Menu();
			break;

		#if (HAS_SUICIDE || ENABLED(PSU_CONTROL))
			case PREPARE_CASE_POWERDOWN: 		// Powerdown
				DwinMenuID = DWMENU_POWERDOWN;				
				DwinMenu_powerdown.set(1);
				Popup_Window_Powerdown();
			break;
		#endif

			default: break;
		}
	}
	dwinLCD.UpdateLCD();
}

#endif

