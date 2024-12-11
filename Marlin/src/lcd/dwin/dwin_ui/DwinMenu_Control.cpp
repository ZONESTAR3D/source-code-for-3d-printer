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
 * DwinMenu_control.cpp
 */

 #include "../../../inc/MarlinConfig.h"

#if HAS_DWIN_LCD
#include "dwin.h"

#if ENABLED(CASE_LIGHT_MENU)
#include "../../../feature/caselight.h"
#endif

#define _BREAK_WHILE_PRINTING {if(IS_SD_PRINTING() || IS_SD_PAUSED() || IS_SD_FILE_OPEN()) break;}		
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Control >>> Main
//
#if ENABLED(MIXING_EXTRUDER)
static void Item_Control_Mixer(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MIXER, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_MIXER);
	Draw_More_Icon(row);
}
#endif

static void Item_Control_Config(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_CONFIG, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_CONTACT);
	Draw_More_Icon(row);
}

static void Item_Control_Motion(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MOTION, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_MOTION);
	Draw_More_Icon(row);
}

static void Item_Control_PLA(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_PREHEAT, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_PLA, LBLX+get_MultiLanguageString_Width(MTSTRING_PREHEAT)+5, MBASE(row));
	Draw_Menu_Line(row, ICON_PLAPREHEAT);
	Draw_More_Icon(row);
}

static void Item_Control_ABS(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_PREHEAT, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_ABS, LBLX+get_MultiLanguageString_Width(MTSTRING_PREHEAT)+5, MBASE(row));
	Draw_Menu_Line(row, ICON_ABSPREHEAT);
	Draw_More_Icon(row);	
}

#if ENABLED(BLTOUCH)
static void Item_Control_BLtouch(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_BLTOUCH, LBLX, MBASE(row)); 
	Draw_Menu_Line(row, ICON_BLTOUCH);
	Draw_More_Icon(row);
}
#endif

#if ENABLED(EEPROM_SETTINGS)
static void Item_Control_Save(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_STORE, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_SETTINGS, LBLX+get_MultiLanguageString_Width(MTSTRING_STORE)+5, MBASE(row));
	Draw_Menu_Line(row, ICON_WRITEEEPROM);
}

static void Item_Control_Load(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_LOAD, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_SETTINGS, LBLX+get_MultiLanguageString_Width(MTSTRING_LOAD)+5, MBASE(row));
	Draw_Menu_Line(row, ICON_READEEPROM);
}

static void Item_Control_Reset(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_RESET, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_RESUMEEEPROM);
}
#endif

int8_t mixing_menu_adjust(){
	#if BOTH(MIXING_EXTRUDER, OPTION_MIXING_SWITCH)
	if(!mixer.mixing_enabled)
		return -1;
	#endif
	return 0;
}

void Draw_Control_Menu(const uint8_t MenuItem) {
	DwinMenuID = DWMENU_CONTROL;
	DwinMenu_control.set(MenuItem);
	DwinMenu_control.index = _MAX(DwinMenu_control.now, MROWS);

#if (CONTROL_CASE_TOTAL > MROWS)
	const int8_t scroll = MROWS - DwinMenu_control.index;	
	#define CCSCROL(L) (scroll + (L))
#else
	#define CCSCROL(L) (L)
#endif
	#define CCVISI(L) WITHIN(CCSCROL(L), 0, MROWS)

	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_STATUS);
	
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_CONTROL, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	if (CCVISI(CONTROL_CASE_BACK)) Draw_Back_First(DwinMenu_control.now == CONTROL_CASE_BACK);             // < Back

#if ENABLED(MIXING_EXTRUDER)
	#if ENABLED(OPTION_MIXING_SWITCH)
	if (mixer.mixing_enabled){
		if (CCVISI(CONTROL_CASE_MIXER)) Item_Control_Mixer(CCSCROL(CONTROL_CASE_MIXER));
	}
	#else
		if (CCVISI(CONTROL_CASE_MIXER)) Item_Control_Mixer(CCSCROL(CONTROL_CASE_MIXER));
	#endif		
#endif
	if (CCVISI(CONTROL_CASE_CONFIG + mixing_menu_adjust())) Item_Control_Config(CCSCROL(CONTROL_CASE_CONFIG + mixing_menu_adjust()));
	if (CCVISI(CONTROL_CASE_MOTION + mixing_menu_adjust())) Item_Control_Motion(CCSCROL(CONTROL_CASE_MOTION + mixing_menu_adjust()));
	if (CCVISI(CONTROL_CASE_SETPLA + mixing_menu_adjust())) Item_Control_PLA(CCSCROL(CONTROL_CASE_SETPLA + mixing_menu_adjust()));
	if (CCVISI(CONTROL_CASE_SETABS + mixing_menu_adjust())) Item_Control_ABS(CCSCROL(CONTROL_CASE_SETABS + mixing_menu_adjust()));

#if ENABLED(BLTOUCH)
	if (CCVISI(CONTROL_CASE_BLTOUCH + mixing_menu_adjust())) Item_Control_BLtouch(CCSCROL(CONTROL_CASE_BLTOUCH + mixing_menu_adjust()));
#endif
#if ENABLED(EEPROM_SETTINGS)
	if (CCVISI(CONTROL_CASE_SAVE + mixing_menu_adjust())) Item_Control_Save(CCSCROL(CONTROL_CASE_SAVE + mixing_menu_adjust()));
	if (CCVISI(CONTROL_CASE_LOAD + mixing_menu_adjust())) Item_Control_Load(CCSCROL(CONTROL_CASE_LOAD + mixing_menu_adjust()));
	if (CCVISI(CONTROL_CASE_RESET + mixing_menu_adjust())) Item_Control_Reset(CCSCROL(CONTROL_CASE_RESET + mixing_menu_adjust()));
#endif

	if(DwinMenu_control.now)	Draw_Menu_Cursor(CCSCROL(DwinMenu_control.now));	
	Draw_Status_Area();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Control >> Mixer
//
#if ENABLED (MIXING_EXTRUDER)
void Draw_Mixer_Menu(const uint8_t MenuItem) {	
	DwinMenuID = DWMENU_MIXER;	
	DwinMenu_mixer.set(MenuItem);
	DwinMenu_mixer.index = _MAX(DwinMenu_mixer.now, MROWS);
	HMI_Value.old_mix_mode = -1;

	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_MIXER, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	Draw_Back_First(DwinMenu_mixer.now == MIXER_CASE_BACK);

	//Set V-TOOL
	Draw_Menu_Line(MIXER_CASE_SETVTOOL, ICON_S_VTOOL);
	DWIN_Show_MultiLanguage_String(MTSTRING_SET, LBLX, MBASE(MIXER_CASE_SETVTOOL));
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_VTOOL, LBLX+get_MultiLanguageString_Width(MTSTRING_SET)+5, MBASE(MIXER_CASE_SETVTOOL));
	Draw_More_Icon(MIXER_CASE_SETVTOOL);
	
	//Mauanl (Current V-TOOL)	
	HMI_Value.current_vtool = mixer.selected_vtool;
	Draw_Menu_Line(MIXER_CASE_CURRENTVTOOL, ICON_MIXER_MANUAL);
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_CURRENT, LBLX, MBASE(MIXER_CASE_CURRENTVTOOL));
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_VTOOL, LBLX+get_MultiLanguageString_Width(MTSTRING_MIX_CURRENT)+5, MBASE(MIXER_CASE_CURRENTVTOOL));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MIXER_CASE_CURRENTVTOOL), HMI_Value.current_vtool);
	
	//Gradient
#if ENABLED(GRADIENT_MIX)
	Draw_Menu_Line(MIXER_CASE_GRADIENT,ICON_MIXER_GRADIENT);
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_GRADIENT, LBLX, MBASE(MIXER_CASE_GRADIENT));
	Draw_More_Icon(MIXER_CASE_GRADIENT);
	#if HAS_ONOFF_ICON
	Draw_ONOFF_Icon(MIXER_CASE_GRADIENT, mixer.gradient.enabled, 160);
	#else
	DWIN_Draw_MaskString_Default(190, MBASE(MIXER_CASE_GRADIENT), F_STRING_ONOFF(mixer.gradient.enabled)); 
	#endif
#endif

	//Random	
#if ENABLED(RANDOM_MIX)	
	Draw_Menu_Line(MIXER_CASE_RANDOM,ICON_MIXER_RANDOM);
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_RANDOM, LBLX, MBASE(MIXER_CASE_RANDOM));	
	Draw_More_Icon(MIXER_CASE_RANDOM);
	#if HAS_ONOFF_ICON	
	Draw_ONOFF_Icon(MIXER_CASE_RANDOM, mixer.random_mix.enabled, 160);
	#else
	DWIN_Draw_MaskString_Default(190, MBASE(MIXER_CASE_RANDOM), F_STRING_ONOFF(mixer.random_mix.enabled));
	#endif
#endif

	if (DwinMenu_mixer.now) Draw_Menu_Cursor(DwinMenu_mixer.now);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Control >> Mixer >>> SetVtool
//
static void Item_Mixer_SetVtool_VTOOL(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_VTOOL2, LBLX, MBASE(row));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), HMI_Value.current_vtool);
	Draw_Menu_Line(row, ICON_S_VTOOL);
}

static void Item_Mixer_SetVtool_EXT1(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_EXTRUDER, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_1, LBLX+get_MultiLanguageString_Width(MTSTRING_EXTRUDER)+5, MBASE(row));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), HMI_Value.mix_percent[0]);
	Draw_Menu_Line(row, ICON_EXTRUDER1);
}

static void Item_Mixer_SetVtool_EXT2(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_EXTRUDER, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_2, LBLX+get_MultiLanguageString_Width(MTSTRING_EXTRUDER)+5, MBASE(row));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), HMI_Value.mix_percent[1]);
	Draw_Menu_Line(row, ICON_EXTRUDER2);

}

#if(MIXING_STEPPERS > 2)	
static void Item_Mixer_SetVtool_EXT3(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_EXTRUDER, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_3, LBLX+get_MultiLanguageString_Width(MTSTRING_EXTRUDER)+5, MBASE(row));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), HMI_Value.mix_percent[2]);
	Draw_Menu_Line(row, ICON_EXTRUDER3);
}
#endif

#if(MIXING_STEPPERS > 3)	
static void Item_Mixer_SetVtool_EXT4(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_EXTRUDER, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_4, LBLX+get_MultiLanguageString_Width(MTSTRING_EXTRUDER)+5, MBASE(row));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), HMI_Value.mix_percent[3]);
	Draw_Menu_Line(row, ICON_EXTRUDER4);
}
#endif

static void Item_Mixer_SetVtool_Commit(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_COMIT, LBLX, MBASE(row));	
	Draw_Menu_Line(row, ICON_C_VTOOL);
}

inline void Draw_Mixer_SetVtool_Menu() {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	DwinMenu_manualmix.reset();
	DwinMenuID = DWMENU_MIXER_SETVTOOL;

#if (MANUAL_CASE_TOTAL > MROWS)
	const int8_t scroll = MROWS - DwinMenu_manualmix.index; // Scrolled-up lines
	#define MCSCROL(L) (scroll + (L))
#else
	#define MCSCROL(L) (L)
#endif
	#define MCVISI(L) WITHIN(MCSCROL(L), 0, MROWS)

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_MIX, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
  // < Back
	if (MCVISI(SETVTOOL_CASE_BACK)) Draw_Back_First(DwinMenu_manualmix.now == SETVTOOL_CASE_BACK);
	//VTOOL
	if (MCVISI(SETVTOOL_CASE_VTOOL)) Item_Mixer_SetVtool_VTOOL(MCSCROL(SETVTOOL_CASE_VTOOL));
	//Extruder 1
	if (MCVISI(SETVTOOL_CASE_EXTRUDER1)) Item_Mixer_SetVtool_EXT1(MCSCROL(SETVTOOL_CASE_EXTRUDER1));
	//Extruder 2
	if (MCVISI(SETVTOOL_CASE_EXTRUDER2)) Item_Mixer_SetVtool_EXT2(MCSCROL(SETVTOOL_CASE_EXTRUDER2));
	//Extruder 3
	#if(MIXING_STEPPERS > 2)	
	if (MCVISI(SETVTOOL_CASE_EXTRUDER3)) Item_Mixer_SetVtool_EXT3(MCSCROL(SETVTOOL_CASE_EXTRUDER3));
	#endif
	//Extruder 4
	#if(MIXING_STEPPERS > 3)
	if (MCVISI(SETVTOOL_CASE_EXTRUDER4)) Item_Mixer_SetVtool_EXT4(MCSCROL(SETVTOOL_CASE_EXTRUDER4));
	#endif
	if (MCVISI(SETVTOOL_CASE_COMMIT)) Item_Mixer_SetVtool_Commit(MCSCROL(SETVTOOL_CASE_COMMIT));
			
	if (DwinMenu_manualmix.now) Draw_Menu_Cursor(MCSCROL(DwinMenu_manualmix.now));
}

void HMI_Adjust_Mixer_SetVtool_Vtool() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int8(encoder_diffState, &HMI_Value.current_vtool)) {			
			DWIN_Draw_IntValue_Default(2, MENUVALUE_X+16, MBASE(MROWS + SETVTOOL_CASE_VTOOL - DwinMenu_manualmix.index), HMI_Value.current_vtool);
			mixer.update_mix_from_vtool(HMI_Value.current_vtool);
			MIXER_STEPPER_LOOP(i) {HMI_Value.mix_percent[i] = mixer.percentmix[i];}
			if((MROWS + SETVTOOL_CASE_EXTRUDER1 - DwinMenu_manualmix.index) > SETVTOOL_CASE_BACK) 
				DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS + SETVTOOL_CASE_EXTRUDER1 - DwinMenu_manualmix.index), HMI_Value.mix_percent[0]);
			if((MROWS + SETVTOOL_CASE_EXTRUDER2 - DwinMenu_manualmix.index) > SETVTOOL_CASE_BACK) 
				DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS + SETVTOOL_CASE_EXTRUDER2 - DwinMenu_manualmix.index), HMI_Value.mix_percent[1]);
		#if (MIXING_STEPPERS > 2)
			if((MROWS + SETVTOOL_CASE_EXTRUDER3 - DwinMenu_manualmix.index) > SETVTOOL_CASE_BACK) 
				DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS + SETVTOOL_CASE_EXTRUDER3 - DwinMenu_manualmix.index), HMI_Value.mix_percent[2]);
		#endif
		#if (MIXING_STEPPERS > 3)
			if((MROWS + SETVTOOL_CASE_EXTRUDER4 - DwinMenu_manualmix.index) > SETVTOOL_CASE_BACK) 
				DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS + SETVTOOL_CASE_EXTRUDER4 - DwinMenu_manualmix.index), HMI_Value.mix_percent[3]);
		#endif		
			DwinMenuID = DWMENU_MIXER_SETVTOOL;
			EncoderRate.enabled = false;		
		}
		else {
			NOLESS(HMI_Value.current_vtool, 0);
			NOMORE(HMI_Value.current_vtool, (MIXING_VIRTUAL_TOOLS-1));
			DWIN_Draw_Select_IntValue_Default(2, MENUVALUE_X+16, MBASE(MROWS + SETVTOOL_CASE_VTOOL-DwinMenu_manualmix.index), HMI_Value.current_vtool);		
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_Ext_Percent(uint8_t Extruder_Number) {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int8(encoder_diffState, &HMI_Value.mix_percent[Extruder_Number])) {
			DwinMenuID = DWMENU_MIXER_SETVTOOL;
			EncoderRate.enabled = false;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS+SETVTOOL_CASE_EXTRUDER1+Extruder_Number-DwinMenu_manualmix.index), HMI_Value.mix_percent[Extruder_Number]);			
		}
		else{
			NOLESS(HMI_Value.mix_percent[Extruder_Number], 0);
			NOMORE(HMI_Value.mix_percent[Extruder_Number], 100);
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS+SETVTOOL_CASE_EXTRUDER1+Extruder_Number-DwinMenu_manualmix.index), HMI_Value.mix_percent[Extruder_Number]);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Mixer_SetVtool() { 
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {		
		if (DwinMenu_manualmix.inc(SETVTOOL_CASE_END)){
			if (DwinMenu_manualmix.now > MROWS && DwinMenu_manualmix.now > DwinMenu_manualmix.index) {
				DwinMenu_manualmix.index = DwinMenu_manualmix.now;				
				Scroll_Menu(DWIN_SCROLL_UP);
				if(DwinMenu_manualmix.index == SETVTOOL_CASE_COMMIT) Item_Mixer_SetVtool_Commit(MROWS);
				#if (MIXING_STEPPERS > 4)
				else if(DwinMenu_manualmix.index == SETVTOOL_CASE_EXTRUDER4) Item_Mixer_SetVtool_EXT4(MROWS);			
				#endif
			}
			else 
				Move_Highlight(1, DwinMenu_manualmix.now + MROWS - DwinMenu_manualmix.index);		
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_manualmix.dec()){
			if (DwinMenu_manualmix.now < DwinMenu_manualmix.index - MROWS) {
				DwinMenu_manualmix.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if(DwinMenu_manualmix.index - MROWS == SETVTOOL_CASE_BACK) Draw_Back_First();				
				else if(DwinMenu_manualmix.index - MROWS == SETVTOOL_CASE_EXTRUDER1) Item_Mixer_SetVtool_EXT1(0);
				else if(DwinMenu_manualmix.index - MROWS == SETVTOOL_CASE_VTOOL) Item_Mixer_SetVtool_VTOOL(0);
			}
			else 
				Move_Highlight(-1, DwinMenu_manualmix.now + MROWS - DwinMenu_manualmix.index);			
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_manualmix.now) {
			case SETVTOOL_CASE_BACK: 						// Back				
				Draw_Mixer_Menu(MIXER_CASE_SETVTOOL);
			break;

			case SETVTOOL_CASE_VTOOL: 	// Vtool
				DwinMenuID = DWMENU_SETVTOOL_VTOOL;
				DWIN_Draw_Select_IntValue_Default(2, MENUVALUE_X+16, MBASE(MROWS + SETVTOOL_CASE_VTOOL -DwinMenu_manualmix.index), HMI_Value.current_vtool);
				EncoderRate.enabled = true;
			break;
			
			case SETVTOOL_CASE_EXTRUDER1: 	// ex1
				DwinMenuID = DWMENU_MIXER_EXT1;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS + SETVTOOL_CASE_EXTRUDER1 -DwinMenu_manualmix.index), HMI_Value.mix_percent[0]);
				EncoderRate.enabled = true;
			break;
				
			case SETVTOOL_CASE_EXTRUDER2: 	// ex2
				DwinMenuID = DWMENU_MIXER_EXT2;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS + SETVTOOL_CASE_EXTRUDER2 -DwinMenu_manualmix.index), HMI_Value.mix_percent[1]);
				EncoderRate.enabled = true;
			break;

		#if(MIXING_STEPPERS > 2)
			case SETVTOOL_CASE_EXTRUDER3: 	// ex3
				DwinMenuID = DWMENU_MIXER_EXT3;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS + SETVTOOL_CASE_EXTRUDER3 -DwinMenu_manualmix.index), HMI_Value.mix_percent[2]);
				EncoderRate.enabled = true;
			break;
		#endif

		#if(MIXING_STEPPERS > 3) 
			case SETVTOOL_CASE_EXTRUDER4: 	// ex4
				DwinMenuID = DWMENU_MIXER_EXT4;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS + SETVTOOL_CASE_EXTRUDER4 -DwinMenu_manualmix.index), HMI_Value.mix_percent[3]);
				EncoderRate.enabled = true;
			break;
		#endif
				
			case SETVTOOL_CASE_COMMIT: 		// COMMIT				
				mixer.normalize_mixer_percent(&HMI_Value.mix_percent[0]);
				MIXER_STEPPER_LOOP(i) {mixer.percentmix[i] = HMI_Value.mix_percent[i];}
				mixer.copy_percentmix_to_collector();
				mixer.normalize(HMI_Value.current_vtool);						
				Draw_Mixer_SetVtool_Menu();
			break;
		 
			default: break;
		}
	}
	dwinLCD.UpdateLCD();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Control >> Mixer >>> Gradient
//
#if ENABLED(GRADIENT_MIX)
static void Item_Mixer_Gradient_startZ(const uint8_t row) {
	HMI_Value.Auto_Zstart_scale = mixer.gradient.start_z*MINUNITMULT;
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_START, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z, LBLX+get_MultiLanguageString_Width(MTSTRING_MIX_START)+6, MBASE(row));	
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), HMI_Value.Auto_Zstart_scale);
	Draw_Menu_Line(row, ICON_ZPOS_RISE);
}

static void Item_Mixer_Gradient_endZ(const uint8_t row) {
	HMI_Value.Auto_Zend_scale = mixer.gradient.end_z*MINUNITMULT;
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_END, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z, LBLX+get_MultiLanguageString_Width(MTSTRING_MIX_END)+6, MBASE(row));	
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), HMI_Value.Auto_Zend_scale);
	Draw_Menu_Line(row, ICON_ZPOS_DROP);
}

static void Item_Mixer_Gradient_startVtool(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_START, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_VTOOL2, LBLX+get_MultiLanguageString_Width(MTSTRING_MIX_START)+6, MBASE(row));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), mixer.gradient.start_vtool);
	Draw_Menu_Line(row, ICON_VTOOL_RISE);

}

static void Item_Mixer_Gradient_EndVtool(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_END, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_VTOOL2, LBLX+get_MultiLanguageString_Width(MTSTRING_MIX_END)+6, MBASE(row));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), mixer.gradient.end_vtool);
	Draw_Menu_Line(row, ICON_VTOOL_DROP);

}

static void Item_Mixer_Gradient_Stop(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_STOP, LBLX, MBASE(row)); 
	Draw_Menu_Line(row, ICON_CURSOR);
}

static void Draw_Mixer_Gradient_Menu() {
	DwinMenuID = DWMENU_MIXER_GRADIENT;	
	DwinMenu_GradientMix.reset();
	mixer.gradient.enabled = false;
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);	
#if (GRADIENT_CASE_TOTAL > MROWS)
	const int8_t scroll = MROWS - DwinMenu_GradientMix.index; // Scrolled-up lines
	#define ACSCROL(L) (scroll + (L))
#else
	#define ACSCROL(L) (L)
#endif
	#define ACLINE(L) MBASE(ACSCROL(L))
	#define ACVISI(L) WITHIN(ACSCROL(L), 0, MROWS)

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_GRADIENT, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	if(ACVISI(GRADIENT_CASE_BACK)) Draw_Back_First(DwinMenu_GradientMix.now == GRADIENT_CASE_BACK);  // < Back
	//Start Z
	if(ACVISI(GRADIENT_CASE_ZPOS_START)) Item_Mixer_Gradient_startZ(ACSCROL(GRADIENT_CASE_ZPOS_START));	
	//End Z
	if(ACVISI(GRADIENT_CASE_ZPOS_END)) Item_Mixer_Gradient_endZ(ACSCROL(GRADIENT_CASE_ZPOS_END));
	//Start VTool
	if(ACVISI(GRADIENT_CASE_VTOOL_START)) Item_Mixer_Gradient_startVtool(ACSCROL(GRADIENT_CASE_VTOOL_START));
	//End VTool
	if(ACVISI(GRADIENT_CASE_VTOOL_END)) Item_Mixer_Gradient_EndVtool(ACSCROL(GRADIENT_CASE_VTOOL_END));
	//Stop
	if(ACVISI(GRADIENT_CASE_STOP)) Item_Mixer_Gradient_Stop(ACSCROL(GRADIENT_CASE_STOP));
	  
 	if (DwinMenu_GradientMix.now) Draw_Menu_Cursor(DwinMenu_GradientMix.now);
}

void HMI_Adjust_Gradient_Zpos_Start() {
	static int16_t last_Z_scale = 0;
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();

	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Auto_Zstart_scale)) {			
			EncoderRate.enabled = false;
			last_Z_scale = HMI_Value.Auto_Zstart_scale;
			DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_ZPOS_START), HMI_Value.Auto_Zstart_scale);
			DwinMenuID = DWMENU_MIXER_GRADIENT;			
			mixer.gradient.start_z = (float)HMI_Value.Auto_Zstart_scale / MINUNITMULT;			
	  }
		else{
		  if ((HMI_Value.Auto_Zstart_scale - last_Z_scale) > (Z_MAX_POS) * MINUNITMULT)
				HMI_Value.Auto_Zstart_scale = last_Z_scale + (Z_MAX_POS) * MINUNITMULT;
		  else if ((last_Z_scale - HMI_Value.Auto_Zstart_scale) > (Z_MAX_POS) * MINUNITMULT)
				HMI_Value.Auto_Zstart_scale = last_Z_scale - (Z_MAX_POS) * MINUNITMULT;		
			NOLESS(HMI_Value.Auto_Zstart_scale, 0);
			NOMORE(HMI_Value.Auto_Zstart_scale, Z_MAX_POS*MINUNITMULT);
			DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_ZPOS_START), HMI_Value.Auto_Zstart_scale);		
		}			
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_Gradient_Zpos_End() {
	static int16_t last_Z_scale = 0;
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();

	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Auto_Zend_scale)) {			
			EncoderRate.enabled = false;
			last_Z_scale = HMI_Value.Auto_Zend_scale;
			DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_ZPOS_END), HMI_Value.Auto_Zend_scale);
			DwinMenuID = DWMENU_MIXER_GRADIENT;
			mixer.gradient.end_z = (float)HMI_Value.Auto_Zend_scale / MINUNITMULT;			
	  }		
		else {
		  if ((HMI_Value.Auto_Zend_scale - last_Z_scale) > (Z_MAX_POS) * MINUNITMULT)
				HMI_Value.Auto_Zend_scale = last_Z_scale + (Z_MAX_POS) * MINUNITMULT;
		  else if ((last_Z_scale - HMI_Value.Auto_Zend_scale) > (Z_MAX_POS) * MINUNITMULT)
				HMI_Value.Auto_Zend_scale = last_Z_scale - (Z_MAX_POS) * MINUNITMULT;
			
			NOLESS(HMI_Value.Auto_Zend_scale, 0);	
			NOMORE(HMI_Value.Auto_Zend_scale, Z_MAX_POS*MINUNITMULT);	
			DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_ZPOS_END), HMI_Value.Auto_Zend_scale);		
		}			 
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_Gradient_VTool_Start() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int8(encoder_diffState, &mixer.gradient.start_vtool)) {			
			EncoderRate.enabled = false;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_VTOOL_START), mixer.gradient.start_vtool);			
			DwinMenuID = DWMENU_MIXER_GRADIENT;
		}
		else {
			NOLESS(mixer.gradient.start_vtool, 0);
			NOMORE(mixer.gradient.start_vtool, MIXING_VIRTUAL_TOOLS-1);
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_VTOOL_START), mixer.gradient.start_vtool);
		}			
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_Gradient_VTool_End() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int8(encoder_diffState,&mixer.gradient.end_vtool)) {			
			EncoderRate.enabled = false;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_VTOOL_END), mixer.gradient.end_vtool);			
			DwinMenuID = DWMENU_MIXER_GRADIENT;
		}
		else {
			NOLESS(mixer.gradient.end_vtool, 0);
			NOMORE(mixer.gradient.end_vtool, MIXING_VIRTUAL_TOOLS-1);
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_VTOOL_END), mixer.gradient.end_vtool);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Mixer_Gradient() {	
 	ENCODER_DiffState encoder_diffState = get_encoder_state();
 	if (encoder_diffState == ENCODER_DIFF_NO) return;
 
 	// Avoid flicker by updating only the previous menu
 	if (encoder_diffState == ENCODER_DIFF_CW) {
  	if (DwinMenu_GradientMix.inc(GRADIENT_CASE_END)) {
   		if (DwinMenu_GradientMix.now > MROWS && DwinMenu_GradientMix.now > DwinMenu_GradientMix.index) {
    		DwinMenu_GradientMix.index = DwinMenu_GradientMix.now;
				Scroll_Menu(DWIN_SCROLL_UP);
   		}
   		else 
    		Move_Highlight(1, DwinMenu_GradientMix.now + MROWS - DwinMenu_GradientMix.index);
		}	
	}
 	else if (encoder_diffState == ENCODER_DIFF_CCW) {
  	if (DwinMenu_GradientMix.dec()) {
   		if (DwinMenu_GradientMix.now < DwinMenu_GradientMix.index - MROWS) {
    		DwinMenu_GradientMix.index--;
    		Scroll_Menu(DWIN_SCROLL_DOWN);
   		}
   		else 
    		Move_Highlight(-1, DwinMenu_GradientMix.now + MROWS - DwinMenu_GradientMix.index);   
  	}
 }
 else if (encoder_diffState == ENCODER_DIFF_ENTER) {
  switch (DwinMenu_GradientMix.now) {
	 case 0: 						// Back	 	 	
	 	mixer.refresh_gradient();
    Draw_Mixer_Menu(MIXER_CASE_GRADIENT);
    break;
		
	 case GRADIENT_CASE_ZPOS_START: 		// zpos_start
    DwinMenuID = DWMENU_MIXER_GRADIENT_ZSTART;
		HMI_Value.Auto_Zstart_scale = mixer.gradient.start_z*MINUNITMULT;
		DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_ZPOS_START), HMI_Value.Auto_Zstart_scale);
	  EncoderRate.enabled = true;
    break;
		
	 case GRADIENT_CASE_ZPOS_END: 		// zpos_end
    DwinMenuID = DWMENU_MIXER_GRADIENT_ZEND;
		HMI_Value.Auto_Zstart_scale = mixer.gradient.end_z*MINUNITMULT;
		DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_ZPOS_END), HMI_Value.Auto_Zend_scale);
	  EncoderRate.enabled = true;
    break;
		
	 case GRADIENT_CASE_VTOOL_START: 		// vtool_start
    DwinMenuID = DWMENU_MIXER_GRADIENT_TSTAR;
    DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_VTOOL_START), mixer.gradient.start_vtool);
		EncoderRate.enabled = true;
    break;
		
	 case GRADIENT_CASE_VTOOL_END: 		// vtool_end
    DwinMenuID = DWMENU_MIXER_GRADIENT_TEND;
		DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_GradientMix.index + GRADIENT_CASE_VTOOL_END), mixer.gradient.end_vtool);
		EncoderRate.enabled = true;
    break;

	 case GRADIENT_CASE_STOP: 		// STOP
		mixer.gradientmix_reset();		
		Draw_Mixer_Gradient_Menu();		
    break;
		
   default: break;
  }
 }
 dwinLCD.UpdateLCD();
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Control >> Mixer >>> Random
//
#if ENABLED(RANDOM_MIX)
static void Item_Mixer_Random_startZ(const uint8_t row) {
	HMI_Value.Random_Zstart_scale = mixer.random_mix.start_z*MINUNITMULT;
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_START, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z, LBLX+get_MultiLanguageString_Width(MTSTRING_MIX_START)+6, MBASE(row));		
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), HMI_Value.Random_Zstart_scale);
	Draw_Menu_Line(row, ICON_ZPOS_RISE);
}

static void Item_Mixer_Random_endZ(const uint8_t row) {
	HMI_Value.Random_Zend_scale = mixer.random_mix.end_z*MINUNITMULT;
	DWIN_Show_MultiLanguage_String(MTSTRING_MIX_END, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z, LBLX+get_MultiLanguageString_Width(MTSTRING_MIX_END)+6, MBASE(row));	
	DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), HMI_Value.Random_Zend_scale);
	Draw_Menu_Line(row, ICON_ZPOS_DROP);
}

static void Item_Mixer_Random_Height(const uint8_t row) {
#if (DWINLCD_MENU_VERSION >= 2)	
		DWIN_Show_MultiLanguage_String(MTSTRING_RANDOM_HEIGTH, LBLX, MBASE(row));
#else
		DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Height"));
#endif
		HMI_Value.Random_Height = mixer.random_mix.height*MINUNITMULT;	
		DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(row), HMI_Value.Random_Height);
		Draw_Menu_Line(row, ICON_CURSOR);
}

static void Item_Mixer_Random_Extruders(const uint8_t row) {
#if (DWINLCD_MENU_VERSION >= 2)		
	DWIN_Show_MultiLanguage_String(MTSTRING_RANDOM_EXTRUDERS, LBLX, MBASE(row));
#else
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Extruders"));
#endif	
	DWIN_Draw_IntValue_Default(1, MENUVALUE_X+4*8, MBASE(row), mixer.random_mix.extruders);
	Draw_Menu_Line(row, ICON_CURSOR);
}

static void Item_Mixer_Random_Stop(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_STOP, LBLX, MBASE(row)); 
	Draw_Menu_Line(row, ICON_CURSOR);
}

static void Draw_Mixer_Random_Menu() {
	DwinMenuID = DWMENU_MIXER_RANDOM;
	DwinMenu_RandomMix.reset();
	mixer.random_mix.enabled = false;
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	
#if (RANDOM_CASE_TOTAL > MROWS)
	const int8_t scroll = MROWS - DwinMenu_RandomMix.index; // Scrolled-up lines
	#define ACSCROL(L) (scroll + (L))
#else
	#define ACSCROL(L) (L)
#endif
	#define ACLINE(L) MBASE(ACSCROL(L))
	#define ACVISI(L) WITHIN(ACSCROL(L), 0, MROWS)

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_RANDOM, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);	
	
	if(ACVISI(RANDOM_CASE_BACK)) Draw_Back_First(DwinMenu_RandomMix.now == RANDOM_CASE_BACK); // < Back	
	//Z start
	if(ACVISI(RANDOM_CASE_ZPOS_START)) Item_Mixer_Random_startZ(ACSCROL(RANDOM_CASE_ZPOS_START));	
	//Z END
	if(ACVISI(RANDOM_CASE_ZPOS_END)) Item_Mixer_Random_endZ(ACSCROL(RANDOM_CASE_ZPOS_END));
	//Height		
	if(ACVISI(RANDOM_CASE_HEIGHT)) Item_Mixer_Random_Height(ACSCROL(RANDOM_CASE_HEIGHT));	
	//Extruders	
	if(ACVISI(RANDOM_CASE_EXTRUDERS)) Item_Mixer_Random_Extruders(ACSCROL(RANDOM_CASE_EXTRUDERS));
	//Stop
	if(ACVISI(RANDOM_CASE_STOP)) Item_Mixer_Random_Stop(ACSCROL(RANDOM_CASE_STOP));
	//
 	if (DwinMenu_RandomMix.now) Draw_Menu_Cursor(DwinMenu_RandomMix.now); 
}

void HMI_Adjust_Random_Zpos_Start() {
	static int16_t last_Z_scale = 0;
	
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Random_Zstart_scale)) {			
			EncoderRate.enabled = false;
			last_Z_scale = HMI_Value.Random_Zstart_scale;
			DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_ZPOS_START), HMI_Value.Random_Zstart_scale);			
			mixer.random_mix.start_z = (float)HMI_Value.Random_Zstart_scale / MINUNITMULT;
			DwinMenuID = DWMENU_MIXER_RANDOM;			
	  }		
		else {
		  if ((HMI_Value.Random_Zstart_scale - last_Z_scale) > (Z_MAX_POS) * MINUNITMULT)
				HMI_Value.Random_Zstart_scale = last_Z_scale + (Z_MAX_POS) * MINUNITMULT;
		  else if ((last_Z_scale - HMI_Value.Random_Zstart_scale) > (Z_MAX_POS) * MINUNITMULT)
				HMI_Value.Random_Zstart_scale = last_Z_scale - (Z_MAX_POS) * MINUNITMULT;
			
			NOLESS(HMI_Value.Random_Zstart_scale, 0);		
			DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_ZPOS_START), HMI_Value.Random_Zstart_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_Random_Zpos_End() {
	static int16_t last_Z_scale = 0;
	
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Random_Zend_scale)) {			
			EncoderRate.enabled = false;
			last_Z_scale = HMI_Value.Random_Zend_scale;
			DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_ZPOS_END), HMI_Value.Random_Zend_scale);			
			mixer.random_mix.end_z = (float)HMI_Value.Random_Zend_scale / MINUNITMULT;
			DwinMenuID = DWMENU_MIXER_RANDOM;
	  }
		else {
		  if ((HMI_Value.Random_Zend_scale - last_Z_scale) > (Z_MAX_POS) * MINUNITMULT)
				HMI_Value.Random_Zend_scale = last_Z_scale + (Z_MAX_POS) * MINUNITMULT;
		  else if ((last_Z_scale - HMI_Value.Random_Zend_scale) > (Z_MAX_POS) * MINUNITMULT)
				HMI_Value.Random_Zend_scale = last_Z_scale - (Z_MAX_POS) * MINUNITMULT;
			
		   NOLESS(HMI_Value.Random_Zend_scale, 0);		 
			 DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_ZPOS_END), HMI_Value.Random_Zend_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_Random_Height() {
	static float last_Height = 0;
	
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Random_Height)) {			
			EncoderRate.enabled = false;
			last_Height = HMI_Value.Random_Height;
			DWIN_Draw_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_HEIGHT), HMI_Value.Random_Height);			
			DwinMenuID = DWMENU_MIXER_RANDOM;
	  }
		else {
			if ((HMI_Value.Random_Height - last_Height) > 100 * MINUNITMULT)
				HMI_Value.Random_Height = last_Height + 100 * MINUNITMULT;
			else if ((last_Height - HMI_Value.Random_Height) > Z_MAX_POS * MINUNITMULT)
				HMI_Value.Random_Height = last_Height - Z_MAX_POS * MINUNITMULT;
			NOLESS(HMI_Value.Random_Height, 1);
			
			NOMORE(HMI_Value.Random_Height, Z_MAX_POS * MINUNITMULT);
			mixer.random_mix.height = (float)HMI_Value.Random_Height / MINUNITMULT;
			DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_HEIGHT), HMI_Value.Random_Height);
		}
		dwinLCD.UpdateLCD();
	}
}


void HMI_Adjust_Random_Extruders() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_uint8(encoder_diffState, &mixer.random_mix.extruders)) {			
			EncoderRate.enabled = false;
			DWIN_Draw_IntValue_Default(1, MENUVALUE_X+4*8, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_EXTRUDERS), mixer.random_mix.extruders);			
			DwinMenuID = DWMENU_MIXER_RANDOM;
		}
		else {
			NOLESS(mixer.random_mix.extruders, 1);
			NOMORE(mixer.random_mix.extruders, MIXING_STEPPERS);		
			DWIN_Draw_Select_IntValue_Default(1, MENUVALUE_X+4*8, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_EXTRUDERS), mixer.random_mix.extruders);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Mixer_Random() {
 ENCODER_DiffState encoder_diffState = get_encoder_state();
 if (encoder_diffState == ENCODER_DIFF_NO) return;
 
 // Avoid flicker by updating only the previous menu
 if (encoder_diffState == ENCODER_DIFF_CW) {
  if (DwinMenu_RandomMix.inc(RANDOM_CASE_END)) {
   if (DwinMenu_RandomMix.now > MROWS && DwinMenu_RandomMix.now > DwinMenu_RandomMix.index) {
    DwinMenu_RandomMix.index = DwinMenu_RandomMix.now;
		Scroll_Menu(DWIN_SCROLL_UP);
   }
   else
    Move_Highlight(1, DwinMenu_RandomMix.now + MROWS - DwinMenu_RandomMix.index);
  }
 }
 else if (encoder_diffState == ENCODER_DIFF_CCW) {
  if (DwinMenu_RandomMix.dec()) {
   if (DwinMenu_RandomMix.now < DwinMenu_RandomMix.index - MROWS) {
    DwinMenu_RandomMix.index--;
    Scroll_Menu(DWIN_SCROLL_DOWN);
   }
   else
    Move_Highlight(-1, DwinMenu_RandomMix.now + MROWS - DwinMenu_RandomMix.index);
  }
 }
 else if (encoder_diffState == ENCODER_DIFF_ENTER) {
  switch (DwinMenu_RandomMix.now) {
	 case 0: 						// Back
	 	mixer.refresh_random_mix();		
    Draw_Mixer_Menu(MIXER_CASE_RANDOM);
   break;
		
	 case RANDOM_CASE_ZPOS_START: 		// zpos_start
    DwinMenuID = DWMENU_MIXER_RANDOM_ZSTART;
		HMI_Value.Random_Zstart_scale = mixer.random_mix.start_z*MINUNITMULT;
		DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_ZPOS_START), HMI_Value.Random_Zstart_scale);
	  EncoderRate.enabled = true;
   break;
	 
	 case RANDOM_CASE_ZPOS_END: 			// zpos_end
    DwinMenuID = DWMENU_MIXER_RANDOM_ZEND;
		HMI_Value.Random_Zend_scale = mixer.random_mix.end_z*MINUNITMULT;
		DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_ZPOS_END), HMI_Value.Random_Zend_scale);
	  EncoderRate.enabled = true;
   break;
	 
  case RANDOM_CASE_HEIGHT: 		// Height
    DwinMenuID = DWMENU_MIXER_RANDOM_HEIGHT;
		HMI_Value.Random_Height = mixer.random_mix.height*MINUNITMULT;
		DWIN_Draw_Selected_Small_Float31(MENUVALUE_X, MBASE(MROWS -DwinMenu_RandomMix.index + RANDOM_CASE_HEIGHT), HMI_Value.Random_Height);
	  EncoderRate.enabled = true;	
   break;
	 
  case RANDOM_CASE_EXTRUDERS: // Extruders
    DwinMenuID = DWMENU_MIXER_RANDOM_EXTN;
		DWIN_Draw_Select_IntValue_Default(1, MENUVALUE_X+4*8, MBASE(RANDOM_CASE_EXTRUDERS), mixer.random_mix.extruders);
	  EncoderRate.enabled = true;
   break;

	case RANDOM_CASE_STOP: 		// Stop		
		mixer.randommix_reset();
		Draw_Mixer_Random_Menu();
    break;
	 
   default: break;
  }
 }
 dwinLCD.UpdateLCD();
}
#endif

void HMI_Adjust_Mixer_Vtool() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int8(encoder_diffState, &HMI_Value.current_vtool)) {
			DwinMenuID = DWMENU_MIXER;
			EncoderRate.enabled = false;			
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MIXER_CASE_CURRENTVTOOL), HMI_Value.current_vtool);	
			mixer.selected_vtool = HMI_Value.current_vtool;
			mixer.update_mix_from_vtool(mixer.selected_vtool);			
		}
		else {
			NOLESS(HMI_Value.current_vtool, 0);
			NOMORE(HMI_Value.current_vtool, (MIXING_VIRTUAL_TOOLS-1));
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MIXER_CASE_CURRENTVTOOL), HMI_Value.current_vtool);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Mixer() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_mixer.inc(MIXER_CASE_END)) Move_Highlight(1, DwinMenu_mixer.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_mixer.dec()) Move_Highlight(-1, DwinMenu_mixer.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER){
		switch (DwinMenu_mixer.now){
			case 0: 																// Back
			if(IS_SD_PRINTING() || IS_SD_PAUSED() || IS_SD_FILE_OPEN()){
				HMI_flag.show_mode = SHOWED_TUNE;				
				Draw_Tune_Menu(TUNE_CASE_MIXER);
			}
			else{
				Draw_Control_Menu(CONTROL_CASE_MIXER);				
			}
			break;

			case MIXER_CASE_SETVTOOL: 	// SET VTOOL			
				HMI_Value.current_vtool = 0;
				mixer.update_mix_from_vtool(HMI_Value.current_vtool);
				MIXER_STEPPER_LOOP(i) {HMI_Value.mix_percent[i] = mixer.percentmix[i];}				
				Draw_Mixer_SetVtool_Menu();
			break;

			case MIXER_CASE_CURRENTVTOOL: 	// Current vtool
				if(mixer.gradient.enabled || mixer.random_mix.enabled){
					DWIN_FEEDBACK_WARNNING();
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Turn off Gradient/Random first!"));
					break;
				}
				DwinMenuID = DWMENU_MIXER_VTOOL;
				HMI_Value.current_vtool = mixer.selected_vtool;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MIXER_CASE_CURRENTVTOOL), HMI_Value.current_vtool);
				EncoderRate.enabled = true;
			break;
			
		#if ENABLED(GRADIENT_MIX)
			case MIXER_CASE_GRADIENT:  	// Gradient mix				
				//mixer.selected_vtool = mixer.gradient.start_vtool;
				//mixer.update_mix_from_vtool();
				Draw_Mixer_Gradient_Menu();
			break;
		#endif
		
		#if ENABLED(RANDOM_MIX)
			case MIXER_CASE_RANDOM: 	// Random  mix				
				Draw_Mixer_Random_Menu();
			break;
		#endif
		
			default: break;
		}
	}
	dwinLCD.UpdateLCD();
}
#endif //MIXING_EXTRUDER

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Control >> Configure
//
#if ENABLED(FWRETRACT) 
static void Item_Config_Retract(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Auto Retract"));
	Draw_Menu_Line(row,ICON_CURSOR);
	Draw_More_Icon(row);
}
#endif

#if ENABLED(LIN_ADVANCE) 
#if EXTRUDERS > 1
static void Item_Config_LinearAdvance(const uint8_t row) {	
	DWIN_Draw_MaskString_Default_Color(COLOR_WHITE, LBLX, MBASE(row), PSTR("Linear Advance"));
	Draw_Menu_Line(row,ICON_CURSOR);
	Draw_More_Icon(row);
}
#else
static void Item_Config_LinearAdvance(const uint8_t row) {	
	DWIN_Draw_MaskString_Default_Color(COLOR_WHITE, LBLX, MBASE(row), PSTR("Linear Advance K"));
	Draw_Menu_Line(row,ICON_CURSOR);
	HMI_Value.extruder_advance_K[0] = (int16_t)(planner.extruder_advance_K[0]*MAXUNITMULT);
	DWIN_Draw_Small_Float22(MENUVALUE_X-16, MBASE(row), HMI_Value.extruder_advance_K[0]);	
}
#endif
#endif 

#if ENABLED(OPTION_REPEAT_PRINTING) 
static void Item_Config_Reprint(const uint8_t row){
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row), PSTR("Repeat print"));
	Draw_Menu_Line(row,ICON_CURSOR);
	Draw_More_Icon(row);
}
#endif

#if ENABLED(FILAMENT_RUNOUT_SENSOR)
static void Item_Config_FilamentRunOut(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Runout Sensor"));
	Draw_Menu_Line(row,ICON_CURSOR);	
#if HAS_ONOFF_ICON		
	Draw_ONOFF_Icon(row, runout.enabled);
#else
	DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(row), F_STRING_ONOFF(runout.enabled));	
#endif
}
#endif

#if ENABLED(OPTION_ABORT_UNLOADFILAMENT)
static void Item_Config_FilamentAutoUnload(const uint8_t row) {	
	DWIN_Draw_MaskString_Default_Color((mixer.mixing_enabled?COLOR_GREY : COLOR_WHITE), LBLX, MBASE(row), PSTR("Auto Unload"));
	Draw_Menu_Line(row,ICON_CURSOR);		
#if HAS_ONOFF_ICON		
	Draw_ONOFF_Icon(row, HMI_flag.AutoUnload_enabled);
#else
	DWIN_Draw_MaskString_Default_Color((mixer.mixing_enabled?COLOR_GREY : COLOR_WHITE),MENUONOFF_X, MBASE(row), F_STRING_ONOFF(HMI_flag.AutoUnload_enabled));	
#endif
}
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
static void Item_Config_Powerloss(const uint8_t row) {
	Draw_Menu_Line(row,ICON_CURSOR);
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row), PSTR("Power Loss Recovery"));		
#if HAS_ONOFF_ICON	
	Draw_ONOFF_Icon(row, recovery.enabled);
#else
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, MENUONOFF_X, MBASE(row), F_STRING_ONOFF(recovery.enabled));	
#endif
}
#endif

#if ENABLED(OPTION_AUTOPOWEROFF)
static void Item_Config_Shutdown(const uint8_t row) {
	Draw_Menu_Line(row,ICON_CURSOR);
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Auto Shutdown"));		
#if HAS_ONOFF_ICON	
	Draw_ONOFF_Icon(row, HMI_flag.Autoshutdown_enabled);
#else
  DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(row), F_STRING_ONOFF(HMI_flag.Autoshutdown_enabled));
#endif
}
#endif

#if ENABLED(CASE_LIGHT_MENU)
static void Item_Config_Backlight(const uint8_t row) {
	Draw_Menu_Line(row,ICON_CURSOR);
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Backlight"));
#if ENABLED(CASE_LIGHT_NO_BRIGHTNESS)
	#if HAS_ONOFF_ICON
		Draw_ONOFF_Icon(row, caselight.on);
	#else
		DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(row), F_STRING_ONOFF(caselight.on));
	#endif
#else
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), caselight.brightness);
#endif
}
#endif


#if ENABLED(OPTION_WIFI_MODULE)
static void Item_Config_Wifi(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row),PSTR("WiFi"));
	Draw_Menu_Line(row,ICON_CURSOR); 
#if HAS_ONOFF_ICON 
	Draw_ONOFF_Icon(row, WiFi_Enabled);
#else
	DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(row), F_STRING_ONOFF(WiFi_Enabled)); 
#endif
}

#if ENABLED(OPTION_WIFI_BAUDRATE)
static void Item_Config_WifiBaudrate(const uint8_t row) {
	Draw_Menu_Line(row,ICON_CURSOR);
	if(WiFi_Enabled){
		DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row),PSTR("WiFi BaudRate"));
		DWIN_Draw_IntValue_Default_Color(COLOR_FLEX_ITEM, 6, MENUVALUE_X-16, MBASE(row), Table_Baudrate[WiFi_BaudRate]);
	}
	else{
		DWIN_Draw_MaskString_Default_Color(COLOR_GREY, LBLX, MBASE(row),PSTR("WiFi BaudRate"));
		DWIN_Draw_IntValue_Default_Color(COLOR_GREY, 6, MENUVALUE_X-16, MBASE(row), Table_Baudrate[WiFi_BaudRate]);
	}	
}
#endif
#endif

#if ENABLED(OPTION_LASER)
static void Item_Config_Laser(const uint8_t row) {
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row),PSTR("Laser"));
	Draw_Menu_Line(row,ICON_CURSOR);	
#if HAS_ONOFF_ICON  
	Draw_ONOFF_Icon(row, Laser_Enabled);
#else
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, MENUONOFF_X, MBASE(row), F_STRING_ONOFF(Laser_Enabled)); 
#endif
}
#endif

#if ENABLED(SPINDLE_FEATURE)
static void Item_Config_Spindle(const uint8_t row) {
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row),PSTR("Spindle"));
	Draw_Menu_Line(row,ICON_CURSOR);	
#if HAS_ONOFF_ICON  
	Draw_ONOFF_Icon(row, cutter.Spindle_Enabled);
#else
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, MENUONOFF_X, MBASE(row), F_STRING_ONOFF(cutter.Spindle_Enabled)); 
#endif
}
#endif


#if BOTH(MIXING_EXTRUDER, OPTION_MIXING_SWITCH)
static void Item_Config_MixerEnable(const uint8_t row) {
  Draw_Menu_Line(row,ICON_CURSOR);
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row), PSTR("Hotend Type"));
 	if(mixer.mixing_enabled)
		DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBRX-strlen("    Mix")*8, MBASE(row), PSTR("    Mix")); 
	else
		DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBRX-strlen("Non-mix")*8, MBASE(row), PSTR("Non-mix"));  	
}

#if ENABLED(SWITCH_EXTRUDER_MENU)
inline PGM_P _getString_SwitchExtruder(){
	switch(HMI_Value.switchExtruder){
		default:
			HMI_Value.switchExtruder = SE_DEFAULT;
	#if (MIXING_STEPPERS== 2)
		case SE_E1E2:
			return PSTR("E1-E2");
		case SE_E2E1:
			return PSTR("E2-E1");
	#elif (MIXING_STEPPERS== 3)
		case SE_E1E2E3:
			return PSTR("E1-E2-E3");
		case SE_E1E3E2:
			return PSTR("E1-E3-E4");
		case SE_E2E1E3:
			return PSTR("E2-E1-E3");
		case SE_E2E3E1:
			return PSTR("E2-E3-E1");
		case SE_E3E1E2:
			return PSTR("E3-E1-E2");
		case SE_E3E2E1:
			return PSTR("E3-E2-E1");			
	#elif (MIXING_STEPPERS == 4)
		case SE_E1E2E3E4:
			return PSTR("E1-E2-E3-E4");
		case SE_E1E2E4E3:
			return PSTR("E1-E2-E4-E3");
		case SE_E1E3E2E4:
			return PSTR("E1-E3-E2-E4");
		case SE_E1E3E4E2:
			return PSTR("E1-E3-E4-E2");
		case SE_E1E4E2E3:
			return PSTR("E1-E4-E2-E3");
		case SE_E1E4E3E2:
			return PSTR("E1-E4-E3-E2");
			
		case SE_E2E1E3E4:
			return PSTR("E2-E1-E3-E4");
		case SE_E2E1E4E3:
			return PSTR("E2-E1-E4-E3");
		case SE_E2E3E1E4:
			return PSTR("E2-E3-E1-E4");
		case SE_E2E3E4E1:
			return PSTR("E2-E3-E4-E1");
		case SE_E2E4E1E3:
			return PSTR("E2-E4-E1-E3");
		case SE_E2E4E3E1:
			return PSTR("E2-E4-E3-E1");
			
		case SE_E3E1E2E4:
			return PSTR("E3-E1-E2-E4");
		case SE_E3E1E4E2:
			return PSTR("E3-E1-E4-E2");				
		case SE_E3E2E1E4:
			return PSTR("E3-E2-E1-E4");
		case SE_E3E2E4E1:
			return PSTR("E3-E2-E4-E1");	
		case SE_E3E4E1E2:
			return PSTR("E3-E4-E1-E2");
		case SE_E3E4E2E1:
			return PSTR("E3-E4-E2-E1");	
			
		case SE_E4E1E2E3:
			return PSTR("E4-E1-E2-E3");	
		case SE_E4E1E3E2:
			return PSTR("E4-E1-E3-E2");	
		case SE_E4E2E1E3:
			return PSTR("E4-E2-E1-E3");
		case SE_E4E2E3E1:
			return PSTR("E4-E2-E3-E1");
		case SE_E4E3E1E2:
			return PSTR("E4-E3-E1-E2");
		case SE_E4E3E2E1:
			return PSTR("E4-E3-E2-E1");		
	#endif
	}
}

#define MIX_SWITCH(FROM, TO)	do{mixer.reset_collector(FROM); mixer.normalize(TO);}while(0)
inline void switch_extruder_sequence(){
	
	switch(HMI_Value.switchExtruder){
		default:
			HMI_Value.switchExtruder = SE_DEFAULT;
			
	#if (MIXING_STEPPERS == 2)	
		case SE_E1E2:
			MIX_SWITCH(0, 0);
			MIX_SWITCH(1, 1);
			break;
		case SE_E2E1:
			MIX_SWITCH(0, 1);
			MIX_SWITCH(1, 0);
			break;
	#elif (MIXING_STEPPERS == 3)
		case SE_E1E2E3:
			MIX_SWITCH(0, 0);
			MIX_SWITCH(1, 1);
			MIX_SWITCH(2, 2);
			break;
		case SE_E1E3E2:
			MIX_SWITCH(0, 0);
			MIX_SWITCH(2, 1);
			MIX_SWITCH(1, 2);
			break;			
		case SE_E2E1E3:		
			MIX_SWITCH(1, 0);
			MIX_SWITCH(0, 1);
			MIX_SWITCH(2, 2);
			break;			
		case SE_E2E3E1:
			MIX_SWITCH(1, 0);
			MIX_SWITCH(2, 1);
			MIX_SWITCH(0, 2);
			break;			
		case SE_E3E1E2:
			MIX_SWITCH(2, 0);
			MIX_SWITCH(0, 1);
			MIX_SWITCH(1, 2);
			break;			
		case SE_E3E2E1:
			MIX_SWITCH(2, 0);
			MIX_SWITCH(1, 1);
			MIX_SWITCH(0, 2);	
			break;			
	#elif (MIXING_STEPPERS == 4)		
		case SE_E1E2E3E4:
			MIX_SWITCH(0, 0);
			MIX_SWITCH(1, 1);
			MIX_SWITCH(2, 2);
			MIX_SWITCH(3, 3);
			break;
		case SE_E1E2E4E3:
			MIX_SWITCH(0, 0);
			MIX_SWITCH(1, 1);
			MIX_SWITCH(3, 2);
			MIX_SWITCH(2, 3);	
			break;
		case SE_E1E3E2E4:
			MIX_SWITCH(0, 0);
			MIX_SWITCH(2, 1);
			MIX_SWITCH(1, 2);
			MIX_SWITCH(3, 3);	
  		break;
		case SE_E1E3E4E2:
			MIX_SWITCH(0, 0);
			MIX_SWITCH(2, 1);
			MIX_SWITCH(3, 2);
			MIX_SWITCH(1, 3);	
  		break;
		case SE_E1E4E2E3:
			MIX_SWITCH(0, 0);
			MIX_SWITCH(3, 1);
			MIX_SWITCH(1, 2);
			MIX_SWITCH(2, 3);
  		break;
		case SE_E1E4E3E2:
			MIX_SWITCH(0, 0);
			MIX_SWITCH(3, 1);
			MIX_SWITCH(2, 2);
			MIX_SWITCH(1, 3);
  		break;
			
		case SE_E2E1E3E4:			
			MIX_SWITCH(1, 0);
			MIX_SWITCH(0, 1);
			MIX_SWITCH(2, 2);
			MIX_SWITCH(3, 3);
  		break;
		case SE_E2E1E4E3:
			MIX_SWITCH(1, 0);
			MIX_SWITCH(0, 1);
			MIX_SWITCH(3, 2);
			MIX_SWITCH(2, 3);
  		break;
		case SE_E2E3E1E4:
			MIX_SWITCH(1, 0);
			MIX_SWITCH(2, 1);
			MIX_SWITCH(0, 2);
			MIX_SWITCH(3, 3);
  		break;
		case SE_E2E3E4E1:
			MIX_SWITCH(1, 0);
			MIX_SWITCH(2, 1);
			MIX_SWITCH(3, 2);
			MIX_SWITCH(0, 3);
  		break;
		case SE_E2E4E1E3:
			MIX_SWITCH(1, 0);
			MIX_SWITCH(3, 1);
			MIX_SWITCH(0, 2);
			MIX_SWITCH(2, 3);	
  		break;
		case SE_E2E4E3E1:
			MIX_SWITCH(1, 0);
			MIX_SWITCH(3, 1);
			MIX_SWITCH(2, 2);
			MIX_SWITCH(0, 3);			
  		break;

			
		case SE_E3E1E2E4:
			MIX_SWITCH(2, 0);
			MIX_SWITCH(0, 1);
			MIX_SWITCH(1, 2);
			MIX_SWITCH(3, 3);		
  		break;
		case SE_E3E1E4E2:
			MIX_SWITCH(2, 0);
			MIX_SWITCH(0, 1);
			MIX_SWITCH(3, 2);
			MIX_SWITCH(1, 3);				
  		break;	
		case SE_E3E2E1E4:
			MIX_SWITCH(2, 0);
			MIX_SWITCH(1, 1);
			MIX_SWITCH(0, 2);
			MIX_SWITCH(3, 3);					
  		break;
		case SE_E3E2E4E1:
			MIX_SWITCH(2, 0);
			MIX_SWITCH(1, 1);
			MIX_SWITCH(3, 2);
			MIX_SWITCH(0, 3);		
  		break;
		case SE_E3E4E1E2:
			MIX_SWITCH(2, 0);
			MIX_SWITCH(3, 1);
			MIX_SWITCH(0, 2);
			MIX_SWITCH(1, 3);		
  		break;
		case SE_E3E4E2E1:
			MIX_SWITCH(2, 0);
			MIX_SWITCH(3, 1);
			MIX_SWITCH(1, 2);
			MIX_SWITCH(0, 3);				
  		break;
			
		case SE_E4E1E2E3:
			MIX_SWITCH(3, 0);
			MIX_SWITCH(0, 1);
			MIX_SWITCH(1, 2);
			MIX_SWITCH(2, 3);		
  		break;
		case SE_E4E1E3E2:
			MIX_SWITCH(3, 0);
			MIX_SWITCH(0, 1);
			MIX_SWITCH(2, 2);
			MIX_SWITCH(1, 3);			
  		break;
		case SE_E4E2E1E3:
			MIX_SWITCH(3, 0);
			MIX_SWITCH(1, 1);
			MIX_SWITCH(0, 2);
			MIX_SWITCH(2, 3);			
  		break;
		case SE_E4E2E3E1:
			MIX_SWITCH(3, 0);
			MIX_SWITCH(1, 1);
			MIX_SWITCH(2, 2);
			MIX_SWITCH(0, 3);			
  		break;			
		case SE_E4E3E1E2:
			MIX_SWITCH(3, 0);
			MIX_SWITCH(2, 1);
			MIX_SWITCH(0, 2);
			MIX_SWITCH(1, 3);	
  		break;
		case SE_E4E3E2E1:
			MIX_SWITCH(3, 0);
			MIX_SWITCH(2, 1);
			MIX_SWITCH(1, 2);
			MIX_SWITCH(0, 3);	
  		break;
	#endif			
	}
}

static void Item_Config_SwitchExtruder(const uint8_t row) {
	Draw_Menu_Line(row,ICON_CURSOR);
	if(!mixer.mixing_enabled){
	 	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row),PSTR("Extr.Sequence"));
		DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, 216-12*MIXING_STEPPERS, MBASE(row), _getString_SwitchExtruder()); 
	}
	else{
		DWIN_Draw_MaskString_Default_Color(COLOR_GREY, LBLX, MBASE(row),PSTR("Extr.Sequence"));
		DWIN_Draw_MaskString_Default_Color(COLOR_GREY, 216-12*MIXING_STEPPERS, MBASE(row), _getString_SwitchExtruder()); 
	} 	
}
#endif

#endif

#if HAS_LEVELING
static void Item_Config_Leveling(const uint8_t row) {
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row), PSTR("Auto Leveling"));	
	Draw_Menu_Line(row,ICON_CURSOR);	
#if HAS_ONOFF_ICON  	
	Draw_ONOFF_Icon(row, HMI_flag.Leveling_Menu_Fg);
#else
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, MENUONOFF_X, MBASE(row),F_STRING_ONOFF(HMI_flag.Leveling_Menu_Fg));	
#endif
}

#if ENABLED(OPTION_NEWAUTOLEVELING)
static void Item_Config_ActiveLevel(const uint8_t row) {	
	Draw_Menu_Line(row,ICON_CURSOR);	
	DWIN_Draw_MaskString_Default_Color(HMI_flag.Leveling_Menu_Fg?COLOR_FLEX_ITEM : COLOR_GREY, LBLX, MBASE(row), PSTR("Active Autolevel"));		
#if HAS_ONOFF_ICON  		
	Draw_ONOFF_Icon(row, HMI_flag.AutoLeveling_Fg);
#else
	DWIN_Draw_MaskString_Default_Color(HMI_flag.Leveling_Menu_Fg?COLOR_FLEX_ITEM: COLOR_GREY, MENUONOFF_X, MBASE(row), F_STRING_ONOFF(HMI_flag.AutoLeveling_Fg));	
#endif
}
#else
static void Item_Config_ActiveLevel(const uint8_t row) {	
	Draw_Menu_Line(row,ICON_CURSOR);	
	DWIN_Draw_MaskString_Default_Color(HMI_flag.Leveling_Menu_Fg?COLOR_FLEX_ITEM : COLOR_GREY, LBLX, MBASE(row), PSTR("Active Autolevel"));		
#if HAS_ONOFF_ICON  		
	Draw_ONOFF_Icon(row, planner.leveling_active);
#else
	DWIN_Draw_MaskString_Default_Color(HMI_flag.Leveling_Menu_Fg?COLOR_FLEX_ITEM: COLOR_GREY, MENUONOFF_X, MBASE(row), F_STRING_ONOFF(planner.leveling_active));	
#endif
}
#endif


#if HAS_PROBE_XY_OFFSET
static void Item_Config_ProbeOffset(const uint8_t row) { 
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row), PSTR("PROBE OFFSET"));	
	Draw_Menu_Line(row,ICON_CURSOR);
	Draw_More_Icon(row);
}
#endif
#endif

#if HAS_OFFSET_MENU
static void Item_Config_HomeOffset(const uint8_t row) { 
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row), PSTR("HOME OFFSET"));	
	Draw_Menu_Line(row,ICON_CURSOR);
	Draw_More_Icon(row);
}
#endif

#if EITHER(PID_EDIT_MENU, PID_AUTOTUNE_MENU)
static void Item_Config_PIDTune(const uint8_t row) { 
#if ENABLED(PID_EDIT_MENU)
	DWIN_Draw_MaskString_Default_Color(COLOR_FLEX_ITEM, LBLX, MBASE(row), PSTR("Hotend PID"));
	Draw_Menu_Line(row,ICON_CURSOR);
	Draw_More_Icon(row);
#else
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("PID auto tune"));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), HMI_Value.PIDAutotune_Temp);
	Draw_Menu_Line(row,ICON_CURSOR);
#endif
}
#endif

#if ENABLED(PID_AUTOTUNE_MENU)
void DWIN_Show_PIDAutoTune(uint8_t cycles, uint8_t ncycles){
	char string_Buf[50]={0};
	if(DWIN_status != ID_SM_PIDAUTOTUNING) return;
	sprintf_P(string_Buf,PSTR("PID tuning, cycles = %1d/%1d ."), cycles, ncycles);
	DWIN_Show_Status_Message(COLOR_RED, string_Buf, 0);	
}

void DWIN_Show_PIDAutoTuneDone(){
	DWIN_Show_Status_Message(COLOR_WHITE, PSTR("PID Auto tune done!"));
	DWIN_FEEDBACK_CONFIRM();
	Draw_PIDTune_Menu();
	DWIN_status = ID_SM_IDLE;
}
#endif

void Draw_Config_Menu(const uint8_t MenuItem) {	
	DwinMenuID = DWMENU_CONFIG;
	DwinMenu_configure.set(MenuItem);
	DwinMenu_configure.index = _MAX(DwinMenu_configure.now, MROWS);
	
#if CONFIG_CASE_TOTAL > MROWS
	const int8_t scroll = MROWS - DwinMenu_configure.index;
	#define COSCROL(L) (scroll + (L))
#else
	#define COSCROL(L) (L)
#endif
	#define COVISI(L) WITHIN(COSCROL(L), 0, MROWS)
	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_STATUS);	
#if (DWINLCD_MENU_VERSION >= 2)
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_CONFIGRE, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);	
#else	
	DWIN_Draw_UnMaskString_Default(14, 7, PSTR("Configure")); 
#endif
	if (COVISI(CONFIG_CASE_BACK)) Draw_Back_First(DwinMenu_configure.now == CONFIG_CASE_BACK);  // < Back 

	#if ENABLED(FWRETRACT) 
		if (COVISI(CONFIG_CASE_RETRACT))	Item_Config_Retract(COSCROL(CONFIG_CASE_RETRACT));				// Retract >
	#endif 	
	
	#if ENABLED(LIN_ADVANCE) 
		if (COVISI(CONFIG_CASE_LINADVANCE))	Item_Config_LinearAdvance(COSCROL(CONFIG_CASE_LINADVANCE));				// Linear Advance >
	#endif 
		
	#if ENABLED(OPTION_REPEAT_PRINTING) 
		 if (COVISI(CONFIG_CASE_REPRINT)) 	Item_Config_Reprint(COSCROL(CONFIG_CASE_REPRINT));			// repeat print
	#endif

	#if ENABLED(FILAMENT_RUNOUT_SENSOR) 
	if (COVISI(CONFIG_CASE_FILAMENTRUNOUT)) Item_Config_FilamentRunOut(COSCROL(CONFIG_CASE_FILAMENTRUNOUT));  		// filament runout
	#endif 

	#if ENABLED(OPTION_ABORT_UNLOADFILAMENT)
	if (COVISI(CONFIG_CASE_FILAMENTAUTOUNLOAD)) Item_Config_FilamentAutoUnload(COSCROL(CONFIG_CASE_FILAMENTAUTOUNLOAD));  		// Auto Unload filament when printing abort
	#endif 
	
	#if ENABLED(POWER_LOSS_RECOVERY) 
	if (COVISI(CONFIG_CASE_POWERLOSS)) Item_Config_Powerloss(COSCROL(CONFIG_CASE_POWERLOSS));  // powerloss
	#endif

	#if ENABLED(OPTION_AUTOPOWEROFF) 
	if (COVISI(CONFIG_CASE_SHUTDOWN)) Item_Config_Shutdown(COSCROL(CONFIG_CASE_SHUTDOWN));  	 // shutdown
	#endif

	#if ENABLED(CASE_LIGHT_MENU)
	if (COVISI(CONFIG_CASE_BACKLIGHT)) 	Item_Config_Backlight(COSCROL(CONFIG_CASE_BACKLIGHT)); // Backlight
	#endif
	
	#if ENABLED(OPTION_WIFI_MODULE) 
	 if (COVISI(CONFIG_CASE_WIFI)) 	Item_Config_Wifi(COSCROL(CONFIG_CASE_WIFI));  	 					// WIFI	 
	#endif
	
 	//if(!IS_SD_PRINTING() && !IS_SD_PAUSED())
	{	
	#if BOTH(OPTION_WIFI_MODULE, OPTION_WIFI_BAUDRATE)
	 if (COVISI(CONFIG_CASE_WIFISPEED)) Item_Config_WifiBaudrate(COSCROL(CONFIG_CASE_WIFISPEED));			// WIFI Baudrate
	#endif

	#if ENABLED(OPTION_LASER)
	if (COVISI(CONFIG_CASE_LASER)) 	Item_Config_Laser(COSCROL(CONFIG_CASE_LASER));  	 					// Laser	 
	#endif	
	
	#if ENABLED(SPINDLE_FEATURE)
	if (COVISI(CONFIG_CASE_SPINDLE)) 	Item_Config_Spindle(COSCROL(CONFIG_CASE_SPINDLE));  	 	 // Spindle	 
	#endif	
	
	#if BOTH(MIXING_EXTRUDER, OPTION_MIXING_SWITCH)
	 if (COVISI(CONFIG_CASE_MIXERENABLE)) Item_Config_MixerEnable(COSCROL(CONFIG_CASE_MIXERENABLE));					// mixing feature:
	 #if ENABLED(SWITCH_EXTRUDER_MENU)
	 if (COVISI(CONFIG_CASE_SWITCHEXTRUDER)) Item_Config_SwitchExtruder(COSCROL(CONFIG_CASE_SWITCHEXTRUDER));	// switch extruder:
	 #endif
	#endif	

	#if HAS_OFFSET_MENU 
	 if (COVISI(CONFIG_CASE_HOMEOFFSET)) 	Item_Config_HomeOffset(COSCROL(CONFIG_CASE_HOMEOFFSET));  	 				//Home offset
	#endif

	#if HAS_LEVELING 		
	 if (COVISI(CONFIG_CASE_LEVELING)) Item_Config_Leveling(COSCROL(CONFIG_CASE_LEVELING));  	 								// auto lelevling	 
	 if (COVISI(CONFIG_CASE_ACTIVELEVEL)) Item_Config_ActiveLevel(COSCROL(CONFIG_CASE_ACTIVELEVEL)); 					// Active lelevling
	 #if HAS_PROBE_XY_OFFSET
	 if (COVISI(CONFIG_CASE_PROBEOFFSET)) Item_Config_ProbeOffset(COSCROL(CONFIG_CASE_PROBEOFFSET)); 					// Probe offset
	 #endif
	#endif	 
	 	
	#if EITHER(PID_EDIT_MENU, PID_AUTOTUNE_MENU)
	 if (COVISI(CONFIG_CASE_PIDTUNE)) 	Item_Config_PIDTune(COSCROL(CONFIG_CASE_PIDTUNE));  	 	// PID AutoTune:
	#endif
 }
 if (DwinMenu_configure.now) Draw_Menu_Cursor(COSCROL(DwinMenu_configure.now));
 Draw_Status_Area();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Control >> Config >> Auto retraction
//
#if ENABLED(FWRETRACT) 
static void Item_Retract_Retract_Enabled(const uint8_t row) {	
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Auto-Retract"));	
	Draw_Menu_Line(row,ICON_CURSOR);
#if HAS_ONOFF_ICON	
	Draw_ONOFF_Icon(row, fwretract.autoretract_enabled);
#else
	DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(row), F_STRING_ONOFF(fwretract.autoretract_enabled));
#endif
}

static void Item_Retract_Retract_mm(const uint8_t row) {
	HMI_Value.Retract_MM_scale = fwretract.settings.retract_length*MINUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Retract MM"));
	DWIN_Draw_Small_Float21(MENUVALUE_X-8, MBASE(row), HMI_Value.Retract_MM_scale);
	Draw_Menu_Line(row,ICON_CURSOR);
}

static void Item_Retract_Retract_V(const uint8_t row) {
	HMI_Value.Retract_V_scale = fwretract.settings.retract_feedrate_mm_s*MINUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Retract V"));
	DWIN_Draw_Small_Float21(MENUVALUE_X-8, MBASE(row), HMI_Value.Retract_V_scale);
	Draw_Menu_Line(row,ICON_CURSOR);
}

static void Item_Retract_Retract_ZHop(const uint8_t row) {
	HMI_Value.Retract_ZHOP_scale = fwretract.settings.retract_zraise*MINUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Retract ZHop"));
	DWIN_Draw_Small_Float21(MENUVALUE_X-8, MBASE(row), HMI_Value.Retract_ZHOP_scale);
	Draw_Menu_Line(row,ICON_CURSOR);
}

static void Item_Retract_UnRetract_mm(const uint8_t row) {
	HMI_Value.unRetract_MM_scale = fwretract.settings.retract_recover_extra*MAXUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("UnRetract MM"));
	DWIN_Draw_Small_Float22(MENUVALUE_X-16, MBASE(row), HMI_Value.unRetract_MM_scale);
	Draw_Menu_Line(row,ICON_CURSOR);
}

static void Item_Retract_UnRetract_V(const uint8_t row) {	
	HMI_Value.unRetract_V_scale = fwretract.settings.retract_recover_feedrate_mm_s*MINUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("UnRetract V"));
	DWIN_Draw_Small_Float21(MENUVALUE_X-8, MBASE(row),  HMI_Value.unRetract_V_scale);
	Draw_Menu_Line(row,ICON_CURSOR);
}

inline void Draw_Retract_Menu() {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	DwinMenu_fwretract.reset();	
	
#if RETRACT_CASE_TOTAL > MROWS
	const int8_t scroll = MROWS - DwinMenu_fwretract.index;
	#define RESCROL(L) (scroll + (L))
#else
	#define RESCROL(L) (L)
#endif
	#define REVISI(L) WITHIN(RESCROL(L), 0, MROWS)
	
	//title
	DWIN_Draw_UnMaskString_Default(14, 7, PSTR("Retract")); 
	if (REVISI(RETRACT_CASE_BACK)) Draw_Back_First(DwinMenu_fwretract.now == RETRACT_CASE_BACK);
#if ENABLED(FWRETRACT_AUTORETRACT)	
	if (REVISI(RETRACT_CASE_AUTO)) Item_Retract_Retract_Enabled(RESCROL(RETRACT_CASE_AUTO));
#endif
	if (REVISI(RETRACT_CASE_RETRACT_MM)) Item_Retract_Retract_mm(RESCROL(RETRACT_CASE_RETRACT_MM));
	if (REVISI(RETRACT_CASE_RETRACT_V)) Item_Retract_Retract_V(RESCROL(RETRACT_CASE_RETRACT_V));
	if (REVISI(RETRACT_CASE_RETRACT_ZHOP)) Item_Retract_Retract_ZHop(RESCROL(RETRACT_CASE_RETRACT_ZHOP));
	if (REVISI(RETRACT_CASE_RECOVER_MM)) Item_Retract_UnRetract_mm(RESCROL(RETRACT_CASE_RECOVER_MM));
	if (REVISI(RETRACT_CASE_RECOVER_V)) Item_Retract_UnRetract_V(RESCROL(RETRACT_CASE_RECOVER_V));
	
	if (DwinMenu_fwretract.now) Draw_Menu_Cursor(RESCROL(DwinMenu_fwretract.now));
}

void HMI_Retract_MM() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Retract_MM_scale)) {
			DwinMenuID = DWMENU_SET_RETRACT;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RETRACT_MM), HMI_Value.Retract_MM_scale);
			fwretract.settings.retract_length = (float)HMI_Value.Retract_MM_scale/MINUNITMULT;			
		}
		else {
			NOLESS(HMI_Value.Retract_MM_scale, 2*MINUNITMULT);
			NOMORE(HMI_Value.Retract_MM_scale, 30*MINUNITMULT);		
			DWIN_Draw_Selected_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RETRACT_MM), HMI_Value.Retract_MM_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Retract_V() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Retract_V_scale)) {
			DwinMenuID = DWMENU_SET_RETRACT;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RETRACT_V), HMI_Value.Retract_V_scale);
			fwretract.settings.retract_feedrate_mm_s = (float)HMI_Value.Retract_V_scale/MINUNITMULT;			
		}
		else {
			NOLESS(HMI_Value.Retract_V_scale, 5*MINUNITMULT);
			NOMORE(HMI_Value.Retract_V_scale, 50*MINUNITMULT);		
			DWIN_Draw_Selected_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RETRACT_V), HMI_Value.Retract_V_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Retract_ZHOP() {
	//char string_Buf[20]={0};
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Retract_ZHOP_scale)) {
			DwinMenuID = DWMENU_SET_RETRACT;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RETRACT_ZHOP), HMI_Value.Retract_ZHOP_scale);
			fwretract.settings.retract_zraise = (float)HMI_Value.Retract_ZHOP_scale/MINUNITMULT;
		}
		else {
			NOLESS(HMI_Value.Retract_ZHOP_scale, 0);
			NOMORE(HMI_Value.Retract_ZHOP_scale, 10*MINUNITMULT);		
			DWIN_Draw_Selected_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RETRACT_ZHOP), HMI_Value.Retract_ZHOP_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_UnRetract_MM() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.unRetract_MM_scale)) {
			DwinMenuID = DWMENU_SET_RETRACT;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float22(MENUVALUE_X-16, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RECOVER_MM), HMI_Value.unRetract_MM_scale);
			fwretract.settings.retract_recover_extra = (float)HMI_Value.unRetract_MM_scale/MAXUNITMULT;
		}
		else {
			NOLESS(HMI_Value.unRetract_MM_scale, 0);
			NOMORE(HMI_Value.unRetract_MM_scale, 10*MAXUNITMULT);		
			DWIN_Draw_Selected_Small_Float22(MENUVALUE_X-16, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RECOVER_MM), HMI_Value.unRetract_MM_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_UnRetract_V() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.unRetract_V_scale)) {
			DwinMenuID = DWMENU_SET_RETRACT;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RECOVER_V), HMI_Value.unRetract_V_scale);
			fwretract.settings.retract_recover_feedrate_mm_s = (float)HMI_Value.unRetract_V_scale/MINUNITMULT;		
		}
		else {
			NOLESS(HMI_Value.unRetract_V_scale, 5*MINUNITMULT);
			NOMORE(HMI_Value.unRetract_V_scale, 50*MINUNITMULT);		
			DWIN_Draw_Selected_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RECOVER_V), HMI_Value.unRetract_V_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Retract() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	 // Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_fwretract.inc(RETRACT_CASE_END)) {
			if (DwinMenu_fwretract.now > MROWS && DwinMenu_fwretract.now > DwinMenu_fwretract.index) {				
				// Scroll up and draw a blank bottom line
				DwinMenu_fwretract.index = DwinMenu_fwretract.now;
				Scroll_Menu(DWIN_SCROLL_UP);
				//if(DwinMenu_fwretract.index == RETRACT_CASE_AUTO) Item_Retract_Retract_Enabled(MROWS);
				//if(DwinMenu_fwretract.index == RETRACT_CASE_RETRACT_MM) Item_Retract_Retract_mm(MROWS);
				//if(DwinMenu_fwretract.index == RETRACT_CASE_RETRACT_V) Item_Retract_Retract_V(MROWS);
				//if(DwinMenu_fwretract.index == RETRACT_CASE_RETRACT_ZHOP) Item_Retract_Retract_ZHop(MROWS);
				//if(DwinMenu_fwretract.index == RETRACT_CASE_RECOVER_MM) Item_Retract_UnRetract_mm(MROWS);
				if(DwinMenu_fwretract.index == RETRACT_CASE_RECOVER_V) Item_Retract_UnRetract_V(MROWS);
			}
			else {
				Move_Highlight(1, DwinMenu_fwretract.now + MROWS - DwinMenu_fwretract.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_fwretract.dec()) {
			if (DwinMenu_fwretract.now < DwinMenu_fwretract.index - MROWS) {
				DwinMenu_fwretract.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if(DwinMenu_fwretract.index - MROWS == RETRACT_CASE_BACK) Draw_Back_First();
		#if ENABLED(FWRETRACT_AUTORETRACT)				
				if(DwinMenu_fwretract.index - MROWS == RETRACT_CASE_AUTO) Item_Retract_Retract_Enabled(0);
				else 
		#endif
				if(DwinMenu_fwretract.index - MROWS == RETRACT_CASE_RETRACT_MM) Item_Retract_Retract_mm(0);
				//else if(DwinMenu_fwretract.index - MROWS == RETRACT_CASE_RETRACT_V) Item_Retract_Retract_V(0);
				//else if(DwinMenu_fwretract.index - MROWS == RETRACT_CASE_RETRACT_ZHOP) Item_Retract_Retract_ZHop(0);
				//else if(DwinMenu_fwretract.index - MROWS == RETRACT_CASE_RECOVER_MM) Item_Retract_UnRetract_mm(0);
				//else if(DwinMenu_fwretract.index - MROWS == RETRACT_CASE_RECOVER_V) Item_Retract_UnRetract_V(0);
			}
			else {
				Move_Highlight(-1, DwinMenu_fwretract.now + MROWS - DwinMenu_fwretract.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_fwretract.now) {
			case 0: 									// Back				
				Draw_Config_Menu(CONFIG_CASE_RETRACT);				
			break;

		#if ENABLED(FWRETRACT_AUTORETRACT)
			case RETRACT_CASE_AUTO: 					// auto
				DwinMenuID = DWMENU_SET_RETRACT;
				fwretract.autoretract_enabled = !fwretract.autoretract_enabled;				
			#if HAS_ONOFF_ICON	
				Draw_ONOFF_Icon(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_AUTO, fwretract.autoretract_enabled);
			#else
				DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_AUTO), F_STRING_ONOFF(fwretract.autoretract_enabled));
			#endif
				fwretract.refresh_autoretract();
			break;
		#endif
	 
			case RETRACT_CASE_RETRACT_MM:  			// RETRACT_MM
				DwinMenuID = DWMENU_SET_RETRACT_MM;
				HMI_Value.Retract_MM_scale = fwretract.settings.retract_length*MINUNITMULT;
				DWIN_Draw_Selected_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RETRACT_MM), HMI_Value.Retract_MM_scale);
				EncoderRate.enabled = true;
			break;

			case RETRACT_CASE_RETRACT_V:  			// RETRACT_V
				DwinMenuID = DWMENU_SET_RETRACT_V;
				HMI_Value.Retract_V_scale = fwretract.settings.retract_feedrate_mm_s*MINUNITMULT;
				DWIN_Draw_Selected_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RETRACT_V), HMI_Value.Retract_V_scale);
				EncoderRate.enabled = true;
			break;

			case RETRACT_CASE_RETRACT_ZHOP:  			// RETRACT_ZHOP
				DwinMenuID = DWMENU_SET_RETRACT_ZHOP;
				HMI_Value.Retract_ZHOP_scale = fwretract.settings.retract_zraise*MINUNITMULT;
				DWIN_Draw_Selected_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RETRACT_ZHOP), HMI_Value.Retract_ZHOP_scale);
				EncoderRate.enabled = true;
			break;

			case RETRACT_CASE_RECOVER_MM:  			// RECOVER_MM
				DwinMenuID = DWMENU_SET_UNRETRACT_MM;
				HMI_Value.unRetract_MM_scale = fwretract.settings.retract_recover_extra*MAXUNITMULT;
				DWIN_Draw_Selected_Small_Float22(MENUVALUE_X-16, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RECOVER_MM), HMI_Value.unRetract_MM_scale);
				EncoderRate.enabled = true;
			break;

			case RETRACT_CASE_RECOVER_V:  			// RECOVER_V
				DwinMenuID = DWMENU_SET_UNRETRACT_V;
				HMI_Value.unRetract_V_scale = fwretract.settings.retract_recover_feedrate_mm_s*MINUNITMULT;
				DWIN_Draw_Selected_Small_Float21(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_fwretract.index + RETRACT_CASE_RECOVER_V), HMI_Value.unRetract_V_scale);
				EncoderRate.enabled = true;
			break;
   
   default: break;
  }
 }
 dwinLCD.UpdateLCD();
}
#endif


#if ENABLED(LIN_ADVANCE) 
#if (EXTRUDERS > 1)
inline void Item_LinearAdvance_K(uint8_t row, uint8_t e = 0){		
	Draw_Menu_Line(row, ICON_EXTRUDER1+e);
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Advance K E"));	
	DWIN_Draw_IntValue_Default(1,LBLX+88, MBASE(row), (e+1));
	HMI_Value.extruder_advance_K[e] = (int16_t)(planner.extruder_advance_K[e]*MAXUNITMULT);
	DWIN_Draw_Small_Float22(MENUVALUE_X-16, MBASE(row), HMI_Value.extruder_advance_K[e]);	
}

inline void Draw_LinearAdvance_Menu() {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	DwinMenu_LinAdvance.reset(); 
	//title
	DWIN_Draw_UnMaskString_Default(14, 7, PSTR("Linear Advance")); 
	Draw_Back_First(DwinMenu_LinAdvance.now == LINADVANCE_CASE_BACK);	
	LOOP_L_N(i, E_STEPPERS){
		if(WITHIN(i, 0, MROWS))	Item_LinearAdvance_K(LINADVANCE_CASE_E0 + i, i);
	}
	if (DwinMenu_LinAdvance.now) Draw_Menu_Cursor(DwinMenu_LinAdvance.now);
}

void HMI_Set_LinearAdvance(uint8_t e) {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.extruder_advance_K[e])) {
			DwinMenuID = DWMENU_SET_LINADVANCE;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float22(MENUVALUE_X-16, MBASE(MROWS -DwinMenu_LinAdvance.index + LINADVANCE_CASE_E0 + e), HMI_Value.extruder_advance_K[e]);
			planner.extruder_advance_K[e] = (float)HMI_Value.extruder_advance_K[e]/MAXUNITMULT;	
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.extruder_advance_K[e], 0);
			NOMORE(HMI_Value.extruder_advance_K[e], 200);		
			DWIN_Draw_Selected_Small_Float22(MENUVALUE_X-16, MBASE(MROWS -DwinMenu_LinAdvance.index + LINADVANCE_CASE_E0 + e), HMI_Value.extruder_advance_K[e]);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_LinearAdvance() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	 // Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_LinAdvance.inc(LINADVANCE_CASE_END)) {
			Move_Highlight(1, DwinMenu_LinAdvance.now + MROWS - DwinMenu_LinAdvance.index);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_LinAdvance.dec()) {
				Move_Highlight(-1, DwinMenu_LinAdvance.now + MROWS - DwinMenu_LinAdvance.index);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {		
		uint8_t e = DwinMenu_LinAdvance.now - LINADVANCE_CASE_E0;
		switch (DwinMenu_LinAdvance.now) {
			case 0: 									// Back				
				Draw_Config_Menu(CONFIG_CASE_LINADVANCE);
				HMI_AudioFeedback(settings.save());
			break;

			case LINADVANCE_CASE_E0...(LINADVANCE_CASE_E0 + E_STEPPERS -1):			
				DwinMenuID = (_emDWIN_MENUID_)(DWMENU_SET_LINADVANCE_E0 + e);
				HMI_Value.extruder_advance_K[e] = planner.extruder_advance_K[e]*MAXUNITMULT;
				DWIN_Draw_Selected_Small_Float22(MENUVALUE_X-16, MBASE(MROWS -DwinMenu_LinAdvance.index + LINADVANCE_CASE_E0 + e), HMI_Value.extruder_advance_K[e]);
				EncoderRate.enabled = true;
			break;
			
			default: break;
  	}
 }
 dwinLCD.UpdateLCD();
}
#else
void HMI_Set_LinearAdvance() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.extruder_advance_K[0])) {
			DwinMenuID = DWMENU_CONFIG;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float22(MENUVALUE_X-16, MBASE(MROWS -DwinMenu_configure.index + CONFIG_CASE_LINADVANCE), HMI_Value.extruder_advance_K[0]);
			planner.extruder_advance_K[0] = (float)HMI_Value.extruder_advance_K[0]/MAXUNITMULT;	
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.extruder_advance_K[0], 0);
			NOMORE(HMI_Value.extruder_advance_K[0], 200);		
			DWIN_Draw_Selected_Small_Float22(MENUVALUE_X-16, MBASE(MROWS -DwinMenu_configure.index + CONFIG_CASE_LINADVANCE), HMI_Value.extruder_advance_K[0]);
		}
		dwinLCD.UpdateLCD();
	}
}
#endif //(EXTRUDERS > 1)
#endif //ENABLED(LIN_ADVANCE)


#if ENABLED(CASE_LIGHT_MENU) && DISABLED(CASE_LIGHT_NO_BRIGHTNESS)
void HMI_Adjust_Brightness() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if(caselight.brightness == 255 && encoder_diffState == ENCODER_DIFF_CW) return;
		if(caselight.brightness == 0 && encoder_diffState == ENCODER_DIFF_CCW) return;
		if (Apply_Encoder_uint8(encoder_diffState, &caselight.brightness)) {			
			DwinMenuID = DWMENU_CONFIG;
			EncoderRate.enabled = false;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_configure.index + CONFIG_CASE_BACKLIGHT), caselight.brightness);			
			if(!IS_SD_PRINTING() && !IS_SD_PAUSED() && !IS_SD_FILE_OPEN()) HMI_AudioFeedback(settings.save());
		}
		else{
			if(caselight.brightness == 50 && encoder_diffState == ENCODER_DIFF_CCW) caselight.brightness = 0;
			if(caselight.brightness == 0 && encoder_diffState == ENCODER_DIFF_CW) caselight.brightness = 50;
			#if ENABLED(CASE_LIGHT_MAX_PWM)
			NOMORE(caselight.brightness, CASE_LIGHT_MAX_PWM);
			#endif
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_configure.index + CONFIG_CASE_BACKLIGHT), caselight.brightness);		
		}
		caselight.update_brightness();
		dwinLCD.UpdateLCD();
	}
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Control >> Config >> PID Tune
//
#if ENABLED(PID_EDIT_MENU)
static void Item_PIDTune_KP(const uint8_t row) {
	HMI_Value.PIDKp= thermalManager.temp_hotend[0].pid.Kp * MINUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("PID Tune P"));
	DWIN_Draw_Small_Float31(MENUVALUE_X-8, MBASE(row), HMI_Value.PIDKp);
	Draw_Menu_Line(row,ICON_CURSOR);
}

static void Item_PIDTune_KI(const uint8_t row) {
	HMI_Value.PIDKi= unscalePID_i(thermalManager.temp_hotend[0].pid.Ki) * MAXUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("PID Tune I"));
	DWIN_Draw_Small_Float22(MENUVALUE_X-8, MBASE(row), HMI_Value.PIDKi);
	Draw_Menu_Line(row,ICON_CURSOR);
}

static void Item_PIDTune_KD(const uint8_t row) {
	HMI_Value.PIDKd= unscalePID_d(thermalManager.temp_hotend[0].pid.Kd) * MINUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("PID Tune D"));
	DWIN_Draw_Small_Float31(MENUVALUE_X-8, MBASE(row), HMI_Value.PIDKd);
	Draw_Menu_Line(row,ICON_CURSOR);
}

#if ENABLED(PID_AUTOTUNE_MENU)
static void Item_PIDTune_Auto(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("PID auto tune"));	
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), HMI_Value.PIDAutotune_Temp);
	Draw_Menu_Line(row,ICON_CURSOR);
}
#endif


void Draw_PIDTune_Menu() {
	DwinMenuID = DWMENU_PID_TUNE;

	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	DwinMenu_PIDTune.reset();

#if PIDTUNE_CASE_TOTAL > MROWS
	const int8_t scroll = MROWS - DwinMenu_PIDTune.index;
	#define PIDSCROL(L) (scroll + (L))
#else
	#define PIDSCROL(L) (L)
#endif
	#define PIDVISI(L) WITHIN(PIDSCROL(L), 0, MROWS)
	
	//title
	DWIN_Draw_UnMaskString_Default(14, 7, PSTR("PID Tune")); 
	if (PIDVISI(PIDTUNE_CASE_BACK)) Draw_Back_First(DwinMenu_PIDTune.now == PIDTUNE_CASE_BACK);
	
	if (PIDVISI(PIDTUNE_CASE_KP)) Item_PIDTune_KP(PIDSCROL(PIDTUNE_CASE_KP));
	if (PIDVISI(PIDTUNE_CASE_KI)) Item_PIDTune_KI(PIDSCROL(PIDTUNE_CASE_KI));
	if (PIDVISI(PIDTUNE_CASE_KD)) Item_PIDTune_KD(PIDSCROL(PIDTUNE_CASE_KD));
#if ENABLED(PID_AUTOTUNE_MENU)	
	if (PIDVISI(PIDTUNE_CASE_AUTO)) Item_PIDTune_Auto(PIDSCROL(PIDTUNE_CASE_AUTO));
#endif	
	if (DwinMenu_PIDTune.now) Draw_Menu_Cursor(PIDSCROL(DwinMenu_PIDTune.now));
}

void HMI_PIDTune_KP() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.PIDKp)) {
			DwinMenuID = DWMENU_PID_TUNE;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float31(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_KP), HMI_Value.PIDKp);
			thermalManager.temp_hotend[0].pid.Kp = (float)HMI_Value.PIDKp/MINUNITMULT;
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.PIDKp, 0);
			NOMORE(HMI_Value.PIDKp, 999*MINUNITMULT);		
			DWIN_Draw_Selected_Small_Float31(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_KP), HMI_Value.PIDKp);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_PIDTune_KI() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.PIDKi)) {
			DwinMenuID = DWMENU_PID_TUNE;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float22(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_KI), HMI_Value.PIDKi);
			thermalManager.temp_hotend[0].pid.Ki = scalePID_i((float)HMI_Value.PIDKi/MAXUNITMULT);
			thermalManager.updatePID();
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.PIDKi, 0);
			NOMORE(HMI_Value.PIDKi, 300*MAXUNITMULT);		
			DWIN_Draw_Selected_Small_Float22(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_KI), HMI_Value.PIDKi);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_PIDTune_KD() {
	//char string_Buf[20]={0};
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.PIDKd)) {
			DwinMenuID = DWMENU_PID_TUNE;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float31(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_KD), HMI_Value.PIDKd);
			thermalManager.temp_hotend[0].pid.Kd = scalePID_d((float)HMI_Value.PIDKd/MINUNITMULT);
			thermalManager.updatePID();
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.PIDKd, 0);
			NOMORE(HMI_Value.PIDKd, 300*MINUNITMULT);		
			DWIN_Draw_Selected_Small_Float31(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_KD), HMI_Value.PIDKd);
		}
		dwinLCD.UpdateLCD();
	}
}

#if ENABLED(PID_AUTOTUNE_MENU)
void HMI_PID_AutoTune() {	
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze(); 
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.PIDAutotune_Temp)) {
			DwinMenuID = DWMENU_PID_TUNE;
			EncoderRate.enabled = false;			
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_AUTO), HMI_Value.PIDAutotune_Temp);			
			DWIN_FEEDBACK_CONFIRM();
			thermalManager.setTargetHotend(0, 0);
			thermalManager.set_fan_speed(0, 255);			
			if(thermalManager.degHotend(0) >= HMI_Value.PIDAutotune_Temp - 20) DWIN_Show_Status_Message(COLOR_RED, PSTR("Wait for hot end cooling..."), 0);			
			DWIN_status = ID_SM_PIDAUTOTUNE;
		}
		else {
			NOLESS(HMI_Value.PIDAutotune_Temp, EXTRUDE_MINTEMP);
			NOMORE(HMI_Value.PIDAutotune_Temp, (HEATER_0_MAXTEMP - HOTEND_OVERSHOOT));		
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_AUTO), HMI_Value.PIDAutotune_Temp);
		}
		dwinLCD.UpdateLCD();
	}	
}
#endif


void HMI_PIDTune() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	 // Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_PIDTune.inc(PIDTUNE_CASE_END)) {
			Move_Highlight(1, DwinMenu_PIDTune.now + MROWS - DwinMenu_PIDTune.index);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_PIDTune.dec()) {
				Move_Highlight(-1, DwinMenu_PIDTune.now + MROWS - DwinMenu_PIDTune.index);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
	#if ENABLED(PID_AUTOTUNE_MENU)
		if(DWIN_status == ID_SM_PIDAUTOTUNING){ 
			DWIN_FEEDBACK_WARNNING();
			dwinLCD.UpdateLCD();
			return;
		}
 		else if(DWIN_status == ID_SM_PIDAUTOTUNE){ 
			DWIN_status = ID_SM_IDLE;
			DWIN_Show_Status_Message(COLOR_WHITE, PSTR("PID Auto tune is canceled!"));
		}
	#endif
		switch (DwinMenu_PIDTune.now) {
			case 0: 									// Back					
					Draw_Config_Menu(CONFIG_CASE_PIDTUNE);
			break;

			case PIDTUNE_CASE_KP: 			// PID_P
				DwinMenuID = DWMENU_PID_KP;
				
				HMI_Value.PIDKp= thermalManager.temp_hotend[0].pid.Kp * MINUNITMULT;
				DWIN_Draw_Selected_Small_Float31(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_KP), HMI_Value.PIDKp);
				EncoderRate.enabled = true;
			break;
	 
			case PIDTUNE_CASE_KI:  			// PID_I
				DwinMenuID = DWMENU_PID_KI;
				
				HMI_Value.PIDKi= unscalePID_i(thermalManager.temp_hotend[0].pid.Ki) * MAXUNITMULT;
				DWIN_Draw_Selected_Small_Float22(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_KI), HMI_Value.PIDKi);
				EncoderRate.enabled = true;
			break;

			case PIDTUNE_CASE_KD:  			// PID_D
				DwinMenuID = DWMENU_PID_KD;
				
				HMI_Value.PIDKd = unscalePID_d(thermalManager.temp_hotend[0].pid.Kd) * MINUNITMULT;
				DWIN_Draw_Selected_Small_Float31(MENUVALUE_X-8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_KD), HMI_Value.PIDKd);
				EncoderRate.enabled = true;
			break;

	 #if ENABLED(PID_AUTOTUNE_MENU)
			case PIDTUNE_CASE_AUTO:  			// PID_AUTO
				DwinMenuID = DWMENU_PID_AUTOTUNE;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_PIDTune.index + PIDTUNE_CASE_AUTO), HMI_Value.PIDAutotune_Temp);
				EncoderRate.enabled = true;
			break;
   #endif
	 
   default: break;
  }
 }
 dwinLCD.UpdateLCD();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Control >> Motion
//
static void Item_Motion_Feedrate(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXSPEED);
	DWIN_Show_MultiLanguage_String(MTSTRING_FEEDRATE, LBLX, MBASE(row));
	Draw_More_Icon(row);
}

static void Item_Motion_Accel(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXACCELERATED);
	DWIN_Show_MultiLanguage_String(MTSTRING_ACCELERATION, LBLX, MBASE(row));
	Draw_More_Icon(row);
}

static void Item_Motion_Jerk(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXJERK);
	DWIN_Show_MultiLanguage_String(MTSTRING_JERK, LBLX, MBASE(row));
	Draw_More_Icon(row);
}

static void Item_Motion_Steps(uint8_t row){
	Draw_Menu_Line(row, ICON_STEP);
	DWIN_Show_MultiLanguage_String(MTSTRING_STEPPERMM, LBLX, MBASE(row));
	Draw_More_Icon(row);
}

static void Draw_Motion_Menu(const uint8_t MenuItem = 0) {
	DwinMenuID = DWMENU_MOTION;
	DwinMenu_motion.set(MenuItem);
  DwinMenu_motion.index = _MAX(DwinMenu_motion.now, MROWS);
	 
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_MOTION, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	Draw_Back_First(DwinMenu_motion.now == MOTION_CASE_BACK);
	
	Item_Motion_Feedrate(MOTION_CASE_FEEDRATE);
	Item_Motion_Accel(MOTION_CASE_ACCEL);
	Item_Motion_Jerk(MOTION_CASE_JERK);
	Item_Motion_Steps(MOTION_CASE_STEPS);	
	if (DwinMenu_motion.now) Draw_Menu_Cursor(DwinMenu_motion.now);
}

///////////////////////////////////////////////
//
// Control >> Motion >> Feedrate
//
inline void Item_Feedrate_MaxX(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXSPEEDX);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_FEEDRATE, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+6, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_X, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+get_MultiLanguageString_Width(MTSTRING_FEEDRATE)+12, MBASE(row));
	DWIN_Draw_IntValue_Default(4,CONFIGVALUE_X, MBASE(row), planner.settings.max_feedrate_mm_s[X_AXIS]);
}

inline void Item_Feedrate_MaxY(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXSPEEDY);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_FEEDRATE, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+6, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Y, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+get_MultiLanguageString_Width(MTSTRING_FEEDRATE)+12, MBASE(row));
	DWIN_Draw_IntValue_Default(4,CONFIGVALUE_X, MBASE(row), planner.settings.max_feedrate_mm_s[Y_AXIS]);
}

inline void Item_Feedrate_MaxZ(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXSPEEDZ);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_FEEDRATE, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+6, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+get_MultiLanguageString_Width(MTSTRING_FEEDRATE)+12, MBASE(row));
	DWIN_Draw_IntValue_Default(4,CONFIGVALUE_X, MBASE(row), planner.settings.max_feedrate_mm_s[Z_AXIS]);
}

inline void Item_Feedrate_MaxE(uint8_t row, uint8_t e = 0){	
	uint16_t x = LBLX;
	Draw_Menu_Line(row, ICON_MAXSPEEDE);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, x, MBASE(row));
	x += get_MultiLanguageString_Width(MTSTRING_MAX) + 6;
	DWIN_Show_MultiLanguage_String(MTSTRING_FEEDRATE, x, MBASE(row));
	x += get_MultiLanguageString_Width(MTSTRING_FEEDRATE) + 6;
	DWIN_Show_MultiLanguage_String(MTSTRING_E, x, MBASE(row));
#if ENABLED(DISTINCT_E_FACTORS)			
	x += get_MultiLanguageString_Width(MTSTRING_E);
	DWIN_Show_MultiLanguage_String(MTSTRING_1 + e, x, MBASE(row));
#endif	
	DWIN_Draw_IntValue_Default(4,CONFIGVALUE_X, MBASE(row), planner.settings.max_feedrate_mm_s[E_AXIS + e]);	
}

inline void Draw_Max_Feedrate_Menu() {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	DwinMenu_feedrate.reset();

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_FEEDRATE, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	Draw_Back_First(DwinMenu_feedrate.now == MAXFEEDRATE_CASE_BACK);	
	Item_Feedrate_MaxX(MAXFEEDRATE_CASE_X);
	Item_Feedrate_MaxY(MAXFEEDRATE_CASE_Y);
	Item_Feedrate_MaxZ(MAXFEEDRATE_CASE_Z);	
#if ENABLED(DISTINCT_E_FACTORS)
	LOOP_L_N(i, E_STEPPERS){
		if(WITHIN(MAXFEEDRATE_CASE_E + i, 0, MROWS))	Item_Feedrate_MaxE(MAXFEEDRATE_CASE_E + i, i);
	}
#else
	Item_Feedrate_MaxE(MAXFEEDRATE_CASE_E);
#endif			
		if (DwinMenu_feedrate.now) Draw_Menu_Cursor(DwinMenu_feedrate.now);
}


constexpr float default_axis_steps_per_unit[] = DEFAULT_AXIS_STEPS_PER_UNIT;
#define MIN_MAXFEEDSPEED      10
#define MIN_MAXACCELERATION   100
#define MIN_MAXJERK           0.1
void HMI_Adjust_MaxFeedrate() {
#if ENABLED(LIMITED_MAX_FR_EDITING)
	constexpr float max_feedrate_edit[] 	 = MAX_FEEDRATE_EDIT_VALUES;
#else
	constexpr float max_feedrate_edit[] 	 = { 400, 400, 16, 100 };
#endif
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Max_Feedrate)) {
			DwinMenuID = DWMENU_SET_MAXSPEED;
			EncoderRate.enabled = false;
			if (WITHIN(HMI_flag.axis, X_AXIS, XYZE_N)){
				planner.set_max_feedrate(HMI_flag.axis, HMI_Value.Max_Feedrate);				
			}
			DWIN_Draw_IntValue_Default(3, CONFIGVALUE_X+8, MBASE(DwinMenu_feedrate.now + MROWS - DwinMenu_feedrate.index), HMI_Value.Max_Feedrate);
		}
		else {
			// MaxFeedspeed limit
			if (WITHIN(HMI_flag.axis, X_AXIS, XYZE_N))
				NOMORE(HMI_Value.Max_Feedrate, max_feedrate_edit[HMI_flag.axis]);
			if (HMI_Value.Max_Feedrate < MIN_MAXFEEDSPEED) 
				HMI_Value.Max_Feedrate = MIN_MAXFEEDSPEED;
			// MaxFeedspeed value
			DWIN_Draw_Select_IntValue_Default(3, CONFIGVALUE_X+8, MBASE(DwinMenu_feedrate.now + MROWS - DwinMenu_feedrate.index), HMI_Value.Max_Feedrate);
		}
		dwinLCD.UpdateLCD();
	}
}

/* Max Speed */
void HMI_MaxFeedrate() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_feedrate.inc(MAXFEEDRATE_CASE_END)) {
			if (DwinMenu_feedrate.now > MROWS && DwinMenu_feedrate.now > DwinMenu_feedrate.index) {
				DwinMenu_feedrate.index = DwinMenu_feedrate.now;
				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);
			#if ENABLED(DISTINCT_E_FACTORS)
    		LOOP_L_N(i, E_STEPPERS){
					if (DwinMenu_feedrate.index == MAXFEEDRATE_CASE_E + i) Item_Feedrate_MaxE(MROWS, i);
				}
			#endif			
			}
			else {
				Move_Highlight(1, DwinMenu_feedrate.now + MROWS - DwinMenu_feedrate.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_feedrate.dec()) {			
			if (DwinMenu_feedrate.now < DwinMenu_feedrate.index - MROWS) {
				DwinMenu_feedrate.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if (DwinMenu_feedrate.index - MROWS == MAXFEEDRATE_CASE_BACK) Draw_Back_First();    		
				else if (DwinMenu_feedrate.index - MROWS == MAXFEEDRATE_CASE_X) Item_Feedrate_MaxX(0);
				else if (DwinMenu_feedrate.index - MROWS == MAXFEEDRATE_CASE_Y) Item_Feedrate_MaxY(0);				
				else if (DwinMenu_feedrate.index - MROWS == MAXFEEDRATE_CASE_Z) Item_Feedrate_MaxZ(0);				
			}
			else {
				Move_Highlight(-1, DwinMenu_feedrate.now + MROWS - DwinMenu_feedrate.index);
			}
		}
	}	
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		if (WITHIN(DwinMenu_feedrate.now, MAXFEEDRATE_CASE_X, MAXFEEDRATE_CASE_TOTAL)) {
			DwinMenuID = DWMENU_SET_MAXSPEED_VALUE;
			HMI_flag.axis = AxisEnum(DwinMenu_feedrate.now - 1);
			HMI_Value.Max_Feedrate = planner.settings.max_feedrate_mm_s[HMI_flag.axis];
			DWIN_Draw_Select_IntValue_Default(3, CONFIGVALUE_X+8, MBASE(DwinMenu_feedrate.now + MROWS - DwinMenu_feedrate.index), HMI_Value.Max_Feedrate);
			EncoderRate.enabled = true;
		}
		else { // Back   
			Draw_Motion_Menu(MOTION_CASE_FEEDRATE);
		}
	}
	dwinLCD.UpdateLCD();
}


///////////////////////////////////////////////
//
// Control >> Motion >> Acceleration
//
inline void Item_Accel_MaxX(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXACCX);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_ACCEL, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+6, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_X, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+get_MultiLanguageString_Width(MTSTRING_ACCEL)+12, MBASE(row));
	DWIN_Draw_IntValue_Default(4,CONFIGVALUE_X, MBASE(row), planner.settings.max_acceleration_mm_per_s2[X_AXIS]);
}

inline void Item_Accel_MaxY(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXACCY);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_ACCEL, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+6, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Y, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+get_MultiLanguageString_Width(MTSTRING_ACCEL)+12, MBASE(row));
	DWIN_Draw_IntValue_Default(4,CONFIGVALUE_X, MBASE(row), planner.settings.max_acceleration_mm_per_s2[Y_AXIS]);
}

inline void Item_Accel_MaxZ(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXACCZ);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_ACCEL, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+6, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+get_MultiLanguageString_Width(MTSTRING_ACCEL)+12, MBASE(row));
	DWIN_Draw_IntValue_Default(4,CONFIGVALUE_X, MBASE(row), planner.settings.max_acceleration_mm_per_s2[Z_AXIS]);
}

inline void Item_Accel_MaxE(uint8_t row, uint8_t e = 0){
	uint16_t x = LBLX;
	Draw_Menu_Line(row, ICON_MAXACCE);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, x, MBASE(row));
	x += get_MultiLanguageString_Width(MTSTRING_MAX) + 6;
	DWIN_Show_MultiLanguage_String(MTSTRING_ACCEL, x, MBASE(row));
	x += get_MultiLanguageString_Width(MTSTRING_ACCEL) + 6;
	DWIN_Show_MultiLanguage_String(MTSTRING_E, x, MBASE(row));
#if ENABLED(DISTINCT_E_FACTORS)			
	x += get_MultiLanguageString_Width(MTSTRING_E);
	DWIN_Show_MultiLanguage_String(MTSTRING_1 + e, x, MBASE(row));
#endif
	DWIN_Draw_IntValue_Default(5,CONFIGVALUE_X-8, MBASE(row), planner.settings.max_acceleration_mm_per_s2[E_AXIS + e]);
}


inline void Draw_Max_Accel_Menu() {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	DwinMenu_accel.reset();
	//DwinMenu_accel.index = _MAX(DwinMenu_accel.now, MROWS);

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_ACCEL, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	Draw_Back_First(DwinMenu_accel.now == 0);
	Item_Accel_MaxX(MAXACCL_CASE_X);
	Item_Accel_MaxY(MAXACCL_CASE_Y);
	Item_Accel_MaxZ(MAXACCL_CASE_Z);
#if ENABLED(DISTINCT_E_FACTORS)
	LOOP_L_N(i, E_STEPPERS){
		if(WITHIN(MAXACCL_CASE_E + i, 0, MROWS))	Item_Accel_MaxE(MAXACCL_CASE_E + i, i);
	}	
#else
	Item_Accel_MaxE(MAXACCL_CASE_E);
#endif
	if (DwinMenu_accel.now) Draw_Menu_Cursor(DwinMenu_accel.now);
}

void HMI_Adjust_MaxAcceleration() {	
#if ENABLED(LIMITED_MAX_ACCEL_EDITING)
	constexpr float max_accel_edit[]  = MAX_ACCEL_EDIT_VALUES;
#else
	constexpr float max_accel_edit[]  = { 20000, 20000, 200, 10000 };
#endif
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Max_Acceleration)) {
			DwinMenuID = DWMENU_SET_MAXACC;
			EncoderRate.enabled = false;
			if(WITHIN(HMI_flag.axis, X_AXIS, XYZE_N)) planner.set_max_acceleration(HMI_flag.axis, HMI_Value.Max_Acceleration);
			DWIN_Draw_IntValue_Default(5, CONFIGVALUE_X-8, MBASE(DwinMenu_accel.now + MROWS - DwinMenu_accel.index), HMI_Value.Max_Acceleration);
		}
		else {
			// Max Acceleration limit
			if (WITHIN(HMI_flag.axis, X_AXIS, XYZE_N))
				NOMORE(HMI_Value.Max_Acceleration, max_accel_edit[HMI_flag.axis]);
			if (HMI_Value.Max_Acceleration < MIN_MAXACCELERATION) 
				HMI_Value.Max_Acceleration = MIN_MAXACCELERATION;
			// Max Acceleration value
			DWIN_Draw_Select_IntValue_Default(5, CONFIGVALUE_X-8, MBASE(DwinMenu_accel.now + MROWS - DwinMenu_accel.index), HMI_Value.Max_Acceleration);
		}
		dwinLCD.UpdateLCD();
	}
}

/* Max Acceleration */
void HMI_MaxAcceleration() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_accel.inc(MAXACCL_CASE_END)) {
			if (DwinMenu_accel.now > MROWS && DwinMenu_accel.now > DwinMenu_accel.index) {
				DwinMenu_accel.index = DwinMenu_accel.now;
				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);
			#if ENABLED(DISTINCT_E_FACTORS)
    		LOOP_L_N(i, E_STEPPERS){
					if (DwinMenu_accel.index == MAXACCL_CASE_E + i) Item_Accel_MaxE(MROWS, i);
				}
			#endif			
			}
			else {
				Move_Highlight(1, DwinMenu_accel.now + MROWS - DwinMenu_accel.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_accel.dec()) {			
			if (DwinMenu_accel.now < DwinMenu_accel.index - MROWS) {
				DwinMenu_feedrate.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if (DwinMenu_accel.index - MROWS == MAXACCL_CASE_BACK) Draw_Back_First();    		
				else if (DwinMenu_accel.index - MROWS == MAXACCL_CASE_X) Item_Accel_MaxX(0);
				else if (DwinMenu_accel.index - MROWS == MAXACCL_CASE_Y) Item_Accel_MaxY(0);				
				else if (DwinMenu_accel.index - MROWS == MAXACCL_CASE_Z) Item_Accel_MaxZ(0);				
			}
			else {
				Move_Highlight(-1, DwinMenu_accel.now + MROWS - DwinMenu_accel.index);
			}
		}
	}	
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		if (WITHIN(DwinMenu_accel.now, MAXACCL_CASE_X, MAXACCL_CASE_TOTAL)) {
			DwinMenuID = DWMENU_SET_MAXACC_VALUE;
			HMI_flag.axis = AxisEnum(DwinMenu_accel.now - 1);
			HMI_Value.Max_Acceleration = planner.settings.max_acceleration_mm_per_s2[HMI_flag.axis];
			DWIN_Draw_Select_IntValue_Default(5, CONFIGVALUE_X-8, MBASE(DwinMenu_accel.now + MROWS - DwinMenu_accel.index), HMI_Value.Max_Acceleration);
			EncoderRate.enabled = true;
		}
		else { // Back
			Draw_Motion_Menu(MOTION_CASE_ACCEL);
		}
	}
	dwinLCD.UpdateLCD();
}

///////////////////////////////////////////////
//
// Control >> Motion >> Jerk
//
#if HAS_CLASSIC_JERK
inline void Item_Jerk_MaxX(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXSPEEDJERKX);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_JERK, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+6, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_X, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+get_MultiLanguageString_Width(MTSTRING_JERK)+12, MBASE(row));
	DWIN_Draw_Small_Float21(CONFIGVALUE_X+8, MBASE(row), planner.max_jerk[X_AXIS] * MINUNITMULT);
}

inline void Item_Jerk_MaxY(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXSPEEDJERKY);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_JERK, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+6, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Y, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+get_MultiLanguageString_Width(MTSTRING_JERK)+12, MBASE(row));
	DWIN_Draw_Small_Float21(CONFIGVALUE_X+8, MBASE(row), planner.max_jerk[Y_AXIS] * MINUNITMULT);
}

inline void Item_Jerk_MaxZ(uint8_t row){
	Draw_Menu_Line(row, ICON_MAXSPEEDJERKZ);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_JERK, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+6, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z, LBLX+get_MultiLanguageString_Width(MTSTRING_MAX)+get_MultiLanguageString_Width(MTSTRING_JERK)+12, MBASE(row));
	DWIN_Draw_Small_Float21(CONFIGVALUE_X+8, MBASE(row), planner.max_jerk[Z_AXIS] * MINUNITMULT);
}

inline void Item_Jerk_MaxE(uint8_t row){
	uint16_t x = LBLX;
	Draw_Menu_Line(row, ICON_MAXSPEEDJERKE);
	DWIN_Show_MultiLanguage_String(MTSTRING_MAX, x, MBASE(row));
	x += get_MultiLanguageString_Width(MTSTRING_MAX) + 6;
	DWIN_Show_MultiLanguage_String(MTSTRING_JERK, x, MBASE(row));
	x += get_MultiLanguageString_Width(MTSTRING_JERK) + 6;
	DWIN_Show_MultiLanguage_String(MTSTRING_E, x, MBASE(row));
	DWIN_Draw_Small_Float21(CONFIGVALUE_X+8, MBASE(row), planner.max_jerk[E_AXIS] * MINUNITMULT);
}

inline void Draw_Max_Jerk_Menu() {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	DwinMenu_jerk.reset();

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_JERK, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	Draw_Back_First(DwinMenu_jerk.now == 0);
	Item_Jerk_MaxX(MAXJERK_CASE_X);
	Item_Jerk_MaxY(MAXJERK_CASE_Y);
	Item_Jerk_MaxZ(MAXJERK_CASE_Z);
	Item_Jerk_MaxE(MAXJERK_CASE_E);
	if (DwinMenu_jerk.now) Draw_Menu_Cursor(DwinMenu_jerk.now); 
}


void HMI_Adjust_MaxJerk() {
	#if ENABLED(LIMITED_JERK_EDITING)
	constexpr float max_jerk_edit[]      = MAX_JERK_EDIT_VALUES;
	#else
	constexpr float max_jerk_edit[]      = { DEFAULT_XJERK*2, DEFAULT_YJERK*2, DEFAULT_ZJERK*2, DEFAULT_EJERK*2 };
	#endif
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Max_Jerk)) {
			DwinMenuID = DWMENU_SET_MAXJERK;
			EncoderRate.enabled = false;
			if(WITHIN(HMI_flag.axis, X_AXIS, E_AXIS))
				planner.set_max_jerk(HMI_flag.axis, ((float)HMI_Value.Max_Jerk / MINUNITMULT));
				DWIN_Draw_Small_Float21(CONFIGVALUE_X+8, MBASE(DwinMenu_jerk.now + MROWS - DwinMenu_jerk.index), HMI_Value.Max_Jerk);
		}
		else {
			// Max Jerk limit
			if (WITHIN(HMI_flag.axis, X_AXIS, E_AXIS)){
				NOMORE(HMI_Value.Max_Jerk, max_jerk_edit[HMI_flag.axis] * MINUNITMULT);
				NOLESS(HMI_Value.Max_Jerk, (MIN_MAXJERK) * MINUNITMULT);
			}
			// Max Jerk value
			DWIN_Draw_Selected_Small_Float21(CONFIGVALUE_X+8, MBASE(DwinMenu_jerk.now + MROWS - DwinMenu_jerk.index), HMI_Value.Max_Jerk);
		}
		dwinLCD.UpdateLCD();
	}
}

 /* Max Jerk */
void HMI_MaxJerk() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_jerk.inc(MAXJERK_CASE_END)) Move_Highlight(1, DwinMenu_jerk.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_jerk.dec()) Move_Highlight(-1, DwinMenu_jerk.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		if (WITHIN(DwinMenu_jerk.now, MAXJERK_CASE_X, MAXJERK_CASE_TOTAL)) {
			DwinMenuID = DWMENU_SET_MAXJERK_VALUE;
			HMI_flag.axis = AxisEnum(DwinMenu_jerk.now - 1);
			HMI_Value.Max_Jerk = planner.max_jerk[HMI_flag.axis] * MINUNITMULT;
			DWIN_Draw_Selected_Small_Float21(CONFIGVALUE_X+8, MBASE(DwinMenu_jerk.now + MROWS - DwinMenu_jerk.index), HMI_Value.Max_Jerk);
			EncoderRate.enabled = true;
		}
		else { // Back 
			Draw_Motion_Menu(MOTION_CASE_JERK);
		}
	}
	dwinLCD.UpdateLCD();
}
#endif // HAS_CLASSIC_JERK

///////////////////////////////////////////////
//
// Control >> Motion >> Step/mm
//
inline void Item_Steps_X(uint8_t row){
	Draw_Menu_Line(row, ICON_STEPX);
	DWIN_Show_MultiLanguage_String(MTSTRING_STEPPERMM, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_X, LBLX+get_MultiLanguageString_Width(MTSTRING_STEPPERMM)+6, MBASE(row));
	DWIN_Draw_Small_Float41(CONFIGVALUE_X-8, MBASE(row), planner.settings.axis_steps_per_mm[X_AXIS] * MINUNITMULT);
}

inline void Item_Steps_Y(uint8_t row){
	Draw_Menu_Line(row, ICON_STEPY);
	DWIN_Show_MultiLanguage_String(MTSTRING_STEPPERMM, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Y, LBLX+get_MultiLanguageString_Width(MTSTRING_STEPPERMM)+6, MBASE(row));
	DWIN_Draw_Small_Float41(CONFIGVALUE_X-8, MBASE(row), planner.settings.axis_steps_per_mm[Y_AXIS] * MINUNITMULT);
}

inline void Item_Steps_Z(uint8_t row){
	Draw_Menu_Line(row, ICON_STEPZ);
	DWIN_Show_MultiLanguage_String(MTSTRING_STEPPERMM, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_Z, LBLX+get_MultiLanguageString_Width(MTSTRING_STEPPERMM)+6, MBASE(row));
	DWIN_Draw_Small_Float41(CONFIGVALUE_X-8, MBASE(row), planner.settings.axis_steps_per_mm[Z_AXIS] * MINUNITMULT);
}

inline void Item_Steps_E(uint8_t row, uint8_t e = 0){
	uint16_t x = LBLX;
	Draw_Menu_Line(row, ICON_STEPE);
	DWIN_Show_MultiLanguage_String(MTSTRING_STEPPERMM, x, MBASE(row));
	x += get_MultiLanguageString_Width(MTSTRING_STEPPERMM) + 6;
	DWIN_Show_MultiLanguage_String(MTSTRING_E, x, MBASE(row));
#if ENABLED(DISTINCT_E_FACTORS)	
	x += get_MultiLanguageString_Width(MTSTRING_E);
	DWIN_Show_MultiLanguage_String(MTSTRING_1 + e, x, MBASE(row));
#endif
	DWIN_Draw_Small_Float41(CONFIGVALUE_X-8, MBASE(row), planner.settings.axis_steps_per_mm[E_AXIS + e] * MINUNITMULT);
}


inline void Draw_Steps_Menu() {
	DwinMenu_step.reset();
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	HMI_flag.axis = X_AXIS;
	HMI_Value.Steps_per_mm = planner.settings.axis_steps_per_mm[0] * MINUNITMULT;
	
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_STEP, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	
	Draw_Back_First(DwinMenu_step.now == STEPSMM_CASE_BACK);
	Item_Steps_X(STEPSMM_CASE_X);
	Item_Steps_Y(STEPSMM_CASE_Y);
	Item_Steps_Z(STEPSMM_CASE_Z);
#if ENABLED(DISTINCT_E_FACTORS)
	LOOP_L_N(i, E_STEPPERS){
		if(WITHIN(STEPSMM_CASE_E + i, 0, MROWS))	Item_Steps_E(STEPSMM_CASE_E + i, i);
	}	
#else
	Item_Steps_E(STEPSMM_CASE_E);
#endif
	if (DwinMenu_step.now) Draw_Menu_Cursor(DwinMenu_step.now);
}

void HMI_Adjust_Steppermm() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.Steps_per_mm)) {
			DwinMenuID = DWMENU_SET_STEPPREMM;
			EncoderRate.enabled = false;
			if (WITHIN(HMI_flag.axis, X_AXIS, XYZE_N))
				planner.settings.axis_steps_per_mm[HMI_flag.axis] = (float)HMI_Value.Steps_per_mm / MINUNITMULT;
			DWIN_Draw_Small_Float41(CONFIGVALUE_X-8, MBASE(DwinMenu_step.now + MROWS - DwinMenu_step.index), HMI_Value.Steps_per_mm);
		}
		else {
			// Step/mm limit
			if (WITHIN(HMI_flag.axis, X_AXIS, XYZE_N)){
				NOMORE(HMI_Value.Steps_per_mm, default_axis_steps_per_unit[HMI_flag.axis < E_AXIS?HMI_flag.axis : E_AXIS]*4*MINUNITMULT);
				NOLESS(HMI_Value.Steps_per_mm, default_axis_steps_per_unit[HMI_flag.axis < E_AXIS?HMI_flag.axis : E_AXIS]);
			}
			// Step/mm value
			DWIN_Draw_Selected_Small_Float41(CONFIGVALUE_X-8, MBASE(DwinMenu_step.now + MROWS - DwinMenu_step.index), HMI_Value.Steps_per_mm);
		}
		dwinLCD.UpdateLCD();
	}
}

/* Step */
void HMI_StepPermm() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;
	
	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_step.inc(STEPSMM_CASE_END)) {
			if (DwinMenu_step.now > MROWS && DwinMenu_step.now > DwinMenu_step.index) {
				DwinMenu_step.index = DwinMenu_step.now;
				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);
			#if ENABLED(DISTINCT_E_FACTORS)
    		LOOP_L_N(i, E_STEPPERS){
					if (DwinMenu_step.index == STEPSMM_CASE_E + i) Item_Steps_E(MROWS, i);
				}
			#endif			
			}
			else {
				Move_Highlight(1, DwinMenu_step.now + MROWS - DwinMenu_step.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_step.dec()) {			
			if (DwinMenu_step.now < DwinMenu_step.index - MROWS) {
				DwinMenu_step.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if (DwinMenu_step.index - MROWS == STEPSMM_CASE_BACK) Draw_Back_First();    		
				else if (DwinMenu_step.index - MROWS == STEPSMM_CASE_X) Item_Steps_X(0);
				else if (DwinMenu_step.index - MROWS == STEPSMM_CASE_Y) Item_Steps_Y(0);				
				else if (DwinMenu_step.index - MROWS == STEPSMM_CASE_Z) Item_Steps_Z(0);				
			}
			else {
				Move_Highlight(-1, DwinMenu_step.now + MROWS - DwinMenu_step.index);
			}
		}
	} 
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		if (WITHIN(DwinMenu_step.now, STEPSMM_CASE_X, STEPSMM_CASE_TOTAL)) {
			DwinMenuID = DWMENU_SET_STEPPREMM_VALUE;
			HMI_flag.axis = AxisEnum(DwinMenu_step.now - 1);
			HMI_Value.Steps_per_mm = planner.settings.axis_steps_per_mm[HMI_flag.axis] * MINUNITMULT;
			DWIN_Draw_Selected_Small_Float41(CONFIGVALUE_X-8, MBASE(DwinMenu_step.now + MROWS - DwinMenu_step.index), HMI_Value.Steps_per_mm);
			EncoderRate.enabled = true;
		}
		else { // Back   
			Draw_Motion_Menu(MOTION_CASE_STEPS);
		}
	}
	dwinLCD.UpdateLCD();
}


void HMI_Motion() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		
		if (DwinMenu_motion.inc(MOTION_CASE_END)){ 			
			Move_Highlight(1, DwinMenu_motion.now);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {		
		if (DwinMenu_motion.dec()){ 
			Move_Highlight(-1, DwinMenu_motion.now);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_motion.now) {
		case MOTION_CASE_BACK: 			// Back
			Draw_Control_Menu(CONTROL_CASE_MOTION + mixing_menu_adjust());
		break;
		
		case MOTION_CASE_FEEDRATE:  // Max speed
			DwinMenuID = DWMENU_SET_MAXSPEED;
			DwinMenu_feedrate.reset();
			Draw_Max_Feedrate_Menu();
		break;
		
		case MOTION_CASE_ACCEL: 		// Max acceleration
			DwinMenuID = DWMENU_SET_MAXACC;
			DwinMenu_accel.reset();
			Draw_Max_Accel_Menu();
		break;
		
	#if HAS_CLASSIC_JERK
		case MOTION_CASE_JERK: 			// Max jerk
			DwinMenuID = DWMENU_SET_MAXJERK;
			DwinMenu_jerk.reset();
			Draw_Max_Jerk_Menu();
		break;
	#endif
	
		case MOTION_CASE_STEPS: 		// Steps per mm
			DwinMenuID = DWMENU_SET_STEPPREMM;
			DwinMenu_step.reset();
			Draw_Steps_Menu();
		break;
		
		default: break;
		}
	}
	dwinLCD.UpdateLCD();
}

///////////////////////////////////////////////
//
// Control >> BLTouch
//
#if ENABLED(BLTOUCH)
inline void Draw_Bltouch_Menu() {
	DwinMenuID = DWMENU_SET_BLTOUCH;
	DwinMenu_bltouch.reset();
	//DwinMenu_bltouch.index = _MAX(DwinMenu_bltouch.now, MROWS);
	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_BLTOUCH, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);

	Draw_Back_First(DwinMenu_bltouch.now == 0);
	DWIN_Show_MultiLanguage_String(MTSTRING_BLTOUCH_RESET, LBLX, MBASE(BLTOUCH_CASE_RESET));
	Draw_Menu_Line(BLTOUCH_CASE_RESET,ICON_BLTOUCH_RESET);
	DWIN_Show_MultiLanguage_String(MTSTRING_BLTOUCH_TEST, LBLX, MBASE(BLTOUCH_CASE_TEST));
	Draw_Menu_Line(BLTOUCH_CASE_TEST,ICON_BLTOUCH_TEST);
	DWIN_Show_MultiLanguage_String(MTSTRING_BLTOUCH_STOW, LBLX, MBASE(BLTOUCH_CASE_STOW));
	Draw_Menu_Line(BLTOUCH_CASE_STOW,ICON_BLTOUCH_STOW);	
	DWIN_Show_MultiLanguage_String(MTSTRING_BLTOUCH_DEPLOY, LBLX, MBASE(BLTOUCH_CASE_DEPLOY));
	Draw_Menu_Line(BLTOUCH_CASE_DEPLOY,ICON_BLTOUCH_DEPLOY);	
	DWIN_Show_MultiLanguage_String(MTSTRING_BLTOUCH_MODE, LBLX, MBASE(BLTOUCH_CASE_SW));
	Draw_Menu_Line(BLTOUCH_CASE_SW,ICON_BLTOUCH_SW);
	if (DwinMenu_bltouch.now) Draw_Menu_Cursor(DwinMenu_bltouch.now);	
}

void HMI_Option_Bltouch() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_bltouch.inc(BLTOUCH_CASE_END)) Move_Highlight(1, DwinMenu_bltouch.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_bltouch.dec()) Move_Highlight(-1, DwinMenu_bltouch.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_bltouch.now) {
			case BLTOUCH_CASE_BACK: 	// Back
				Draw_Control_Menu(CONTROL_CASE_BLTOUCH + mixing_menu_adjust());
			break;

			case BLTOUCH_CASE_RESET: 	// Reset
				DwinMenuID = DWMENU_SET_BLTOUCH;
				bltouch._reset();
			break;

			case BLTOUCH_CASE_TEST: 	// Test
					DwinMenuID = DWMENU_SET_BLTOUCH;
					bltouch._selftest();
			break;

			case BLTOUCH_CASE_STOW: 	// Stow
					DwinMenuID = DWMENU_SET_BLTOUCH;
					bltouch._stow();
			break;

			case BLTOUCH_CASE_DEPLOY: 	// Proc
					DwinMenuID = DWMENU_SET_BLTOUCH;
					bltouch._deploy();
			break;
			
			case BLTOUCH_CASE_SW: 	// sw
				DwinMenuID = DWMENU_SET_BLTOUCH;
				bltouch._set_SW_mode();
			break;
			
			default: break;
		}
	}
	dwinLCD.UpdateLCD();
}
#endif

///////////////////////////////////////////////
//
//control >> Configre >> Home offset
//
#if HAS_OFFSET_MENU
static void Item_HomeOffset_X(const uint8_t row) {
	HMI_Value.HomeOffsetX_scale = home_offset.x * MINUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Home Offset X"));
	DWIN_Draw_Small_Float21(MENUVALUE_X+8, MBASE(row), HMI_Value.HomeOffsetX_scale);
	Draw_Menu_Line(row, ICON_HOME_X);
}

static void Item_HomeOffset_Y(const uint8_t row) {
	HMI_Value.HomeOffsetY_scale = home_offset.y * MINUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Home Offset Y"));
	DWIN_Draw_Small_Float21(MENUVALUE_X+8, MBASE(row), HMI_Value.HomeOffsetY_scale);
	Draw_Menu_Line(row, ICON_HOME_Y);
}

static void Item_HomeOffset_Z(const uint8_t row) {
	HMI_Value.HomeOffsetZ_scale = home_offset.z * MAXUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Home Offset Z"));
	DWIN_Draw_Small_Float22(MENUVALUE_X+8, MBASE(row), HMI_Value.HomeOffsetZ_scale);
	Draw_Menu_Line(row, ICON_HOME_Z);
}

inline void Draw_HomeOffset_Menu() {
	DwinMenuID = DWMENU_SET_HOMEOFFSET;
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	DwinMenu_Homeoffset.reset();		
	//title
	DWIN_Draw_UnMaskString_Default(14, 7, PSTR("HOME OFFSET")); 
	Draw_Back_First(DwinMenu_Homeoffset.now == HOMEOFFSET_CASE_BACK);	
	Item_HomeOffset_X(HOMEOFFSET_CASE_X);
	Item_HomeOffset_Y(HOMEOFFSET_CASE_Y);
	Item_HomeOffset_Z(HOMEOFFSET_CASE_Z);
	if (DwinMenu_Homeoffset.now) Draw_Menu_Cursor(DwinMenu_Homeoffset.now);	
}

void HMI_Adjust_HomeOffset_X() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.HomeOffsetX_scale)) {
			DwinMenuID = DWMENU_SET_HOMEOFFSET;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Homeoffset.index + HOMEOFFSET_CASE_X), HMI_Value.HomeOffsetX_scale);
			home_offset.x = (float)HMI_Value.HomeOffsetX_scale/MINUNITMULT;		
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.HomeOffsetX_scale, -50 * MINUNITMULT);
			NOMORE(HMI_Value.HomeOffsetX_scale, 50 * MINUNITMULT);		
			DWIN_Draw_Selected_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Homeoffset.index + HOMEOFFSET_CASE_X), HMI_Value.HomeOffsetX_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_HomeOffset_Y() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.HomeOffsetY_scale)) {
			DwinMenuID = DWMENU_SET_HOMEOFFSET;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Homeoffset.index + HOMEOFFSET_CASE_Y), HMI_Value.HomeOffsetY_scale);
			home_offset.y = (float)HMI_Value.HomeOffsetY_scale/MINUNITMULT;			
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.HomeOffsetY_scale, -50 * MINUNITMULT);
			NOMORE(HMI_Value.HomeOffsetY_scale, 50 * MINUNITMULT);		
			DWIN_Draw_Selected_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Homeoffset.index + HOMEOFFSET_CASE_Y), HMI_Value.HomeOffsetY_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_HomeOffset_Z() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.HomeOffsetZ_scale)) {
			DwinMenuID = DWMENU_SET_HOMEOFFSET;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float22(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Homeoffset.index + HOMEOFFSET_CASE_Z), HMI_Value.HomeOffsetZ_scale);
			home_offset.z = (float)HMI_Value.HomeOffsetZ_scale/MAXUNITMULT;
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.HomeOffsetZ_scale, -20 * MAXUNITMULT);
			NOMORE(HMI_Value.HomeOffsetZ_scale, 20 * MAXUNITMULT);		
			DWIN_Draw_Selected_Small_Float22(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Homeoffset.index + HOMEOFFSET_CASE_Z), HMI_Value.HomeOffsetZ_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_HomeOffset(){
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return; 

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_Homeoffset.inc(HOMEOFFSET_CASE_END)) {
			if (DwinMenu_Homeoffset.now > MROWS && DwinMenu_Homeoffset.now > DwinMenu_Homeoffset.index) {				
				// Scroll up and draw a blank bottom line
				DwinMenu_Homeoffset.index = DwinMenu_Homeoffset.now;
				Scroll_Menu(DWIN_SCROLL_UP);
			}
			else {
				Move_Highlight(1, DwinMenu_Homeoffset.now + MROWS - DwinMenu_Homeoffset.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_Homeoffset.dec()) {
			if (DwinMenu_Homeoffset.now < DwinMenu_Homeoffset.index - MROWS) {
				DwinMenu_Homeoffset.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
			}
			else {
				Move_Highlight(-1, DwinMenu_Homeoffset.now + MROWS - DwinMenu_Homeoffset.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_fwretract.now) {
			case HOMEOFFSET_CASE_BACK: 		// Back				
				Draw_Config_Menu(CONFIG_CASE_HOMEOFFSET);				
			break;
	 
			case HOMEOFFSET_CASE_X:  			// HOME OFFSET X
				DwinMenuID = DWMENU_SET_HOMEOFFSET_X;
				HMI_Value.HomeOffsetX_scale= home_offset.x * MINUNITMULT;
				DWIN_Draw_Selected_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Homeoffset.index + HOMEOFFSET_CASE_X), HMI_Value.HomeOffsetX_scale);
				EncoderRate.enabled = true;
			break;

			case HOMEOFFSET_CASE_Y:  			// HOME OFFSET Y
				DwinMenuID = DWMENU_SET_HOMEOFFSET_Y;
				HMI_Value.HomeOffsetY_scale = home_offset.y * MINUNITMULT;
				DWIN_Draw_Selected_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Homeoffset.index + HOMEOFFSET_CASE_Y), HMI_Value.HomeOffsetY_scale);
				EncoderRate.enabled = true;
			break;

			case HOMEOFFSET_CASE_Z:  			// HOME OFFSET Z
				DwinMenuID = DWMENU_SET_HOMEOFFSET_Z;
				HMI_Value.HomeOffsetZ_scale = home_offset.z * MAXUNITMULT;
				DWIN_Draw_Selected_Small_Float22(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Homeoffset.index + HOMEOFFSET_CASE_Z), HMI_Value.HomeOffsetZ_scale);
				EncoderRate.enabled = true;
			break;
			
		 default: break;
		}
	}
	dwinLCD.UpdateLCD();
}
#endif

#if HAS_PROBE_XY_OFFSET
static void Item_ProbeOffset_X(const uint8_t row) {
	HMI_Value.ProbeOffsetX_scale = probe.offset.x * MINUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Probe Offset X"));
	DWIN_Draw_Small_Float21(MENUVALUE_X+8, MBASE(row), HMI_Value.ProbeOffsetX_scale);
	Draw_Menu_Line(row, ICON_HOME_X);
}

static void Item_ProbeOffset_Y(const uint8_t row) {
	HMI_Value.ProbeOffsetY_scale = probe.offset.y * MINUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Probe Offset Y"));
	DWIN_Draw_Small_Float21(MENUVALUE_X+8, MBASE(row), HMI_Value.ProbeOffsetY_scale);
	Draw_Menu_Line(row, ICON_HOME_Y);
}

static void Item_ProbeOffset_Z(const uint8_t row) {
	HMI_Value.ProbeOffsetZ_scale = probe.offset.z * MAXUNITMULT;
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Probe Offset Z"));
	DWIN_Draw_Small_Float22(MENUVALUE_X+8, MBASE(row), HMI_Value.ProbeOffsetZ_scale);
	Draw_Menu_Line(row, ICON_HOME_Z);
}

inline void Draw_ProbeOffset_Menu() {
	DwinMenuID = DWMENU_SET_PROBEOFFSET;
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	DwinMenu_Probeoffset.reset();		
	//title
	DWIN_Draw_UnMaskString_Default(14, 7, PSTR("PROBE OFFSET")); 
	Draw_Back_First(DwinMenu_Probeoffset.now == HOMEOFFSET_CASE_BACK);	
	Item_ProbeOffset_X(PROBEOFFSET_CASE_X);
	Item_ProbeOffset_Y(PROBEOFFSET_CASE_Y);
	Item_ProbeOffset_Z(PROBEOFFSET_CASE_Z);
	if (DwinMenu_Probeoffset.now) Draw_Menu_Cursor(DwinMenu_Probeoffset.now);	
}

void HMI_Adjust_ProbeOffset_X() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.ProbeOffsetX_scale)) {
			DwinMenuID = DWMENU_SET_PROBEOFFSET;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Probeoffset.index + PROBEOFFSET_CASE_X), HMI_Value.ProbeOffsetX_scale);
			probe.offset.x = (float)HMI_Value.ProbeOffsetX_scale/MINUNITMULT;		
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.ProbeOffsetX_scale, -50 * MINUNITMULT);
			NOMORE(HMI_Value.ProbeOffsetX_scale, 50 * MINUNITMULT);		
			DWIN_Draw_Selected_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Probeoffset.index + PROBEOFFSET_CASE_X), HMI_Value.ProbeOffsetX_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_ProbeOffset_Y() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.ProbeOffsetY_scale)) {
			DwinMenuID = DWMENU_SET_PROBEOFFSET;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Probeoffset.index + PROBEOFFSET_CASE_Y), HMI_Value.ProbeOffsetY_scale);
			probe.offset.y = (float)HMI_Value.ProbeOffsetY_scale/MINUNITMULT;			
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.ProbeOffsetY_scale, -50 * MINUNITMULT);
			NOMORE(HMI_Value.ProbeOffsetY_scale, 50 * MINUNITMULT);		
			DWIN_Draw_Selected_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Probeoffset.index + PROBEOFFSET_CASE_Y), HMI_Value.ProbeOffsetY_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_ProbeOffset_Z() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.ProbeOffsetZ_scale)) {
			DwinMenuID = DWMENU_SET_PROBEOFFSET;
			EncoderRate.enabled = false;
			DWIN_Draw_Small_Float22(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Probeoffset.index + PROBEOFFSET_CASE_Z), HMI_Value.ProbeOffsetZ_scale);
			probe.offset.z = (float)HMI_Value.ProbeOffsetZ_scale/MAXUNITMULT;
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(HMI_Value.ProbeOffsetZ_scale, (-20) * MAXUNITMULT);
			NOMORE(HMI_Value.ProbeOffsetZ_scale, (20) * MAXUNITMULT);		
			DWIN_Draw_Selected_Small_Float22(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Probeoffset.index + PROBEOFFSET_CASE_Z), HMI_Value.ProbeOffsetZ_scale);
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_Adjust_ProbeOffset(){
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return; 

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_Probeoffset.inc(PROBEOFFSET_CASE_END)) {
			if (DwinMenu_Probeoffset.now > MROWS && DwinMenu_Probeoffset.now > DwinMenu_Probeoffset.index) {				
				// Scroll up and draw a blank bottom line
				DwinMenu_Probeoffset.index = DwinMenu_Probeoffset.now;
				Scroll_Menu(DWIN_SCROLL_UP);
			}
			else {
				Move_Highlight(1, DwinMenu_Probeoffset.now + MROWS - DwinMenu_Probeoffset.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_Probeoffset.dec()) {
			if (DwinMenu_Probeoffset.now < DwinMenu_Probeoffset.index - MROWS) {
				DwinMenu_Probeoffset.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
			}
			else {
				Move_Highlight(-1, DwinMenu_Probeoffset.now + MROWS - DwinMenu_Probeoffset.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_Probeoffset.now) {
			case PROBEOFFSET_CASE_BACK: 		// Back				
				Draw_Config_Menu(CONFIG_CASE_PROBEOFFSET);				
			break;
	 
			case PROBEOFFSET_CASE_X:  			// Probe OFFSET X
				DwinMenuID = DWMENU_SET_PROBEOFFSET_X;
				HMI_Value.HomeOffsetX_scale= probe.offset.x * MINUNITMULT;
				DWIN_Draw_Selected_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Probeoffset.index + PROBEOFFSET_CASE_X), HMI_Value.HomeOffsetX_scale);
				EncoderRate.enabled = true;
			break;

			case PROBEOFFSET_CASE_Y:  			// Probe OFFSET Y
				DwinMenuID = DWMENU_SET_PROBEOFFSET_Y;
				HMI_Value.HomeOffsetY_scale = probe.offset.y * MINUNITMULT;
				DWIN_Draw_Selected_Small_Float21(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Probeoffset.index + PROBEOFFSET_CASE_Y), HMI_Value.HomeOffsetY_scale);
				EncoderRate.enabled = true;
			break;

			case PROBEOFFSET_CASE_Z:  			// Probe OFFSET Z
				DwinMenuID = DWMENU_SET_PROBEOFFSET_Z;
				HMI_Value.ProbeOffsetZ_scale = probe.offset.z * MAXUNITMULT;
				DWIN_Draw_Selected_Small_Float22(MENUVALUE_X+8, MBASE(MROWS -DwinMenu_Probeoffset.index + PROBEOFFSET_CASE_Z), HMI_Value.ProbeOffsetZ_scale);
				EncoderRate.enabled = true;
			break;
			
		 default: break;
		}
	}
	dwinLCD.UpdateLCD();
}
#endif

#if (ENABLED(PID_AUTOTUNE_MENU) && DISABLED(PID_EDIT_MENU))
void HMI_PID_AutoTune() {
	char gcode_string[50]={0};	
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze(); 
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.PIDAutotune_Temp)) {
			DwinMenuID = DWMENU_CONFIG;
			EncoderRate.enabled = false;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_configure.index + CONFIG_CASE_PIDTUNE), HMI_Value.PIDAutotune_Temp);			
			DWIN_FEEDBACK_CONFIRM();
			sprintf_P(gcode_string,PSTR("M303 S%3d E0 C%1d U1\nM500"),HMI_Value.PIDAutotune_Temp, HMI_Value.PIDAutotune_cycles);
			queue.inject(gcode_string);			
		}
		else {
			NOLESS(HMI_Value.PIDAutotune_Temp, EXTRUDE_MINTEMP);
			NOMORE(HMI_Value.PIDAutotune_Temp, (HEATER_0_MAXTEMP - HOTEND_OVERSHOOT));		
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_configure.index + CONFIG_CASE_PIDTUNE), HMI_Value.PIDAutotune_Temp);
		}
		dwinLCD.UpdateLCD();
	}
}
#endif


#if BOTH(OPTION_WIFI_MODULE, OPTION_WIFI_BAUDRATE)
void HMI_Adjust_WiFi_BaudRate(){
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze(); 
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_uint8(encoder_diffState, &WiFi_BaudRate)) {
			DwinMenuID = DWMENU_CONFIG;
			EncoderRate.enabled = false;
			DWIN_Draw_IntValue_Default(6, MENUVALUE_X-16, MBASE(CONFIG_CASE_WIFISPEED + MROWS -DwinMenu_configure.index), Table_Baudrate[WiFi_BaudRate]);
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Reset machine to apply!"), 5);
			HMI_AudioFeedback(settings.save());
		}
		else {
			NOLESS(WiFi_BaudRate, 0);
			NOMORE(WiFi_BaudRate, 3);
			DWIN_Draw_Select_IntValue_Default(6, MENUVALUE_X-16, MBASE(CONFIG_CASE_WIFISPEED + MROWS -DwinMenu_configure.index), Table_Baudrate[WiFi_BaudRate]);
		}
		dwinLCD.UpdateLCD();		
	}
}
#endif

#if BOTH(OPTION_WIFI_MODULE, OPTION_WIFI_QRCODE)
void Popup_Window_WiFiLink(char *strIPAddress) {
	Clear_Dwin_Area(AREA_MENU|AREA_BOTTOM);
	Draw_Popup_Bkgd_60();
	DwinMenuID = DWMENU_POP_WIFILINK;
	if(strcmp(strIPAddress, "192.168.0.1") == 0)
		DWIN_Draw_MaskString_Default_PopMenu((272-strlen("WiFi hotspot:ZONESTAR3DP")*10)/2, 75, PSTR("WiFi hotspot:ZONESTAR3DP"));		
	else
		DWIN_Draw_MaskString_Default_PopMenu((272-strlen("Connect to router")*10)/2, 75, PSTR("Connect to router"));	
	DWIN_Draw_MaskString_Default_PopMenu((272-strlen("Scan the QR code")*10)/2,95 , PSTR("Scan the QR code"));
	DWIN_Draw_MaskString_Default_PopMenu((272-strlen("to control by WiFi")*10)/2,115 , PSTR("to control by WiFi"));
	dwinLCD.Draw_QRCode((272-46*3)/2, 145, 3, strIPAddress);		
	DWIN_Draw_MaskString_Default_PopMenu((272-strlen("Press the knob to exit")*10)/2, 145+46*3+15, PSTR("Press the knob to exit"));
}

void HMI_Pop_WiFiLink() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();	
	if(encoder_diffState == ENCODER_DIFF_NO) return;
	else if(encoder_diffState == ENCODER_DIFF_ENTER){		
		Draw_Config_Menu(CONFIG_CASE_WIFI);
	}
}
#endif


#if ENABLED(SWITCH_EXTRUDER_MENU)
void HMI_Adjust_Extruder_Sequence() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze(); 
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int8(encoder_diffState, &HMI_Value.switchExtruder)) {
			DwinMenuID = DWMENU_CONFIG;
			EncoderRate.enabled = false;
			DWIN_Draw_MaskString_Default(216-12*MIXING_STEPPERS, MBASE(CONFIG_CASE_SWITCHEXTRUDER + MROWS - DwinMenu_configure.index), _getString_SwitchExtruder());
			switch_extruder_sequence();
		}
		else {
			NOLESS(HMI_Value.switchExtruder, SE_DEFAULT);			
			NOMORE(HMI_Value.switchExtruder, SE_END-1);
			DWIN_Draw_MaskString_Default_Color(COLOR_BLUE, 216-12*MIXING_STEPPERS, MBASE(CONFIG_CASE_SWITCHEXTRUDER + MROWS - DwinMenu_configure.index), _getString_SwitchExtruder());
		}
		dwinLCD.UpdateLCD();		
	}
}
#endif

bool _break_on_printing(void){
	if(IS_SD_PRINTING() || IS_SD_PAUSED() || IS_SD_FILE_OPEN()){
		DWIN_Show_Status_Message(COLOR_RED, PSTR("Can't change on printing!"));
		DWIN_FEEDBACK_WARNNING();
		return true;
	}
	return false;
}

void HMI_Config() {
 ENCODER_DiffState encoder_diffState = get_encoder_state();
 if (encoder_diffState == ENCODER_DIFF_NO) return;

 // Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if(DwinMenu_configure.inc(CONFIG_CASE_END)){
			if (DwinMenu_configure.now > MROWS && DwinMenu_configure.now > DwinMenu_configure.index) {
				DwinMenu_configure.index = DwinMenu_configure.now;
				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);

				if(DwinMenu_configure.index == CONFIG_CASE_BACK){}
				
			#if ENABLED(FWRETRACT)
				else if(DwinMenu_configure.index == CONFIG_CASE_RETRACT) Item_Config_Retract(MROWS);
			#endif

			#if ENABLED(LIN_ADVANCE) 
				else if(DwinMenu_configure.index == CONFIG_CASE_LINADVANCE) Item_Config_LinearAdvance(MROWS);
			#endif
			
			#if ENABLED(OPTION_REPEAT_PRINTING) 
				else if(DwinMenu_configure.index == CONFIG_CASE_REPRINT) Item_Config_Reprint(MROWS);
			#endif
			
			#if ENABLED(FILAMENT_RUNOUT_SENSOR)
				else if(DwinMenu_configure.index == CONFIG_CASE_FILAMENTRUNOUT) Item_Config_FilamentRunOut(MROWS);
			#endif

			#if ENABLED(OPTION_ABORT_UNLOADFILAMENT)
				else if(DwinMenu_configure.index == CONFIG_CASE_FILAMENTAUTOUNLOAD) Item_Config_FilamentAutoUnload(MROWS);
			#endif
			
			#if ENABLED(POWER_LOSS_RECOVERY)
				else if(DwinMenu_configure.index == CONFIG_CASE_POWERLOSS) Item_Config_Powerloss(MROWS);
			#endif
			
			#if ENABLED(OPTION_AUTOPOWEROFF)
				else if(DwinMenu_configure.index == CONFIG_CASE_SHUTDOWN) Item_Config_Shutdown(MROWS);
			#endif
			
			#if ENABLED(CASE_LIGHT_MENU)
				else if(DwinMenu_configure.index == CONFIG_CASE_BACKLIGHT) Item_Config_Backlight(MROWS);
			#endif
			
			#if ENABLED(OPTION_WIFI_MODULE)
				else if(DwinMenu_configure.index == CONFIG_CASE_WIFI) Item_Config_Wifi(MROWS);
				#if ENABLED(OPTION_WIFI_BAUDRATE) 
				else if(DwinMenu_configure.index == CONFIG_CASE_WIFISPEED) Item_Config_WifiBaudrate(MROWS);
				#endif
			#endif	

			#if ENABLED(OPTION_LASER)
				else if(DwinMenu_configure.index == CONFIG_CASE_LASER) Item_Config_Laser(MROWS);
			#endif
			#if ENABLED(SPINDLE_FEATURE)
				else if(DwinMenu_configure.index == CONFIG_CASE_SPINDLE) Item_Config_Spindle(MROWS);
			#endif

			#if BOTH(MIXING_EXTRUDER, OPTION_MIXING_SWITCH)
			 else if(DwinMenu_configure.index == CONFIG_CASE_MIXERENABLE) 	Item_Config_MixerEnable(MROWS);
			 #if ENABLED(SWITCH_EXTRUDER_MENU)
			 else if(DwinMenu_configure.index == CONFIG_CASE_SWITCHEXTRUDER) 	Item_Config_SwitchExtruder(MROWS);			 
			 #endif
			#endif	
			
			#if HAS_OFFSET_MENU
				else if(DwinMenu_configure.index == CONFIG_CASE_HOMEOFFSET) Item_Config_HomeOffset(MROWS);
			#endif	
			
			#if HAS_LEVELING						
				#if HAS_PROBE_XY_OFFSET
				else if(DwinMenu_configure.index == CONFIG_CASE_PROBEOFFSET) Item_Config_ProbeOffset(MROWS);
				#endif
				else if(DwinMenu_configure.index == CONFIG_CASE_LEVELING) Item_Config_Leveling(MROWS);
				else if(DwinMenu_configure.index == CONFIG_CASE_ACTIVELEVEL) Item_Config_ActiveLevel(MROWS);
			#endif
						
			#if EITHER(PID_EDIT_MENU, PID_AUTOTUNE_MENU)
				else if(DwinMenu_configure.index == CONFIG_CASE_PIDTUNE) Item_Config_PIDTune(MROWS);			
			#endif			 
			}
			else 
				Move_Highlight(1, DwinMenu_configure.now + MROWS - DwinMenu_configure.index);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_configure.dec()) {
			if (DwinMenu_configure.now < DwinMenu_configure.index - MROWS) {
				DwinMenu_configure.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);

				if(DwinMenu_configure.index - MROWS == CONFIG_CASE_BACK) Draw_Back_First();
			#if ENABLED(FWRETRACT)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_RETRACT) Item_Config_Retract(0);
			#endif

			#if ENABLED(LIN_ADVANCE) 
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_LINADVANCE) Item_Config_LinearAdvance(0);
			#endif
			
			#if ENABLED(OPTION_REPEAT_PRINTING) 
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_REPRINT) Item_Config_Reprint(0);
			#endif
			
			#if ENABLED(FILAMENT_RUNOUT_SENSOR)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_FILAMENTRUNOUT) Item_Config_FilamentRunOut(0);
			#endif
			
			#if ENABLED(OPTION_ABORT_UNLOADFILAMENT)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_FILAMENTAUTOUNLOAD) Item_Config_FilamentAutoUnload(0);
			#endif
			
			#if ENABLED(POWER_LOSS_RECOVERY)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_POWERLOSS) Item_Config_Powerloss(0);
			#endif
			
			#if ENABLED(OPTION_AUTOPOWEROFF)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_SHUTDOWN) Item_Config_Shutdown(0);
			#endif
			
			#if ENABLED(CASE_LIGHT_MENU)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_BACKLIGHT) Item_Config_Backlight(0);
			#endif
			
			#if ENABLED(OPTION_WIFI_MODULE)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_WIFI) Item_Config_Wifi(0);
			 #if ENABLED(OPTION_WIFI_BAUDRATE)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_WIFISPEED) Item_Config_WifiBaudrate(0);
			 #endif			
			#endif
			
			#if ENABLED(OPTION_LASER)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_LASER) Item_Config_Laser(0);
			#endif			
			#if ENABLED(SPINDLE_FEATURE)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_SPINDLE) Item_Config_Spindle(0);
			#endif
			
			#if BOTH(MIXING_EXTRUDER, OPTION_MIXING_SWITCH)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_MIXERENABLE) Item_Config_MixerEnable(0);
				#if ENABLED(SWITCH_EXTRUDER_MENU)
				else if(DwinMenu_configure.index - MROWS  == CONFIG_CASE_SWITCHEXTRUDER) 	Item_Config_SwitchExtruder(0);
				#endif
			#endif
			
			#if HAS_LEVELING					
				#if HAS_PROBE_XY_OFFSET
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_PROBEOFFSET) Item_Config_ProbeOffset(0);
				#endif
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_LEVELING) Item_Config_Leveling(0);				
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_ACTIVELEVEL) Item_Config_ActiveLevel(0);			
			#endif			
						
			#if EITHER(PID_EDIT_MENU, PID_AUTOTUNE_MENU)
				else if(DwinMenu_configure.index - MROWS == CONFIG_CASE_PIDTUNE) Item_Config_PIDTune(0);			
			#endif
			}
			else {
				Move_Highlight(-1, DwinMenu_configure.now + MROWS - DwinMenu_configure.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {		
	  switch (DwinMenu_configure.now) {
      case CONFIG_CASE_BACK: 					// Back
				if(IS_SD_PRINTING() || IS_SD_PAUSED() || IS_SD_FILE_OPEN()){
					HMI_flag.show_mode = SHOWED_TUNE;
					Draw_Tune_Menu(TUNE_CASE_CONFIG);
				}
				else{
					Draw_Control_Menu(CONTROL_CASE_CONFIG + mixing_menu_adjust());
				}
		  break;
	#if ENABLED(FWRETRACT) 
			case CONFIG_CASE_RETRACT: 				// RETRACT			
				DwinMenuID = DWMENU_SET_RETRACT;
				DwinMenu_fwretract.index = MROWS;
				Draw_Retract_Menu();
			break;
  #endif

	#if ENABLED(LIN_ADVANCE) 
		#if (EXTRUDERS > 1)
			case CONFIG_CASE_LINADVANCE: 				// Linear Advance
				DwinMenuID = DWMENU_SET_LINADVANCE;
				DwinMenu_LinAdvance.index = MROWS;
				Draw_LinearAdvance_Menu();
			break;
		#else
			case CONFIG_CASE_LINADVANCE: 				// Linear Advance	
				DwinMenuID = DWMENU_SET_LINADVANCE;
				HMI_Value.extruder_advance_K[0] = (int16_t)(planner.extruder_advance_K[0]*MAXUNITMULT);
				DWIN_Draw_Selected_Small_Float22(MENUVALUE_X-16, MBASE(CONFIG_CASE_LINADVANCE + MROWS - DwinMenu_configure.index), HMI_Value.extruder_advance_K[0]);
			break;
		#endif
	#endif
			 
  #if ENABLED(OPTION_REPEAT_PRINTING)
			case CONFIG_CASE_REPRINT:				
				if(_break_on_printing()) break;
				DwinMenuID = DWMENU_SET_REPRINT;
				DwinMenu_reprint.index = MROWS;
				Draw_RepeatPrint_Menu();
			break;
  #endif
	 
  #if ENABLED(FILAMENT_RUNOUT_SENSOR)
			case CONFIG_CASE_FILAMENTRUNOUT:  // FILAMENT Run out
				DwinMenuID = DWMENU_CONFIG;
				runout.enabled = !runout.enabled;
			#if HAS_ONOFF_ICON					
				Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_FILAMENTRUNOUT, runout.enabled);
			#else
				DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_FILAMENTRUNOUT + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(runout.enabled));		
			#endif
				if(runout.enabled)
					queue.inject_P(PSTR("M412 S1"));		
				else
					queue.inject_P(PSTR("M412 S0"));
				HMI_AudioFeedback(settings.save());		
		   break;
  #endif

	#if ENABLED(OPTION_ABORT_UNLOADFILAMENT)
			case CONFIG_CASE_FILAMENTAUTOUNLOAD: // auto unload filament when printing about				
				if(mixer.mixing_enabled){
					HMI_flag.AutoUnload_enabled = false;
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Only work on Non-mix Hotend!"));
					DWIN_FEEDBACK_WARNNING();
					break;
				}
				DwinMenuID = DWMENU_CONFIG;
				HMI_flag.AutoUnload_enabled = !HMI_flag.AutoUnload_enabled;
			#if HAS_ONOFF_ICON
				Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_FILAMENTAUTOUNLOAD, HMI_flag.AutoUnload_enabled);
			#else
				DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_FILAMENTAUTOUNLOAD + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(HMI_flag.AutoUnload_enabled));
			#endif
				HMI_AudioFeedback(settings.save());		
		   break;
  #endif
	 
  #if ENABLED(POWER_LOSS_RECOVERY)
			case CONFIG_CASE_POWERLOSS:  			// POWERLOSS	 		
				if(_break_on_printing()) break;
				DwinMenuID = DWMENU_CONFIG;
				recovery.enabled = !recovery.enabled;
				recovery.changed();
			#if HAS_ONOFF_ICON
				Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_POWERLOSS, recovery.enabled);
			#else
				DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_POWERLOSS + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(recovery.enabled));
			#endif
				if(recovery.enabled)
					queue.inject_P(PSTR("M413 S1"));			
				else
					queue.inject_P(PSTR("M413 S0"));
				HMI_AudioFeedback(settings.save());				
			break;
  #endif
		
  #if ENABLED(OPTION_AUTOPOWEROFF)
			case CONFIG_CASE_SHUTDOWN:			// Auto Shutdown
				DwinMenuID = DWMENU_CONFIG;
				HMI_flag.Autoshutdown_enabled = !HMI_flag.Autoshutdown_enabled;
			#if HAS_ONOFF_ICON
				Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_SHUTDOWN, HMI_flag.Autoshutdown_enabled);
			#else
				DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_SHUTDOWN + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(HMI_flag.Autoshutdown_enabled));
			#endif
				HMI_AudioFeedback(settings.save());		
			break;
  #endif

  #if ENABLED(CASE_LIGHT_MENU)
 			case CONFIG_CASE_BACKLIGHT:
			#if ENABLED(CASE_LIGHT_NO_BRIGHTNESS)
				DwinMenuID = DWMENU_CONFIG;
				caselight.on = !caselight.on;				
			#if HAS_ONOFF_ICON
				Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_BACKLIGHT, caselight.on);
			#else
				DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_BACKLIGHT + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(caselight.on));
			#endif
				caselight.update(false);
			#else
				DwinMenuID = DWMENU_SET_CASELIGHTBRIGHTNESS;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X + 8, MBASE(CONFIG_CASE_BACKLIGHT + MROWS - DwinMenu_configure.index), caselight.brightness);
			#endif			
			break;
  #endif  
	
  #if ENABLED(OPTION_WIFI_MODULE)
			case CONFIG_CASE_WIFI:	 	 	
				WiFi_Enabled = !WiFi_Enabled;				
			#if HAS_ONOFF_ICON
				Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_WIFI, WiFi_Enabled);
			#else
				DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_WIFI + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(WiFi_Enabled));				
			#endif
			  if(WiFi_Enabled){
					queue.wifi_Handshake_ok = false;
					HMI_flag.wifi_link_timer = WIFI_LINK_CHECK_TIME;
					WIFI_onoff();
					DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Turn on WiFi and connecting...."));
			  }
				else{
					DwinMenuID = DWMENU_CONFIG;
					WIFI_onoff();
				}
			#if ENABLED(OPTION_WIFI_BAUDRATE)
				if(WITHIN(MROWS - DwinMenu_configure.index + CONFIG_CASE_WIFISPEED, 0, MROWS)) Item_Config_WifiBaudrate(MROWS - DwinMenu_configure.index + CONFIG_CASE_WIFISPEED);
			#endif
				HMI_AudioFeedback(settings.save());				
			break;
			
	 #if ENABLED(OPTION_WIFI_BAUDRATE)
      case CONFIG_CASE_WIFISPEED:
				if(_break_on_printing() || !WiFi_Enabled) break;
				DwinMenuID = DWMENU_SET_WIFIBAUDRATE;
				DWIN_Draw_Select_IntValue_Default(6, MENUVALUE_X-16, MBASE(CONFIG_CASE_WIFISPEED + MROWS -DwinMenu_configure.index), Table_Baudrate[WiFi_BaudRate]);
				HMI_AudioFeedback(settings.save());	
			break;
	 #endif		
  #endif

	#if ENABLED(OPTION_LASER)
			case CONFIG_CASE_LASER:	 	 
				if(_break_on_printing()) break;
				Laser_Enabled = !Laser_Enabled;				
			#if HAS_ONOFF_ICON				
				Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_LASER, Laser_Enabled);
			#else
				DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_LASER + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(Laser_Enabled));
			#endif
				HMI_AudioFeedback(settings.save());				
			break;
	
	#endif
	
	#if ENABLED(SPINDLE_FEATURE)
			case CONFIG_CASE_SPINDLE:	 	 	
				if(_break_on_printing()) break;
				if(cutter.Spindle_Enabled) 
					cutter.disable_spindle_feature(); 
				else 
					cutter.Spindle_Enabled = true;
		#if HAS_ONOFF_ICON				
				Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_SPINDLE, cutter.Spindle_Enabled);
		#else
				DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_SPINDLE + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(cutter.Spindle_Enabled));
		#endif
				HMI_AudioFeedback(settings.save());				
			break;
	
	#endif
 
	#if BOTH(MIXING_EXTRUDER, OPTION_MIXING_SWITCH)
			case CONFIG_CASE_MIXERENABLE:	
				if(_break_on_printing()) break;
					
				mixer.mixing_enabled = !mixer.mixing_enabled;
				if(mixer.mixing_enabled) {
				#if ENABLED(OPTION_ABORT_UNLOADFILAMENT)
					HMI_flag.AutoUnload_enabled = false;
				#endif
					DWIN_Draw_MaskString_Default(LBRX-strlen("    Mixing")*8, MBASE(CONFIG_CASE_MIXERENABLE + MROWS - DwinMenu_configure.index), PSTR("    Mixing")); 
					DWIN_FEEDBACK_WARNNING();
					DWIN_Show_Status_Message(COLOR_RED, PSTR("CAUTION! Require Mixing HOTEND."));
				}
				else {
					DWIN_Draw_MaskString_Default(LBRX-strlen("Non-Mixing")*8, MBASE(CONFIG_CASE_MIXERENABLE + MROWS - DwinMenu_configure.index), PSTR("Non-Mixing"));
					DWIN_FEEDBACK_WARNNING();
					DWIN_Show_Status_Message(COLOR_RED, PSTR("CAUTION! Require Non-mixing HOTEND"));
				}
				if(WITHIN(MROWS - DwinMenu_configure.index + CONFIG_CASE_SWITCHEXTRUDER, 0, MROWS)) Item_Config_SwitchExtruder(MROWS - DwinMenu_configure.index + CONFIG_CASE_SWITCHEXTRUDER);
			#if ENABLED(OPTION_ABORT_UNLOADFILAMENT)
				if(WITHIN(MROWS - DwinMenu_configure.index + CONFIG_CASE_FILAMENTAUTOUNLOAD, 0, MROWS)) Item_Config_FilamentAutoUnload(MROWS - DwinMenu_configure.index + CONFIG_CASE_FILAMENTAUTOUNLOAD);
			#endif
				HMI_AudioFeedback(settings.save());
			break;
			
		#if ENABLED(SWITCH_EXTRUDER_MENU)
		case CONFIG_CASE_SWITCHEXTRUDER:	
				if(_break_on_printing() && mixer.mixing_enabled) break;
					
				if(mixer.mixing_enabled){
					DWIN_FEEDBACK_WARNNING();
					DWIN_Show_Status_Message(COLOR_RED, PSTR("Can't change in Mixing HOTEND !"));
					break;
				}					
				DwinMenuID = DWMENU_SET_SWITCHEXTRUDER;
				DWIN_Draw_MaskString_Default_Color(COLOR_BLUE, 216-12*MIXING_STEPPERS, MBASE(CONFIG_CASE_SWITCHEXTRUDER + MROWS - DwinMenu_configure.index), _getString_SwitchExtruder()); 				
			break;
		#endif
	#endif
	
	#if HAS_OFFSET_MENU
    case CONFIG_CASE_HOMEOFFSET:
			if(_break_on_printing()) break;
		
			DwinMenuID = DWMENU_SET_HOMEOFFSET;
			DwinMenu_Homeoffset.index = MROWS;
			Draw_HomeOffset_Menu();
		break;
	#endif
 
	#if HAS_LEVELING
	  case CONFIG_CASE_LEVELING:
			if(_break_on_printing()) break;

			DwinMenuID = DWMENU_CONFIG;
			HMI_flag.Leveling_Menu_Fg = !HMI_flag.Leveling_Menu_Fg;			
		#if HAS_ONOFF_ICON
			Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_LEVELING, HMI_flag.Leveling_Menu_Fg);
		#else
			DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_LEVELING + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(HMI_flag.Leveling_Menu_Fg));
		#endif
			
			if(HMI_flag.Leveling_Menu_Fg)	set_bed_leveling_enabled(false);
			HMI_AudioFeedback(settings.save());
		break;

		case CONFIG_CASE_ACTIVELEVEL:
			if(_break_on_printing()) break;
			
			DwinMenuID = DWMENU_CONFIG;
	#if ENABLED(OPTION_NEWAUTOLEVELING)
		HMI_flag.AutoLeveling_Fg = !HMI_flag.AutoLeveling_Fg;
		#if HAS_ONOFF_ICON
			Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_ACTIVELEVEL, HMI_flag.AutoLeveling_Fg);
		#else
			DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_ACTIVELEVEL + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(HMI_flag.AutoLeveling_Fg));
		#endif
			HMI_AudioFeedback(settings.save());
	#else
		planner.leveling_active = !planner.leveling_active;	
		#if HAS_ONOFF_ICON
			Draw_ONOFF_Icon(MROWS - DwinMenu_configure.index + CONFIG_CASE_ACTIVELEVEL, planner.leveling_active);
		#else
			DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(CONFIG_CASE_ACTIVELEVEL + MROWS - DwinMenu_configure.index), F_STRING_ONOFF(planner.leveling_active));
		#endif
			set_bed_leveling_enabled(planner.leveling_active);
	#endif
		break;
		
		#if HAS_PROBE_XY_OFFSET
		case CONFIG_CASE_PROBEOFFSET:
			if(_break_on_printing()) break;
		
			DwinMenuID = DWMENU_SET_PROBEOFFSET;
			DwinMenu_Probeoffset.index = MROWS;
			Draw_ProbeOffset_Menu();
		break;
		#endif
	#endif//HAS_LEVELING

 	#if EITHER(PID_EDIT_MENU, PID_AUTOTUNE_MENU)
      case CONFIG_CASE_PIDTUNE:
				if(_break_on_printing()) break;
					
			#if BOTH(PID_AUTOTUNE_MENU, PID_EDIT_MENU)			
				DwinMenu_PIDTune.index = MROWS;
				Draw_PIDTune_Menu();				
			#elif ENABLED(PID_AUTOTUNE_MENU)			
				DwinMenuID = DWMENU_PID_AUTOTUNE;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(CONFIG_CASE_PIDTUNE + MROWS -DwinMenu_configure.index), HMI_Value.PIDAutotune_Temp);				
 			#endif//CONFIG_CASE_PIDTUNE
			
			break;
	#endif
   
			default: break;
		}
	}
	dwinLCD.UpdateLCD();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Control >> PreHeat PLA
//
#if HAS_PREHEAT
inline void Draw_SetPreHeatPLA_Menu(){
	DwinMenuID = DWMENU_PREHEAT_PLA;
	DwinMenu_PreheatPLA.reset();
	//DwinMenu_PreheatPLA.index = _MAX(DwinMenu_PreheatPLA.now, MROWS);
	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_PLA, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);

	Draw_Back_First();

#if HAS_HOTEND
	DWIN_Show_MultiLanguage_String(MTSTRING_NOZZLE, LBLX, MBASE(PREHEAT_CASE_TEMP));
	DWIN_Show_MultiLanguage_String(MTSTRING_TEMP, LBLX+get_MultiLanguageString_Width(MTSTRING_NOZZLE)+6, MBASE(PREHEAT_CASE_TEMP));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_TEMP), ui.material_preset[0].hotend_temp);
	Draw_Menu_Line(PREHEAT_CASE_TEMP, ICON_SETENDTEMP);
#endif	
	
#if HAS_HEATED_BED
	DWIN_Show_MultiLanguage_String(MTSTRING_BED, LBLX, MBASE(PREHEAT_CASE_BED));
	DWIN_Show_MultiLanguage_String(MTSTRING_TEMP, LBLX+get_MultiLanguageString_Width(MTSTRING_BED)+6, MBASE(PREHEAT_CASE_BED));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_BED), ui.material_preset[0].bed_temp);
	Draw_Menu_Line(PREHEAT_CASE_BED, ICON_SETBEDTEMP);
#endif

#if HAS_FAN
	DWIN_Show_MultiLanguage_String(MTSTRING_FANSPEED, LBLX, MBASE(PREHEAT_CASE_FAN));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_FAN), ui.material_preset[0].fan_speed);
	Draw_Menu_Line(PREHEAT_CASE_FAN, ICON_FANSPEED);
#endif

#if ENABLED(EEPROM_SETTINGS)
	DWIN_Show_MultiLanguage_String(MTSTRING_STORE, LBLX, MBASE(PREHEAT_CASE_SAVE));
	DWIN_Show_MultiLanguage_String(MTSTRING_SETTINGS, LBLX+get_MultiLanguageString_Width(MTSTRING_STORE)+6, MBASE(PREHEAT_CASE_SAVE));
	Draw_Menu_Line(PREHEAT_CASE_SAVE, ICON_WRITEEEPROM);
#endif
} 

///////////////////////////////////////////////////////////////////////////
//
// Control >> PreHeat ABS
//
inline void Draw_SetPreHeatABS_Menu(){
	DwinMenuID = DWMENU_PREHEAT_ABS;
	DwinMenu_PreheatABS.reset();
	//DwinMenu_PreheatABS.index = _MAX(DwinMenu_PreheatABS.now, MROWS);
	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_ABS, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);

	Draw_Back_First();
	
	DWIN_Show_MultiLanguage_String(MTSTRING_NOZZLE, LBLX, MBASE(PREHEAT_CASE_TEMP));
	DWIN_Show_MultiLanguage_String(MTSTRING_TEMP, LBLX+get_MultiLanguageString_Width(MTSTRING_NOZZLE)+6, MBASE(PREHEAT_CASE_TEMP));	
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_TEMP), ui.material_preset[1].hotend_temp);
	Draw_Menu_Line(PREHEAT_CASE_TEMP, ICON_SETENDTEMP);
	
#if HAS_HEATED_BED
	DWIN_Show_MultiLanguage_String(MTSTRING_BED, LBLX, MBASE(PREHEAT_CASE_BED));
	DWIN_Show_MultiLanguage_String(MTSTRING_TEMP, LBLX+get_MultiLanguageString_Width(MTSTRING_BED)+6, MBASE(PREHEAT_CASE_BED));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_BED), ui.material_preset[1].bed_temp);	
	Draw_Menu_Line(PREHEAT_CASE_BED, ICON_SETBEDTEMP);
#endif

#if HAS_FAN
	DWIN_Show_MultiLanguage_String(MTSTRING_FANSPEED, LBLX, MBASE(PREHEAT_CASE_FAN));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_FAN), ui.material_preset[1].fan_speed);
	Draw_Menu_Line(PREHEAT_CASE_FAN, ICON_FANSPEED);
#endif

#if ENABLED(EEPROM_SETTINGS)
	DWIN_Show_MultiLanguage_String(MTSTRING_STORE, LBLX, MBASE(PREHEAT_CASE_SAVE));
	DWIN_Show_MultiLanguage_String(MTSTRING_SETTINGS, LBLX+get_MultiLanguageString_Width(MTSTRING_STORE)+6, MBASE(PREHEAT_CASE_SAVE));
	Draw_Menu_Line(PREHEAT_CASE_SAVE, ICON_WRITEEEPROM);
#endif
}

 /* PLA Preheat */
void HMI_PLAPreheatSetting() {
  ENCODER_DiffState encoder_diffState = get_encoder_state();
  if (encoder_diffState == ENCODER_DIFF_NO) return;

  // Avoid flicker by updating only the previous menu
  if (encoder_diffState == ENCODER_DIFF_CW) {
   if (DwinMenu_PreheatPLA.inc(PREHEAT_CASE_END)) Move_Highlight(1, DwinMenu_PreheatPLA.now);
  }
  else if (encoder_diffState == ENCODER_DIFF_CCW) {
   if (DwinMenu_PreheatPLA.dec()) Move_Highlight(-1, DwinMenu_PreheatPLA.now);
  }
  else if (encoder_diffState == ENCODER_DIFF_ENTER) {
   switch (DwinMenu_PreheatPLA.now) {
    case PREHEAT_CASE_BACK: // Back
			Draw_Control_Menu(CONTROL_CASE_SETPLA + mixing_menu_adjust());
     break;
    #if HAS_HOTEND
     case PREHEAT_CASE_TEMP: // Nozzle temperature
      DwinMenuID = DWMENU_SET_ETMP;
      HMI_Value.E_Temp = ui.material_preset[0].hotend_temp;			
			if(HMI_Value.E_Temp > HOTEND_WARNNING_TEMP)
				DWIN_Draw_Warn_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_TEMP + MROWS - DwinMenu_PreheatPLA.index), HMI_Value.E_Temp);
			else
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_TEMP + MROWS - DwinMenu_PreheatPLA.index), HMI_Value.E_Temp);
      EncoderRate.enabled = true;
      break;
    #endif
    #if HAS_HEATED_BED
     case PREHEAT_CASE_BED: // Bed temperature
      DwinMenuID = DWMENU_SET_BTMP;
      HMI_Value.Bed_Temp = ui.material_preset[0].bed_temp;
      DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_BED + MROWS - DwinMenu_PreheatPLA.index), HMI_Value.Bed_Temp);
      EncoderRate.enabled = true;
      break;
    #endif
    #if HAS_FAN
     case PREHEAT_CASE_FAN: // Fan speed
      DwinMenuID = DWMENU_SET_FANSPEED;
      HMI_Value.Fan_speed = ui.material_preset[0].fan_speed;
      DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_FAN + MROWS - DwinMenu_PreheatPLA.index), HMI_Value.Fan_speed);
      EncoderRate.enabled = true;
      break;
    #endif
    #if ENABLED(EEPROM_SETTINGS)
     case PREHEAT_CASE_SAVE:  // Save PLA configuration
      HMI_AudioFeedback(settings.save());
      break;
    #endif
    default: break;
   }
  }
  dwinLCD.UpdateLCD();
}

/* ABS Preheat */
void HMI_ABSPreheatSetting() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_PreheatABS.inc(PREHEAT_CASE_END)) Move_Highlight(1, DwinMenu_PreheatABS.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_PreheatABS.dec()) Move_Highlight(-1, DwinMenu_PreheatABS.now);
	}
  else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_PreheatABS.now) {
			case PREHEAT_CASE_BACK: // Back
				Draw_Control_Menu(CONTROL_CASE_SETABS + mixing_menu_adjust());
			break;
		#if HAS_HOTEND
			case PREHEAT_CASE_TEMP: // Set nozzle temperature
				DwinMenuID = DWMENU_SET_ETMP;
				HMI_Value.E_Temp = ui.material_preset[1].hotend_temp;			
				if(HMI_Value.E_Temp > HOTEND_WARNNING_TEMP)
					DWIN_Draw_Warn_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_TEMP + MROWS - DwinMenu_PreheatABS.index), HMI_Value.E_Temp);
				else
					DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_TEMP + MROWS - DwinMenu_PreheatABS.index), HMI_Value.E_Temp);
				EncoderRate.enabled = true;
			break;
		#endif

		#if HAS_HEATED_BED
			case PREHEAT_CASE_BED: // Set bed temperature
				DwinMenuID = DWMENU_SET_BTMP;
				HMI_Value.Bed_Temp = ui.material_preset[1].bed_temp;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_BED + MROWS - DwinMenu_PreheatABS.index), HMI_Value.Bed_Temp);
				EncoderRate.enabled = true;
			break;
		#endif

		#if HAS_FAN
			case PREHEAT_CASE_FAN: // Set fan speed
				DwinMenuID = DWMENU_SET_FANSPEED;
				HMI_Value.Fan_speed = ui.material_preset[1].fan_speed;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(PREHEAT_CASE_FAN + MROWS - DwinMenu_PreheatABS.index), HMI_Value.Fan_speed);
				EncoderRate.enabled = true;
			break;
		#endif

		#if ENABLED(EEPROM_SETTINGS)
			case PREHEAT_CASE_SAVE:  // Save ABS configuration
				HMI_AudioFeedback(settings.save());
			break;
		#endif
			default: break;
		}
	}
	dwinLCD.UpdateLCD();
}
#endif



/* Control */
void HMI_Control() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	// Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_control.inc(CONTROL_CASE_END + mixing_menu_adjust())) {
			if (DwinMenu_control.now > MROWS && DwinMenu_control.now > DwinMenu_control.index) {
				DwinMenu_control.index = DwinMenu_control.now;
				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);		
				if (DwinMenu_control.index == CONTROL_CASE_SETPLA + mixing_menu_adjust()) Item_Control_PLA(MROWS);
				else if (DwinMenu_control.index == CONTROL_CASE_SETABS + mixing_menu_adjust()) Item_Control_ABS(MROWS);
			#if ENABLED(BLTOUCH)
				else if(DwinMenu_control.index == CONTROL_CASE_BLTOUCH + mixing_menu_adjust()) Item_Control_BLtouch(MROWS);
			#endif
			#if ENABLED(EEPROM_SETTINGS)
				else if(DwinMenu_control.index == CONTROL_CASE_SAVE + mixing_menu_adjust()) Item_Control_Save(MROWS);
				else if(DwinMenu_control.index == CONTROL_CASE_LOAD + mixing_menu_adjust())	Item_Control_Load(MROWS);
				else if(DwinMenu_control.index == CONTROL_CASE_RESET + mixing_menu_adjust())	Item_Control_Reset(MROWS);
			#endif		
			}
			else {
				Move_Highlight(1, DwinMenu_control.now + MROWS - DwinMenu_control.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_control.dec()) {			
			if (DwinMenu_control.now < DwinMenu_control.index - MROWS) {
				DwinMenu_control.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if (DwinMenu_control.index - MROWS == CONTROL_CASE_BACK) Draw_Back_First();    		
			#if ENABLED(MIXING_EXTRUDER)
				else if (DwinMenu_control.index - MROWS == CONTROL_CASE_MIXER + mixing_menu_adjust()) Item_Control_Mixer(0);
			#endif
				else if (DwinMenu_control.index - MROWS == CONTROL_CASE_CONFIG + mixing_menu_adjust()) Item_Control_Config(0);
				else if (DwinMenu_control.index - MROWS == CONTROL_CASE_MOTION + mixing_menu_adjust()) Item_Control_Motion(0);
				else if (DwinMenu_control.index - MROWS == CONTROL_CASE_SETPLA + mixing_menu_adjust()) Item_Control_PLA(0);
				else if (DwinMenu_control.index - MROWS == CONTROL_CASE_SETABS + mixing_menu_adjust()) Item_Control_ABS(0);		
			}
			else {
				Move_Highlight(-1, DwinMenu_control.now + MROWS - DwinMenu_control.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		uint8_t current_menu = DwinMenu_control.now;
		#if BOTH(MIXING_EXTRUDER, OPTION_MIXING_SWITCH)
		if(!mixer.mixing_enabled && DwinMenu_control.now > 0)
			current_menu = DwinMenu_control.now + 1;		
		#endif
		switch (current_menu) {
			case CONTROL_CASE_BACK: // Back				
				Draw_Main_Menu(false, MAIN_CASE_CONTROL);
			break;
			
		#if ENABLED(MIXING_EXTRUDER)
			case CONTROL_CASE_MIXER: // Mixer
				Draw_Mixer_Menu();
			break;
		#endif
			
			case CONTROL_CASE_CONFIG: // Config
				Draw_Config_Menu();
			break;
			
			case CONTROL_CASE_MOTION: // Motion
				Draw_Motion_Menu();
			break;

			case CONTROL_CASE_SETPLA:	// PLA preheat setting				
				DwinMenu_PreheatPLA.reset();
				HMI_flag.show_mode = SHOWED_PEHEAT_PLA;
				Draw_SetPreHeatPLA_Menu();
			break;

			case CONTROL_CASE_SETABS: // ABS preheat setting				
				DwinMenu_PreheatABS.reset();
				HMI_flag.show_mode = SHOWED_PEHEAT_ABS;
				Draw_SetPreHeatABS_Menu();
			break;	

		#if ENABLED(BLTOUCH)
			case CONTROL_CASE_BLTOUCH: // Bltouch				
				DwinMenu_bltouch.reset();
				SERIAL_ECHOLNPAIR("DwinMenu_control.now = ", DwinMenu_control.now);
				SERIAL_ECHOLNPAIR("DwinMenu_control.last = ", DwinMenu_control.last);
				SERIAL_ECHOLNPAIR("DwinMenu_control.index = ", DwinMenu_control.index);
				Draw_Bltouch_Menu();
			break;
		#endif

		#if ENABLED(EEPROM_SETTINGS)
			case CONTROL_CASE_SAVE: // Write EEPROM    
				HMI_AudioFeedback(settings.save());				
			break;
			
			case CONTROL_CASE_LOAD: // Read EEPROM
				SERIAL_ECHOLNPAIR("\n eeprom size = ", settings.datasize());
				HMI_AudioFeedback(settings.load());
			break;
			
			case CONTROL_CASE_RESET: // Reset EEPROM				
				if(card.isMounted()){
					card.closefile();
					card.removeFile("/old_fw.bin");
					// Purge the recovery file
  				TERN_(POWER_LOSS_RECOVERY, recovery.purge());
				}
				settings.reset();
				HMI_AudioFeedback(settings.save());
				Draw_Control_Menu();
			break;			
		#endif
				
			default: break;
		}
	}
	dwinLCD.UpdateLCD();
}
#endif

