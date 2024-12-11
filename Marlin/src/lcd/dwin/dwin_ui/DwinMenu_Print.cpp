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
 * DwinMenu_Print.cpp
 */

 #include "../../../inc/MarlinConfig.h"

#if HAS_DWIN_LCD
#include "dwin.h"


#if ENABLED(PAUSE_HEAT)
 #if HAS_HOTEND
  static uint16_t temphot = 0;
 #endif
 #if HAS_HEATED_BED
  static uint16_t tempbed = 0;
 #endif
#endif

#if ENABLED(BABYSTEPPING)
static millis_t Babystep_firstclicktime;
baby_step_t babyzoffset;
#endif 

#if ENABLED(OPTION_REPEAT_PRINTING)
#include "../../../feature/repeat_printing.h"
#endif

static void Draw_Select_Highlight(const bool sel) {
	HMI_flag.select_flag = sel;
	const uint16_t c1 = sel ? COLOR_RED : COLOR_BG_WINDOW,
	     c2 = sel ? COLOR_BG_WINDOW : COLOR_RED;
	dwinLCD.Draw_Rectangle(0, c1, 25, 279, 126, 318);
	dwinLCD.Draw_Rectangle(0, c1, 24, 278, 127, 319);
	dwinLCD.Draw_Rectangle(0, c2, 145, 279, 246, 318);
	dwinLCD.Draw_Rectangle(0, c2, 144, 278, 247, 319);
	DWIN_FEEDBACK_TICK();
}

static uint16_t nr_sd_menu_items() {
	return card.get_num_Files() + !card.flag.workDirIsRoot;
}

static bool printer_busy() {
  return planner.movesplanned() || printingIsActive();
}


/**
 * Read and cache the working directory.
 *
 * TODO: New code can follow the pattern of menu_media.cpp
 * and rely on Marlin caching for performance. No need to
 * cache files here.
 */

#ifndef strcasecmp_P
 #define strcasecmp_P(a, b) strcasecmp((a), (b))
#endif

static void make_name_without_ext(char *dst, char *src, size_t maxlen=MENU_CHAR_LIMIT) {
 char * const name = card.longest_filename();
 size_t pos    = strlen(name); // index of ending nul

 // For files, remove the extension
 // which may be .gcode, .gco, or .g
 if (!card.flag.filenameIsDir)
  while (pos && src[pos] != '.') pos--; // find last '.' (stop at 0)

 size_t len = pos;  // nul or '.'
 if (len > maxlen) { // Keep the name short
  pos    = len = maxlen; // move nul down
  dst[--pos] = '.'; // insert dots
  dst[--pos] = '.';
  dst[--pos] = '.';
 }

 dst[len] = '\0';  // end it

 // Copy down to 0
 while (pos--) dst[pos] = src[pos];
}

#if ENABLED(SCROLL_LONG_FILENAMES)
char shift_name[LONG_FILENAME_LENGTH + 1];
int8_t shift_amt; // = 0
millis_t shift_ms; // = 0
// Init the shift name based on the highlighted item
static void Init_Shift_Name() {
	const bool is_subdir = !card.flag.workDirIsRoot;
	const uint16_t filenum = DwinMenu_file.now - 1 - is_subdir; // Skip "Back" and ".."
	const uint16_t fileCnt = card.get_num_Files();
	if(WITHIN(filenum, 0, fileCnt - 1)){
		card.getfilename_sorted(SD_ORDER(filenum, fileCnt));
		char * const name = card.longest_filename();
		make_name_without_ext(shift_name, name, 100);
	}
}

inline void Init_SDItem_Shift() {
	shift_amt = 0;
	shift_ms = (DwinMenu_file.now > 0 && strlen(shift_name) > MENU_CHAR_LIMIT) ? millis() + 750UL : 0;
}

inline void Draw_SDItem_Shifted(int8_t &shift) {
	// Limit to the number of chars past the cutoff
	const size_t len = strlen(shift_name);
	NOMORE(shift, _MAX(len - MENU_CHAR_LIMIT, 0U));

	// Shorten to the available space
	const size_t lastchar = _MIN((signed)len, shift + MENU_CHAR_LIMIT);

	const char c = shift_name[lastchar];
	shift_name[lastchar] = '\0';

	const uint8_t row = DwinMenu_file.now + MROWS - DwinMenu_file.index; // skip "Back" and scroll
	Erase_Menu_Text(row);
	Draw_Menu_Line(row, ICON_FILE, &shift_name[shift]);

	shift_name[lastchar] = c;
}
#endif

/**
 * Display an SD item, adding a CDUP for subfolders.
 */
static void Draw_SDItem(const uint16_t item, int16_t row=-1) {
	if (row < 0) row = item + 1 + MROWS - DwinMenu_file.index;
	const bool is_subdir = !card.flag.workDirIsRoot;
	if (is_subdir && item == 0) {
#if (DWINLCD_MENU_VERSION >= 2)
		Draw_Menu_Line(row, ICON_FOLDER_N, "..");
#else
		Draw_Menu_Line(row, ICON_FOLDER, "..");
#endif
		return;
	}

	card.getfilename_sorted(item - is_subdir);
	char * const name = card.longest_filename();

#if ENABLED(SCROLL_LONG_FILENAMES)
	// Init the current selected name
	// This is used during scroll drawing
	if (item == DwinMenu_file.now - 1) {
		make_name_without_ext(shift_name, name, 100);
		Init_SDItem_Shift();
	}
#endif

	// Draw the file/folder with name aligned left
	char str[strlen(name) + 1];
	make_name_without_ext(str, name);
#if (DWINLCD_MENU_VERSION >= 2)
	Draw_Menu_Line(row, card.flag.filenameIsDir ? ICON_FOLDER_N : ICON_FILE, str);
#else
	Draw_Menu_Line(row, card.flag.filenameIsDir ? ICON_FOLDER : ICON_FILE, str);
#endif
}


// Redraw the first set of SD Files
void Draw_FileList() {
	DwinMenu_file.reset();

	Clear_Dwin_Area(AREA_MENU); // Leave title bar unchanged
	Draw_Back_First();

	if (card.isMounted()) {
		//if (card.flag.mounted) {
		// As many files as will fit
		
		//LOOP_L_N(i, (nr_sd_menu_items()>MROWS? MROWS : nr_sd_menu_items()))
		LOOP_L_N(i, _MIN(nr_sd_menu_items(), MROWS))
			Draw_SDItem(i, i+1);
		TERN_(SCROLL_LONG_FILENAMES, Init_SDItem_Shift());
	}
	else {
		dwinLCD.Draw_Rectangle(1, COLOR_BG_RED, 10, MBASE(3) - 10, DWIN_WIDTH - 10, MBASE(4));
		dwinLCD.Draw_String(false, false, font16x32, COLOR_YELLOW, COLOR_BG_RED, ((DWIN_WIDTH) - 8 * 16) / 2, MBASE(3), PSTR("No Media"));
	}
}

inline void SDCard_Up() {
	card.cdup();
	Draw_FileList();
	HMI_flag.lcd_sd_status = true; // On next DWIN Update
}

inline void SDCard_Folder(char * const dirname) {
	card.cd(dirname);
	Draw_FileList();
	HMI_flag.lcd_sd_status = true; // On next DWIN Update
}

//
// Print>>File
//
void HMI_SDCardInit() {	
	if (card.flag.workDirIsRoot) {
		//#if !PIN_EXISTS(SD_DETECT)
		card.mount();
		//#endif
	}
	card.cdroot();
}


// Select (and Print) File
void HMI_SelectFile() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();

	const uint16_t hasUpDir = !card.flag.workDirIsRoot;

	if (encoder_diffState == ENCODER_DIFF_NO) {
		#if ENABLED(SCROLL_LONG_FILENAMES)
		if (shift_ms && DwinMenu_file.now >= 1 + hasUpDir) {
			// Scroll selected filename every second
			const millis_t ms = millis();
			if (ELAPSED(ms, shift_ms)) {
				const bool was_reset = shift_amt < 0;
				shift_ms = ms + 375UL + was_reset * 250UL; // ms per character
				int8_t shift_new = shift_amt + 1;      // Try to shift by...
				Draw_SDItem_Shifted(shift_new);       // Draw the item
				if (!was_reset && shift_new == 0)      // Was it limited to 0?
				shift_ms = 0;               // No scrolling needed
				else if (shift_new == shift_amt)      // Scroll reached the end
				shift_new = -1;              // Reset
				shift_amt = shift_new;           // Set new scroll
			}
		}
		#endif
		return;
	}

	// First pause is long. Easy.
	// On reset, long pause must be after 0.

	const uint16_t fullCnt = nr_sd_menu_items();

	if (encoder_diffState == ENCODER_DIFF_CW && fullCnt > 0) {
		if (DwinMenu_file.inc(1 + fullCnt)) {
			const uint8_t itemnum = DwinMenu_file.now - 1;       // -1 for "Back"
			if (TERN0(SCROLL_LONG_FILENAMES, shift_ms)) {       // If line was shifted
				Erase_Menu_Text(itemnum + MROWS - DwinMenu_file.index);     // Erase and
				Draw_SDItem(itemnum - 1);                // redraw
			}

			if (DwinMenu_file.now > MROWS && DwinMenu_file.now > DwinMenu_file.index) { // Cursor past the bottom
				DwinMenu_file.index = DwinMenu_file.now;              // New bottom line
				Scroll_Menu(DWIN_SCROLL_UP);
				Draw_SDItem(itemnum, MROWS);              // Draw and init the shift name
			}
			else {
				Move_Highlight(1, DwinMenu_file.now + MROWS - DwinMenu_file.index); // Just move highlight
				TERN_(SCROLL_LONG_FILENAMES, Init_Shift_Name());     // ...and init the shift name
			}
			TERN_(SCROLL_LONG_FILENAMES, Init_SDItem_Shift());
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW && fullCnt > 0) {
		if (DwinMenu_file.dec()) {
			const uint8_t itemnum = DwinMenu_file.now - 1;       // -1 for "Back"
			if (TERN0(SCROLL_LONG_FILENAMES, shift_ms)) {       // If line was shifted
				Erase_Menu_Text(DwinMenu_file.now + 1 + MROWS - DwinMenu_file.index); // Erase and
				Draw_SDItem(itemnum + 1);                // redraw
			}

			if (DwinMenu_file.now < DwinMenu_file.index - MROWS) {        // Cursor past the top
				DwinMenu_file.index--;                      // New bottom line
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if (DwinMenu_file.index == MROWS) {
					Draw_Back_First();
					TERN_(SCROLL_LONG_FILENAMES, shift_ms = 0);
				}
				else {
					Draw_SDItem(itemnum, 0);               // Draw the item (and init shift name)
				}
			}
			else {
				Move_Highlight(-1, DwinMenu_file.now + MROWS - DwinMenu_file.index); // Just move highlight
				TERN_(SCROLL_LONG_FILENAMES, Init_Shift_Name());    // ...and init the shift name
			}
			TERN_(SCROLL_LONG_FILENAMES, Init_SDItem_Shift());    // Reset left. Init timer.
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		if (DwinMenu_file.now == 0) { // Back
			Draw_Main_Menu();
		}
		else if (hasUpDir && DwinMenu_file.now == 1) { // CD-Up
			SDCard_Up();			
		}
		else {
			const uint16_t filenum = DwinMenu_file.now - 1 - hasUpDir;
			card.getfilename_sorted(SD_ORDER(filenum, card.get_num_Files()));

			// Enter that folder!
			if (card.flag.filenameIsDir) {
				SDCard_Folder(card.filename);
				dwinLCD.UpdateLCD();
				return;
			}

			// Reset highlight for next entry
			DwinMenu_print.reset();
			DwinMenu_file.reset();

		#if ENABLED(MIXING_EXTRUDER)
			// Switch current VTOOL to T0			
			HMI_Value.current_vtool= mixer.selected_vtool = 0;
			mixer.update_mix_from_vtool();
			MIXER_STEPPER_LOOP(i) {HMI_Value.mix_percent[i] = mixer.percentmix[i];}
		#endif
		
		#if ENABLED(OPTION_REPEAT_PRINTING) 
			ReprintManager.is_repeatPrinting = false;
			strcpy(rePrint_filename, card.filename);
		#endif

			// Start choice and print SD file
			HMI_flag.show_mode = SHOWED_TUNE;
			card.openAndPrintFile(card.filename);
			
			//
			TERN_(OPTION_ABORT_UNLOADFILAMENT, sum_extrude = 0);

			//DWIN_status = ID_SM_PRINTING;			
			set_all_unhomed();
			DWIN_start_SDPrint();
			return;
		}
	}
	dwinLCD.UpdateLCD();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Printing UI
//
static void Draw_ICON_Tune(const bool selected = false) {
	if(selected) {
		DWIN_Show_ICON(ICON_SETUP_SEL, 8, 252);
		dwinLCD.Draw_Rectangle(0, COLOR_WHITE, 8, 252, 87, 351);
		DWIN_Show_ICON(GET_PIC_ID(ICON_BIG_TUNE), GET_ICON_X(TUNE), 325);
	}
	else {
		DWIN_Show_ICON(ICON_SETUP, 8, 252);
		DWIN_Show_MultiLanguage_String(MTSTRING_TUNE, GET_ICON_X(TUNE), 325);
	}
	dwinLCD.UpdateLCD();
}

static void Draw_ICON_Pause(const bool selected = false) {
	if(selected) {
		DWIN_Show_ICON(ICON_PAUSE_SEL, 96, 252);
		dwinLCD.Draw_Rectangle(0, COLOR_WHITE, 96, 252, 175, 351);
		DWIN_Show_ICON(GET_PIC_ID(ICON_BIG_PAUSE), GET_ICON_X(PAUSE), 325);
	}
	else {
		DWIN_Show_ICON(ICON_PAUSE, 96, 252);
		DWIN_Show_MultiLanguage_String(MTSTRING_PAUSE, GET_ICON_X(PAUSE), 325);
	}
	dwinLCD.UpdateLCD();
}

static void Draw_ICON_Continue(const bool selected = false) {
	if(selected) {
		DWIN_Show_ICON(ICON_CONTINUE_SEL, 96, 252);
		dwinLCD.Draw_Rectangle(0, COLOR_WHITE, 96, 252, 175, 351);
		DWIN_Show_ICON(GET_PIC_ID(ICON_BIG_CONTINUE), GET_ICON_X(CONTINUE), 325);
	}
	else {
		DWIN_Show_ICON(ICON_CONTINUE, 96, 252);
		DWIN_Show_MultiLanguage_String(MTSTRING_CONTINUE, GET_ICON_X(CONTINUE), 325);
	}
	dwinLCD.UpdateLCD();
}

static void Draw_ICON_Stop(const bool selected = false) {
	if(selected) {
		DWIN_Show_ICON(ICON_STOP_SEL, 184, 252);
		dwinLCD.Draw_Rectangle(0, COLOR_WHITE, 184, 252, 263, 351);
		DWIN_Show_ICON(GET_PIC_ID(ICON_BIG_STOP), GET_ICON_X(STOP), 325);
	}
	else {
		DWIN_Show_ICON(ICON_STOP, 184, 252);
		DWIN_Show_MultiLanguage_String(MTSTRING_STOP, GET_ICON_X(STOP), 325);
	}
	dwinLCD.UpdateLCD();
}

static void Draw_ICON_Printingstatus(){
	Draw_ICON_Tune(DwinMenu_print.now == PRINT_CASE_TUNE);
	if(printingIsPaused()) 
		Draw_ICON_Continue(DwinMenu_print.now == PRINT_CASE_PAUSE); 
	else 
		Draw_ICON_Pause(DwinMenu_print.now == PRINT_CASE_PAUSE);
	Draw_ICON_Stop(DwinMenu_print.now == PRINT_CASE_STOP);			
}
//////////////////////////////////////////////////////
//
// Printing progressbar
//
void Draw_Print_ProgressBar() {
	DWIN_Show_ICON(ICON_BAR, 15, 63);
	dwinLCD.Draw_Rectangle(1, BARFILL_COLOR, 15 + HMI_Value.Percentrecord * 240 / 100, 63, 256, 83);
	dwinLCD.Draw_IntValue(false, true, 0, font8x16, COLOR_BG_RED, BARFILL_COLOR, 2, 117, 65, HMI_Value.Percentrecord);
	dwinLCD.Draw_String(false, false, font8x16, COLOR_BG_RED, BARFILL_COLOR, 133, 65, PSTR("%"));
}

void Draw_Print_ElapsedTime() {
	#define	ELAPSEDTIME_STAR_X	40
	duration_t ElapsedTime = HMI_Value.elapsed_value; // print timer	
	char timer_string[20];
	if(ElapsedTime.hour() < 24)
		sprintf_P(timer_string, PSTR("%02d:%02d:%02d"), (uint8_t)(ElapsedTime.hour()%24), (uint8_t)(ElapsedTime.minute()%60), (uint8_t)(ElapsedTime.second()%60));
	else
		sprintf_P(timer_string, PSTR("%02dD%02d:%02d"), (uint8_t)(ElapsedTime.day()%100), (uint8_t)(ElapsedTime.hour()%24), (uint8_t)(ElapsedTime.minute()%60));	
	DWIN_Draw_MaskString_Default(ELAPSEDTIME_STAR_X, 212, timer_string);
}

void Draw_Print_RemainTime() {
	#define	REMAINTIME_STAR_X	192
	duration_t RemainTime = HMI_Value.remain_time; // remain timer
	char timer_string[20];
	
	if(HMI_Value.remain_time <= 0)
		DWIN_Draw_MaskString_Default(REMAINTIME_STAR_X, 212, PSTR("--H--M"));
	else {		
		if(RemainTime.hour() < 24)
			sprintf_P(timer_string, PSTR("%02d:%02d "), (uint8_t)(RemainTime.hour()%24), (uint8_t)(RemainTime.minute()%60));
		else
			sprintf_P(timer_string, PSTR("%02dD%02dH"), (uint8_t)(RemainTime.day()%100), (uint8_t)(RemainTime.hour()%24));	
		DWIN_Draw_MaskString_Default(REMAINTIME_STAR_X, 212, timer_string);
	}	
}

#if(E_STEPPERS == 4)
 	#define	X_ICON_EXT1				 10	
	#define	X_ICON_EXT2				 62	
	#define	X_ICON_EXT3				114	
	#define	X_ICON_EXT4				166	
	#define	X_ICON_VTOOLFAN		218
	#define	Y_ICON_EXT1				 98
	#define	Y_ICON_EXT2				 98
	#define	Y_ICON_EXT3				 98
	#define	Y_ICON_EXT4				 98	
	#define	Y_ICON_VTOOLFAN		 98
#elif(E_STEPPERS == 3)
	#define	X_ICON_EXT1				 21	
	#define	X_ICON_EXT2				 84	
	#define	X_ICON_EXT3				147	
	#define	X_ICON_VTOOLFAN		210
	#define	Y_ICON_EXT1				 98
	#define	Y_ICON_EXT2				 98
	#define	Y_ICON_EXT3				 98
	#define	Y_ICON_VTOOLFAN		 98
#elif(E_STEPPERS == 2)
	#define	X_ICON_EXT1				 36	
	#define	X_ICON_EXT2				114	
	#define	X_ICON_VTOOLFAN		192
	#define	Y_ICON_EXT1				 98
	#define	Y_ICON_EXT2				 98
	#define	Y_ICON_VTOOLFAN		 98
#else
	#define	X_ICON_EXT1				 63	
	#define	X_ICON_VTOOLFAN		168
	#define	Y_ICON_EXT1				 98
	#define	Y_ICON_VTOOLFAN		 98
#endif

inline void Draw_Print_ShowExtruderICON() {
  DWIN_Show_ICON(ICON_EXTRUDER1_P, X_ICON_EXT1, Y_ICON_EXT1);
#if(E_STEPPERS > 1)
 	DWIN_Show_ICON(ICON_EXTRUDER2_P, X_ICON_EXT2, Y_ICON_EXT2);
#endif
#if(E_STEPPERS > 2)
 	DWIN_Show_ICON(ICON_EXTRUDER3_P, X_ICON_EXT3, Y_ICON_EXT3);
#endif
#if(E_STEPPERS > 3)
 	DWIN_Show_ICON(ICON_EXTRUDER4_P, X_ICON_EXT4, Y_ICON_EXT4);
#endif

#if ENABLED(OPTION_MIXING_SWITCH)
	if(!mixer.mixing_enabled)
		DWIN_Show_ICON(ICON_FAN_P, 	X_ICON_VTOOLFAN, Y_ICON_VTOOLFAN);
	else
#endif
		DWIN_Show_ICON(ICON_VTOOL_P, 	X_ICON_VTOOLFAN, Y_ICON_VTOOLFAN);  	
}

//////////////////////////////////////////////////////
//
// Extruder & Mixer Status Area
//
#if ENABLED(MIXING_EXTRUDER)
static void X_Start_Coordinate_Calculation(uint8_t Extruder_number, uint16_t Coordinate){
	const uint8_t Extruder_X_Start_Coordinate[5] = {0,0,34,19,8};
	const uint8_t Extruder_X_Start_Gap[5] = {0,0,78,63,52};
	MIXER_STEPPER_LOOP(i){
		if(Coordinate >= 100){
			MixerDis.Extruder_X_Coordinate[i] = Extruder_X_Start_Coordinate[MIXING_STEPPERS]+Extruder_X_Start_Gap[MIXING_STEPPERS]*i; 
			MixerDis.Extruder_Int_Number[i] = 3;
		}
		else if((Coordinate < 100)&&(Coordinate >= 10)){
			MixerDis.Extruder_X_Coordinate[i] = Extruder_X_Start_Coordinate[MIXING_STEPPERS]+7+Extruder_X_Start_Gap[MIXING_STEPPERS]*i;
			MixerDis.Extruder_Int_Number[i] = 2;
		}
		else if(Coordinate < 10){
			MixerDis.Extruder_X_Coordinate[i] = Extruder_X_Start_Coordinate[MIXING_STEPPERS]+14+Extruder_X_Start_Gap[MIXING_STEPPERS]*i;
			MixerDis.Extruder_Int_Number[i] = 1;
		}		
  }	
	//the last one is VTOOL
	if(Extruder_number == MIXING_STEPPERS){
		if((Coordinate < 100) && (Coordinate >= 10)){ 
			MixerDis.VTool_X_Coordinate = Extruder_X_Start_Coordinate[MIXING_STEPPERS]+7+Extruder_X_Start_Gap[MIXING_STEPPERS]*MIXING_STEPPERS;
			MixerDis.VTool_Int_Number = 2;
		}
		else if(Coordinate < 10){
			MixerDis.VTool_X_Coordinate = Extruder_X_Start_Coordinate[MIXING_STEPPERS]+14+Extruder_X_Start_Gap[MIXING_STEPPERS]*MIXING_STEPPERS;
			MixerDis.VTool_Int_Number = 1;
		}
	}
}

//#define	MIX_RATE_Y_COORDINAT	MixerDis.Y_Coordinat
#define	MIX_RATE_Y_COORDINAT	143
void DWIN_Refresh_ExtruerFAN_State(){	
	Clear_Dwin_Area(AREA_MIXVALUE);
#if ENABLED(OPTION_MIXING_SWITCH)
	if(!mixer.mixing_enabled){
		if(mixer.percentmix[0] == 100)
			DWIN_Show_ICON(ICON_EON, X_ICON_EXT1, MIX_RATE_Y_COORDINAT);
		else
			DWIN_Show_ICON(ICON_EOFF, X_ICON_EXT1, MIX_RATE_Y_COORDINAT);
		
	#if(E_STEPPERS > 1)
		if(mixer.percentmix[1] == 100)
		 	DWIN_Show_ICON(ICON_EON, X_ICON_EXT2, MIX_RATE_Y_COORDINAT);
		else
			DWIN_Show_ICON(ICON_EOFF, X_ICON_EXT2, MIX_RATE_Y_COORDINAT);
	#endif

	#if(E_STEPPERS > 2)
		if(mixer.percentmix[2] == 100)
		 	DWIN_Show_ICON(ICON_EON, X_ICON_EXT3, MIX_RATE_Y_COORDINAT);
		else
			DWIN_Show_ICON(ICON_EOFF, X_ICON_EXT3, MIX_RATE_Y_COORDINAT);
	#endif

	#if(E_STEPPERS > 3)
		if(mixer.percentmix[3] == 100)
		 	DWIN_Show_ICON(ICON_EON, X_ICON_EXT4, MIX_RATE_Y_COORDINAT);
		else
			DWIN_Show_ICON(ICON_EOFF, X_ICON_EXT4, MIX_RATE_Y_COORDINAT);
	#endif

		//FAN speed
		if(thermalManager.fan_speed[0] >= 100){
			MixerDis.VTool_Int_Number = 3;
			MixerDis.VTool_X_Coordinate = 6+52*4;
		}
		else if(thermalManager.fan_speed[0] >= 10){
			MixerDis.VTool_Int_Number = 2;
			MixerDis.VTool_X_Coordinate = 16+52*4;
		}
		else{
			MixerDis.VTool_Int_Number = 1;
			MixerDis.VTool_X_Coordinate = 24+52*4;
		}				
		DWIN_Draw_IntValue_FONT14(COLOR_WHITE, MixerDis.VTool_Int_Number, MixerDis.VTool_X_Coordinate, MIX_RATE_Y_COORDINAT, thermalManager.fan_speed[0]);
	}
	else
#endif
	{
		//Mix data		
		#if HAS_MIXER_SYNC_CHANNEL
		if(mixer.selected_vtool != MIXER_AUTORETRACT_TOOL)
		#endif	
		{
			MIXER_STEPPER_LOOP(i){
				X_Start_Coordinate_Calculation(i, mixer.percentmix[i]);
				DWIN_Draw_IntValue_FONT14(COLOR_WHITE, MixerDis.Extruder_Int_Number[i], MixerDis.Extruder_X_Coordinate[i], MIX_RATE_Y_COORDINAT, mixer.percentmix[i]);			
			}
		}
		//V-TOOL
		#if ENABLED(GRADIENT_MIX)
		if(mixer.gradient.enabled){
			X_Start_Coordinate_Calculation(MIXING_STEPPERS, 10);				
			dwinLCD.Draw_String(false, false, DWIN_FONT_MIX, COLOR_WHITE, COLOR_BG_BLACK, MixerDis.VTool_X_Coordinate, MIX_RATE_Y_COORDINAT, PSTR("Gr"));
		}
		else 
		#endif
		#if ENABLED(RANDOM_MIX)
		if(mixer.random_mix.enabled){
			X_Start_Coordinate_Calculation(MIXING_STEPPERS, 10);
			dwinLCD.Draw_String(false, false, DWIN_FONT_MIX, COLOR_WHITE, COLOR_BG_BLACK, MixerDis.VTool_X_Coordinate, MIX_RATE_Y_COORDINAT, PSTR("Rd"));
		}
		else
		#endif
		//if(mixer.selected_vtool <= MIXING_VIRTUAL_TOOLS)
		{
	    X_Start_Coordinate_Calculation(MIXING_STEPPERS, mixer.selected_vtool);
			DWIN_Draw_IntValue_FONT14(COLOR_WHITE, MixerDis.VTool_Int_Number, MixerDis.VTool_X_Coordinate, MIX_RATE_Y_COORDINAT, mixer.selected_vtool);
		}
	}
}

void Draw_Print_ProgressMixModel(){	
	#if ENABLED(OPTION_MIXING_SWITCH)
	if(!mixer.mixing_enabled) return;
	#endif
	#if HAS_MIXER_SYNC_CHANNEL
 	if(mixer.selected_vtool == MIXER_AUTORETRACT_TOOL) return;
	#endif	
	char string_buf[50] = {0};
#if ENABLED(GRADIENT_MIX)
	if(mixer.gradient.enabled) {
		if(HMI_Value.old_mix_mode == 1) return;
		HMI_Value.old_mix_mode = 1;				
		//Gradient Mix Z:xxx.x->xxx.x V:xx->xx
		sprintf_P(string_buf, PSTR("Gradient Mix Z:%3.1f->%3.1f V:%2d->%2d"), mixer.gradient.start_z, mixer.gradient.end_z, mixer.gradient.start_vtool, mixer.gradient.end_vtool);
		DWIN_Show_Status_Message(COLOR_WHITE, string_buf, 10);
	}
	else 
#endif
#if ENABLED(RANDOM_MIX)
	if(mixer.random_mix.enabled) {
		if(HMI_Value.old_mix_mode == 2) return;
		HMI_Value.old_mix_mode = 2;			
		//Random Mix Z:xxx.x->xxx.x H:xxx.x E:x
		sprintf_P(string_buf, PSTR("Random Mix Z:%3.1f->%3.1f H:%3.1f E:%1d"), mixer.random_mix.start_z, mixer.random_mix.end_z, mixer.random_mix.height, mixer.random_mix.extruders);
		DWIN_Show_Status_Message(COLOR_WHITE, string_buf, 10);
	}
	else 
#endif
	if(HMI_Value.old_mix_mode != 0){
		HMI_Value.old_mix_mode = 0;
		sprintf_P(string_buf, PSTR("Current VTOOL = %2d"), mixer.selected_vtool);
		DWIN_Show_Status_Message(COLOR_WHITE, string_buf, 5);
	}
}
#endif

inline void Draw_Printing_Screen() {
 DWIN_Show_MultiLanguage_String(MTSTRING_TUNE, GET_ICON_X(TUNE), 325);
 DWIN_Show_MultiLanguage_String(MTSTRING_PAUSE, GET_ICON_X(PAUSE), 325);
 DWIN_Show_MultiLanguage_String(MTSTRING_STOP, GET_ICON_X(STOP), 325);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Tune
//
static void Item_Tune_Speed(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_SPEED, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_SETSPEED);
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), feedrate_percentage);
}

#if ENABLED(OPTION_FLOWRATE_MENU)
static void Item_Tune_Flowrate(const uint8_t row) {
#if	(DWINLCD_MENU_VERSION >= 3)
	DWIN_Show_MultiLanguage_String(MTSTRING_FLOWRATE, LBLX, MBASE(row));
#else
	DWIN_Draw_UnMaskString_Default(LBLX, MBASE(row), PSTR("Flow Rate"));
#endif
	Draw_Menu_Line(row, ICON_FLOWRATE);
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), planner.flow_percentage[active_extruder]);
}
#endif

#if HAS_HOTEND
static void Item_Tune_ETemp(const uint8_t row){
	DWIN_Show_MultiLanguage_String(MTSTRING_NOZZLE, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_TEMP, LBLX+get_MultiLanguageString_Width(MTSTRING_NOZZLE)+6, MBASE(row));	
	Draw_Menu_Line(row, ICON_HOTENDTEMP);
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), thermalManager.degTargetHotend(0));
}
#endif

#if HAS_HEATED_BED
static void Item_Tune_BTemp(const uint8_t row){
	DWIN_Show_MultiLanguage_String(MTSTRING_BED, LBLX, MBASE(row));
	DWIN_Show_MultiLanguage_String(MTSTRING_TEMP, LBLX+get_MultiLanguageString_Width(MTSTRING_BED)+6, MBASE(row));	
	Draw_Menu_Line(row, ICON_BEDTEMP);
  DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), thermalManager.degTargetBed());
}
#endif

#if HAS_FAN
static void Item_Tune_FanSpeed(const uint8_t row){
	DWIN_Show_MultiLanguage_String(MTSTRING_FANSPEED, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_FANSPEED);
  DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), thermalManager.fan_speed[0]);
}
#endif

#if ENABLED(BABYSTEPPING)
static void Item_Tune_ZOffset(const uint8_t row){
	DWIN_Show_MultiLanguage_String(MTSTRING_Z_OFFSET, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_ZOFFSET);
  DWIN_Draw_Small_Float22(MENUVALUE_X, MBASE(row), HMI_Value.babyZoffset_Scale);
}
#endif

#if ENABLED(MIXING_EXTRUDER)
static void Item_Tune_Mixer(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_MIXER, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_MIXER);
	Draw_More_Icon(row);
}
#endif

static void Item_Tune_Config(const uint8_t row) {
	DWIN_Show_MultiLanguage_String(MTSTRING_CONFIG, LBLX, MBASE(row));
	Draw_Menu_Line(row, ICON_CONTACT);
	Draw_More_Icon(row);
}

void Draw_Tune_Menu(const uint8_t MenuItem){
	DwinMenuID = DWMENU_TUNE;	
	DwinMenu_tune.set(MenuItem);
	DwinMenu_tune.index = _MAX(DwinMenu_tune.now, MROWS);

	HMI_flag.autoreturntime = 8;
	
#if (TUNE_CASE_TOTAL > MROWS)
	const int8_t kscroll = MROWS - DwinMenu_tune.index; // Scrolled-up lines
	#define KCSCROL(L) (kscroll + (L))
#else
	#define KCSCROL(L) (L)
#endif
	#define KCVISI(L) WITHIN(KCSCROL(L), 0, MROWS)

	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_BOTTOM);

	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_TUNE, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	if (KCVISI(TUNE_CASE_BACK)) Draw_Back_First(DwinMenu_tune.now == TUNE_CASE_BACK);
	
	if (KCVISI(TUNE_CASE_SPEED)) Item_Tune_Speed(KCSCROL(TUNE_CASE_SPEED));
#if ENABLED(OPTION_FLOWRATE_MENU)
	if (KCVISI(TUNE_CASE_FLOWRATE)) Item_Tune_Flowrate(KCSCROL(TUNE_CASE_FLOWRATE));
#endif
#if HAS_HOTEND
	if (KCVISI(TUNE_CASE_ETEMP)) Item_Tune_ETemp(KCSCROL(TUNE_CASE_ETEMP));
#endif
#if HAS_HEATED_BED
	if (KCVISI(TUNE_CASE_BTEMP)) Item_Tune_BTemp(KCSCROL(TUNE_CASE_BTEMP));	
#endif 
#if HAS_FAN
	if (KCVISI(TUNE_CASE_FAN)) Item_Tune_FanSpeed(KCSCROL(TUNE_CASE_FAN));
#endif
#if ENABLED(BABYSTEPPING)
	if (KCVISI(TUNE_CASE_ZOFF)) Item_Tune_ZOffset(KCSCROL(TUNE_CASE_ZOFF)); 
#endif
	if (KCVISI(TUNE_CASE_CONFIG)) Item_Tune_Config(KCSCROL(TUNE_CASE_CONFIG)); 
#if ENABLED(MIXING_EXTRUDER)
	#if ENABLED(OPTION_MIXING_SWITCH)
	if(mixer.mixing_enabled){
		if (KCVISI(TUNE_CASE_MIXER)) Item_Tune_Mixer(KCSCROL(TUNE_CASE_MIXER));
	}
	#else
	if (KCVISI(TUNE_CASE_MIXER)) Item_Tune_Mixer(KCSCROL(TUNE_CASE_MIXER)); 
	#endif
#endif
		
	if (DwinMenu_tune.now) Draw_Menu_Cursor(KCSCROL(DwinMenu_tune.now));
}

//
// Draw Babystep Windows
//
#if ENABLED(BABYSTEPPING)
#if HAS_OFFSET_MENU
#define	SAVE_ZOFFSET_INTIME
#endif

inline bool IS_printer_busy() { 
	return planner.movesplanned() || printingIsActive(); 
}

inline void _init_baby_zoffset(){
#if ENABLED(BABYSTEP_DISPLAY_TOTAL)
	babyzoffset.first = - home_offset.z;
#else
	babyzoffset.first = 0.0;
#endif
	babyzoffset.changed = 0.0;
	babyzoffset.last = babyzoffset.current = babyzoffset.first;
	babystep.axis_total[BS_TOTAL_IND(Z_AXIS)] = babyzoffset.last * planner.settings.axis_steps_per_mm[Z_AXIS];
	HMI_Value.babyZoffset_Scale = babyzoffset.last * MAXUNITMULT;
}

/****
	if baby Z offset is ajusted while printing, save it to home Z offset.
*/
#if HAS_OFFSET_MENU
inline void _Apply_ZOffset(){	
	if((ENABLED(BABYSTEP_WITHOUT_HOMING) || all_axes_known()) && (ENABLED(BABYSTEP_ALWAYS_AVAILABLE) || IS_printer_busy())){				
		babyzoffset.current  = (float)HMI_Value.babyZoffset_Scale/MAXUNITMULT;
		if(babyzoffset.last != babyzoffset.current ){			 
			 babystep.add_mm(Z_AXIS, babyzoffset.current - babyzoffset.last);
			 babyzoffset.last = babyzoffset.current;
		}
	}		
}

#if ENABLED(SAVE_ZOFFSET_INTIME)
inline void save_Zoffset(){
	if(ABS(babyzoffset.last - babyzoffset.first) > 0.1) {		
		home_offset.z += (babyzoffset.first - babyzoffset.last - TERN0(SAVE_ZOFFSET_INTIME, babyzoffset.changed));
		settings.save();
		TERN_(SAVE_ZOFFSET_INTIME, babyzoffset.changed = (babyzoffset.first - babyzoffset.last));
		DWIN_Show_Status_Message(COLOR_WHITE, PSTR("Home Z offset Updated!"), 6);
	}	
}
#endif

#endif

inline void Draw_Babystep_Menu() {	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_TUNE, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	dwinLCD.Draw_String(false, false, font14x28, COLOR_WHITE, COLOR_BG_BLACK, 10, 160, PSTR("Babystep:"));
	DWIN_Draw_Big_Float22(170, 160, HMI_Value.babyZoffset_Scale);
	Babystep_firstclicktime = millis();
}


void HMI_Pop_BabyZstep() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.babyZoffset_Scale) 
			&& (millis() >= Babystep_firstclicktime + DOUBLECLICK_MAX_INTERVAL)) {
			EncoderRate.enabled = false;
			HMI_flag.autoreturntime = 0;
			Draw_Printing_Menu();
			TERN_(SAVE_ZOFFSET_INTIME, save_Zoffset());
		}
		else {
			Babystep_firstclicktime = millis() - DOUBLECLICK_MAX_INTERVAL;
			HMI_flag.autoreturntime = 8;
			NOLESS(HMI_Value.babyZoffset_Scale, -500);
		 	NOMORE(HMI_Value.babyZoffset_Scale, 500); 	
			_Apply_ZOffset();
		  DWIN_Draw_Big_Float22(170, 160, HMI_Value.babyZoffset_Scale);	  
		}
		dwinLCD.UpdateLCD();
	}
}

void HMI_TuneZoffset() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	uint8_t zoff_line = DwinMenu_tune.now + MROWS - DwinMenu_tune.index;
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.babyZoffset_Scale)){
			DWIN_Draw_Small_Float22(MENUVALUE_X, MBASE(zoff_line), HMI_Value.babyZoffset_Scale);
			EncoderRate.enabled = false;
			DwinMenuID = DWMENU_TUNE;
			TERN_(SAVE_ZOFFSET_INTIME, save_Zoffset());			
		}
		else{
			NOLESS(HMI_Value.babyZoffset_Scale, -500);
			NOMORE(HMI_Value.babyZoffset_Scale, 500);
			_Apply_ZOffset();
			DWIN_Draw_Selected_Small_Float22(MENUVALUE_X, MBASE(zoff_line), HMI_Value.babyZoffset_Scale);
		}
		dwinLCD.UpdateLCD();
	}
}
#endif // BABYSTEPPING

void HMI_TunePrintSpeed() {
	// Print speed limit
	#define MAX_PRINT_SPEED   500
	#define MIN_PRINT_SPEED   10
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.print_speed)) {
			DwinMenuID = DWMENU_TUNE;
			EncoderRate.enabled = false;
			feedrate_percentage = HMI_Value.print_speed;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(DwinMenu_tune.now + MROWS - DwinMenu_tune.index), HMI_Value.print_speed);
		}
		else{
			// print_speed limit
			NOMORE(HMI_Value.print_speed, MAX_PRINT_SPEED);
			NOLESS(HMI_Value.print_speed, MIN_PRINT_SPEED);
			// print_speed value
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(DwinMenu_tune.now + MROWS - DwinMenu_tune.index), HMI_Value.print_speed);
		}
		dwinLCD.UpdateLCD();
	}
}

#if ENABLED(OPTION_FLOWRATE_MENU)
void HMI_TuneFlowrate() {
	// flow rate limit
	#define MAX_FLOW_RATE   150
	#define MIN_FLOW_RATE   75
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.flowrate)) {
			DwinMenuID = DWMENU_TUNE;
			EncoderRate.enabled = false;
			planner.flow_percentage[active_extruder] = HMI_Value.flowrate;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(DwinMenu_tune.now + MROWS - DwinMenu_tune.index), HMI_Value.flowrate);			
		}
		else{
			// print_speed limit
			NOMORE(HMI_Value.flowrate, MAX_FLOW_RATE);
			NOLESS(HMI_Value.flowrate, MIN_FLOW_RATE);
			// print_speed value
			DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(DwinMenu_tune.now + MROWS - DwinMenu_tune.index), HMI_Value.flowrate);
		}
		dwinLCD.UpdateLCD();
	}
}
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Draw Popup Windows
//
void Popup_Window_Resume() {
	Clear_Dwin_Area(AREA_TITEL|AREA_POPMENU);
	Draw_Popup_Bkgd_105();

	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 14) / 2, 115, PSTR("Continue Print"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 22) / 2, 192, PSTR("It looks like the last"));
	DWIN_Draw_MaskString_Default_PopMenu( (272 - 10 * 22) / 2, 212, PSTR("file was interrupted."));
	DWIN_Show_ICON(ICON_CANCEL_E,  26, 307);
#if (DWINLCD_MENU_VERSION >= 2)
	DWIN_Show_ICON(ICON_CONTINUE_C, 146, 307);
#else
	DWIN_Show_ICON(ICON_CONTINUE_E, 146, 307);
#endif
}

static void Popup_window_PauseOrStop() {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();

	if (DwinMenu_print.now == PRINT_CASE_PAUSE) 
		DWIN_Draw_MaskString_FONT12(POP_TEXT_COLOR, COLOR_BG_WINDOW, (272 - 12 * 12) / 2, 150, PSTR("Pause Print?"));
	else if (DwinMenu_print.now == PRINT_CASE_STOP) 
		DWIN_Draw_MaskString_FONT12(POP_TEXT_COLOR, COLOR_BG_WINDOW, (272 - 12 * 11) / 2, 150, PSTR("Stop Print?"));
#if (DWINLCD_MENU_VERSION >= 2)	
	DWIN_Show_ICON(ICON_CONFIRM_C, 26, 280);
	DWIN_Show_ICON(ICON_CANCEL_E, 146, 280);
	Draw_Select_Highlight(false);
#else
	DWIN_Show_ICON(ICON_CONFIRM_E, 26, 280);
	DWIN_Show_ICON(ICON_CANCEL_E, 146, 280);
	Draw_Select_Highlight(true);
#endif
}

inline void Popup_Window_StopWaiting(){
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DwinMenuID = DWMENU_POP_WAITING;
	HMI_flag.killtimes = 0;
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DWIN_Draw_String_FIL((272 - 11 * 12)/2, 240, PSTR("Stopping..."));
	DWIN_Draw_String_FIL((272 - 15 * 12)/2, 269, PSTR("Please wait!"));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DWIN_start_SDPrint(){
	DWIN_status = ID_SM_PRINTING;
	TERN_(BABYSTEPPING, _init_baby_zoffset());
	DwinMenu_print.reset();
	Draw_Printing_Menu(0, true);
}

void Draw_Printing_Menu(const uint8_t MenuItem, const bool with_update) {
	if((DwinMenuID == DWMENU_PRINTING) && (DWIN_status == ID_SM_PRINTING)) return;
	
	DwinMenuID = DWMENU_PRINTING;
	DwinMenu_print.set(MenuItem);
	DwinMenu_print.index = _MAX(DwinMenu_print.now, MROWS);
	
	DWIN_status = ID_SM_PRINTING;
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_STATUS);
	
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_SDPRINT, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);

	Draw_Print_ProgressBar();
	Draw_Print_ShowExtruderICON();	
#if ENABLED(MIXING_EXTRUDER)
	DWIN_Refresh_ExtruerFAN_State();
	Draw_Print_ProgressMixModel();
#endif	
	//Copy into file buf string before entry
	char * const name = card.longest_filename();
	const int8_t npos = _MAX(0U, DWIN_WIDTH - strlen(name) * MENU_CHR_W) / 2;
	DWIN_Draw_UnMaskString_Default(npos, 40, name);

	DWIN_Show_ICON(ICON_PRINTTIME, 17, 193);
	DWIN_Show_ICON(ICON_PRINT_TIME, 42, 193);
	DWIN_Show_ICON(ICON_REMAINTIME, 150, 191);	
	DWIN_Show_ICON(ICON_REMAIN_TIME, 176, 193);
	Draw_Print_ElapsedTime();
	Draw_Print_RemainTime();	

	//Draw_Printing_Screen();
	Draw_ICON_Printingstatus();	
	Draw_Status_Area();

	if(with_update) dwinLCD.UpdateLCD();	
}

inline void DWIN_resume_print() {
#if ENABLED(POWER_LOSS_RECOVERY)
	ui.IS_lcd_pause = false;
#endif

	TERN_(PARK_HEAD_ON_PAUSE, wait_for_heatup = wait_for_user = false);		
#if ENABLED(PAUSE_HEAT)
	thermalManager.setTargetBed(tempbed);
	thermalManager.setTargetHotend(temphot,0);
#endif
	queue.inject_P(M24_STR);
	TERN_(ACTION_ON_RESUME, host_action_resume());
	print_job_timer.start(); // Also called by M24
}

static void DWIN_Kill_Waiting(){
	DWIN_FEEDBACK_WARNNING();
	HMI_flag.killtimes++;
	if(HMI_flag.killtimes <= 2){
		DWIN_Show_Status_Message(COLOR_RED, PSTR("Is processing, please wait!"));
		HMI_flag.killElapsedTime = 2;
	}	
	else if(HMI_flag.killtimes == 3){
		DWIN_Show_Status_Message(COLOR_RED, PSTR("Press agin to kill!"));
		HMI_flag.killElapsedTime = 2;
	}
	else if(HMI_flag.killtimes >= 4){
		DWIN_Show_Status_Message(COLOR_RED, PSTR("killed!!"));		
		HMI_flag.killtimes = 0;
		HMI_flag.killElapsedTime = 0;
		Abort_SD_Printing();
	}
}	

static void _Dwin_pause_print() {
	#if ENABLED(POWER_LOSS_RECOVERY)
	ui.IS_lcd_pause = true;
	ui.Lcd_pause_zpos = current_position.z;
	#endif	
	TERN_(HOST_PROMPT_SUPPORT, host_prompt_open(PROMPT_PAUSE_RESUME, PSTR("UI Pause"), PSTR("Resume")));	
	#if ENABLED(PARK_HEAD_ON_PAUSE)
		// Show message immediately to let user know about pause in progress
		DWIN_Pause_Show_Message(PAUSE_MESSAGE_PARKING, PAUSE_MODE_PAUSE_PRINT); 
		queue.inject_P(PSTR("M25 P\nM24"));
	#elif ENABLED(SDSUPPORT)
		queue.inject_P(PSTR("M25"));
	#elif defined(ACTION_ON_PAUSE)
		host_action_pause();
	#endif
}

/* Pause and Stop window */
void DWIN_StopPrintFromSDCard(){			
#if (HAS_OFFSET_MENU && ENABLED(BABYSTEPPING) && DISABLED(SAVE_ZOFFSET_INTIME))
	save_Zoffset();
#endif
#if ENABLED(OPTION_ABORT_UNLOADFILAMENT)
	HMI_flag.AutoUnloadFilament_on = false;
	if(HMI_flag.AutoUnload_enabled && !mixer.mixing_enabled && sum_extrude > 5 && thermalManager.temp_hotend[0].celsius > 150){ 
		HMI_flag.AutoUnloadFilament_on = true;
		DWIN_Show_Status_Message(COLOR_RED, PSTR("Unload filament..."));					
	}
#endif
	Popup_Window_StopWaiting();
	Abort_SD_Printing();
}

void HMI_PauseOrStop() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if(encoder_diffState == ENCODER_DIFF_NO) return;

	if(encoder_diffState == ENCODER_DIFF_CW){
#if 0//BOTH(BABYSTEPPING, DOUBLECLICK_FOR_Z_BABYSTEPPING)
		Babystep_firstclicktime = 0;
#endif
#if (DWINLCD_MENU_VERSION >= 2)
		DWIN_Show_ICON(ICON_CONFIRM_C, 26, 280);
		DWIN_Show_ICON(ICON_CANCEL_E, 146, 280);
#endif
		Draw_Select_Highlight(false);
	}
	else if(encoder_diffState == ENCODER_DIFF_CCW){
#if 0//BOTH(BABYSTEPPING, DOUBLECLICK_FOR_Z_BABYSTEPPING)
		Babystep_firstclicktime = 0;
#endif		
#if (DWINLCD_MENU_VERSION >= 2)
		DWIN_Show_ICON(ICON_CONFIRM_E, 26, 280);
		DWIN_Show_ICON(ICON_CANCEL_C, 146, 280);
#endif
		Draw_Select_Highlight(true);
	}
	else if(encoder_diffState == ENCODER_DIFF_ENTER){		
#if 0//BOTH(BABYSTEPPING, DOUBLECLICK_FOR_Z_BABYSTEPPING)
		if((ENABLED(BABYSTEP_WITHOUT_HOMING) || all_axes_known()) && (ENABLED(BABYSTEP_ALWAYS_AVAILABLE) || printer_busy())){
			if(millis() < Babystep_firstclicktime + (DOUBLECLICK_MAX_INTERVAL/2) && !axes_should_home()) {				
				DwinMenuID = DWMENU_TUNE_BABYSTEPS; 		
				Draw_Babystep_Menu();			
				dwinLCD.UpdateLCD();
				return;
			}
		}
#endif
		if(DwinMenu_print.now == PRINT_CASE_PAUSE){
			if(HMI_flag.select_flag){
				tempbed = thermalManager.degTargetBed();
				temphot = thermalManager.degTargetHotend(0);
				if(printingIsActive()) _Dwin_pause_print();
			}
			else{
				//redraw printing menu
				Draw_Printing_Menu(PRINT_CASE_PAUSE, true); // cancel pause				
			}
		}
		else if(DwinMenu_print.now == PRINT_CASE_STOP){
			if(HMI_flag.select_flag){				
				DWIN_StopPrintFromSDCard();
			}
			else{
				//redraw printing menu
				Draw_Printing_Menu(PRINT_CASE_STOP, true); // cancel stop
			}
		}
	}
	dwinLCD.UpdateLCD();
}


/* Tune */
void HMI_Tune() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	HMI_flag.autoreturntime = 8;
	
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_tune.inc(TUNE_CASE_END + mixing_menu_adjust())) {
			if (DwinMenu_tune.now > MROWS && DwinMenu_tune.now > DwinMenu_tune.index) {
				DwinMenu_tune.index = DwinMenu_tune.now;
				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);
				if(DwinMenu_tune.index == TUNE_CASE_CONFIG)	Item_Tune_Config(MROWS);
			#if ENABLED(MIXING_EXTRUDER)
				else if(DwinMenu_tune.index == TUNE_CASE_MIXER) Item_Tune_Mixer(MROWS);
			#endif
			#if ENABLED(BABYSTEPPING)
				else if(DwinMenu_tune.index == TUNE_CASE_ZOFF) Item_Tune_ZOffset(MROWS);
			#endif
			}
			else {
				Move_Highlight(1, DwinMenu_tune.now + MROWS - DwinMenu_tune.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_tune.dec()) {
			if (DwinMenu_tune.now < DwinMenu_tune.index - MROWS) {
				DwinMenu_tune.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if(DwinMenu_tune.index - MROWS == TUNE_CASE_BACK)	Draw_Back_First();
			#if HAS_FAN
				else if(DwinMenu_tune.index - MROWS == TUNE_CASE_FAN) Item_Tune_FanSpeed(0);
			#endif
			#if HAS_HEATED_BED
				else if(DwinMenu_tune.index - MROWS == TUNE_CASE_BTEMP) Item_Tune_BTemp(0);
			#endif
			#if HAS_HOTEND
				else if(DwinMenu_tune.index - MROWS == TUNE_CASE_ETEMP) Item_Tune_ETemp(0);
			#endif
			#if ENABLED(OPTION_FLOWRATE_MENU)				
				else if(DwinMenu_tune.index - MROWS == TUNE_CASE_FLOWRATE) Item_Tune_Flowrate(0);
			#endif
				else if(DwinMenu_tune.index - MROWS == TUNE_CASE_SPEED) Item_Tune_Speed(0);								
			}
			else {
				Move_Highlight(-1, DwinMenu_tune.now + MROWS - DwinMenu_tune.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_tune.now) {
		 	case 0: // Back			 		
			#if BOTH(BABYSTEPPING, DOUBLECLICK_FOR_Z_BABYSTEPPING)
			  if((ENABLED(BABYSTEP_WITHOUT_HOMING) || all_axes_known()) && (ENABLED(BABYSTEP_ALWAYS_AVAILABLE) || printer_busy())){
					if(millis() < Babystep_firstclicktime + DOUBLECLICK_MAX_INTERVAL && !axes_should_home()) {
						DwinMenuID = DWMENU_TUNE_BABYSTEPS; 		
						Draw_Babystep_Menu();
						break;
					}
				}
				Babystep_firstclicktime = 0;
			#endif
		  	DwinMenu_print.reset();
		  	Draw_Printing_Menu();
		 		break;
				
		 	case TUNE_CASE_SPEED: // Print speed
		  	DwinMenuID = DWMENU_TUNE_PRINTSPEED;
		  	HMI_Value.print_speed = feedrate_percentage;
		  	DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(TUNE_CASE_SPEED + MROWS - DwinMenu_tune.index), HMI_Value.print_speed);
		  	EncoderRate.enabled = true;
		  	break;

		#if ENABLED(OPTION_FLOWRATE_MENU)
			case TUNE_CASE_FLOWRATE: // flow rate
				DwinMenuID = DWMENU_TUNE_FLOWRATE;
		  	HMI_Value.flowrate = planner.flow_percentage[active_extruder];
		  	DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(TUNE_CASE_FLOWRATE + MROWS - DwinMenu_tune.index), HMI_Value.flowrate);
		  	EncoderRate.enabled = true;
		  	break;
		#endif
				
		#if HAS_HOTEND
		 	case TUNE_CASE_ETEMP: // Nozzle temp
				DwinMenuID = DWMENU_SET_ETMP;
				HMI_Value.E_Temp = thermalManager.degTargetHotend(0);
				if(HMI_Value.E_Temp > HOTEND_WARNNING_TEMP)
					DWIN_Draw_Warn_IntValue_Default(3, MENUVALUE_X+8, MBASE(TUNE_CASE_ETEMP + MROWS - DwinMenu_tune.index), HMI_Value.E_Temp);
				else
					DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(TUNE_CASE_ETEMP + MROWS - DwinMenu_tune.index), HMI_Value.E_Temp);
		  	EncoderRate.enabled = true;
		  	break;
		#endif
		
		#if HAS_HEATED_BED
		 	case TUNE_CASE_BTEMP: // Bed temp
	  	 	DwinMenuID = DWMENU_SET_BTMP;
	  		HMI_Value.Bed_Temp = thermalManager.degTargetBed();
	  		DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(TUNE_CASE_BTEMP + MROWS - DwinMenu_tune.index), HMI_Value.Bed_Temp);
	  		EncoderRate.enabled = true;
		  	break;
		#endif
		
		#if HAS_FAN
		 	case TUNE_CASE_FAN: // Fan speed
	  		DwinMenuID = DWMENU_SET_FANSPEED;
	  		HMI_Value.Fan_speed = thermalManager.fan_speed[0];
	  		DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(TUNE_CASE_FAN + MROWS - DwinMenu_tune.index), HMI_Value.Fan_speed);
	  		EncoderRate.enabled = true;
		  	break;
		#endif
		
		#if ENABLED(BABYSTEPPING)
		 	case TUNE_CASE_ZOFF: // Z-offset		 	
		   	DwinMenuID = DWMENU_SET_ZOFFSET;
		  	DWIN_Draw_Selected_Small_Float22(MENUVALUE_X, MBASE(TUNE_CASE_ZOFF + MROWS - DwinMenu_tune.index), HMI_Value.babyZoffset_Scale);
		  	EncoderRate.enabled = true;
		 	break;
		#endif

			case TUNE_CASE_CONFIG:
		 		Draw_Config_Menu();
		 		break;
			
		#if ENABLED(MIXING_EXTRUDER)
			 case TUNE_CASE_MIXER:
				 Draw_Mixer_Menu();
				 break;
		#endif
			
		 	default: break;
		} 			
	}
	dwinLCD.UpdateLCD();
}

void DWIN_Draw_PrintDone_Confirm(){	
#if ENABLED(OPTION_REPEAT_PRINTING)		
	if(ReprintManager.enabled && ReprintManager.is_removing())	return;
#endif
	// show print done confirm
	DwinMenuID = DWMENU_POP_STOPPRINT;
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_105();	
	DWIN_Draw_MaskString_FONT12(POP_TEXT_COLOR, COLOR_BG_WINDOW, (272 - 11 * 12) / 2, 150, PSTR("Print Done,"));
	//show print elapsed time
	duration_t elapsed = HMI_Value.elapsed_value;
	char timer_string[30];
	sprintf_P(timer_string, PSTR("Elapsed time:%dHr%dMin"), (int)(elapsed.hour()%1000), (uint8_t)(elapsed.minute()%60));
	DWIN_Draw_MaskString_Default_PopMenu((272 - 24 * 10) / 2, 200, PSTR(timer_string));	
	//
	DWIN_Draw_MaskString_FONT12(POP_TEXT_COLOR, COLOR_BG_WINDOW, (272 - 14 * 12) / 2, 240, PSTR("Click to exit!"));
	dwinLCD.ICON_Show(ICON_IMAGE_ID,ICON_CONFIRM_E, 86, 280);
#if (ENABLED(BABYSTEPPING) && HAS_OFFSET_MENU )
	save_Zoffset();
#endif
}

static void DRAW_Pause_Mode(const PauseMode mode){
	static PauseMode old_pausmode = PAUSE_MODE_PAUSE_PRINT;
	if(mode != PAUSE_MODE_SAME)	old_pausmode = mode;

#if ENABLED(FILAMENT_RUNOUT_SENSOR)	
	if(runout.enabled && runout.filament_ran_out){
		Draw_Title(PSTR("Run out..."));
		return;
	}
#endif

	switch(old_pausmode){
		case PAUSE_MODE_PAUSE_PRINT:
				Draw_Title(PSTR("Pause..."));				
			break;
		case PAUSE_MODE_CHANGE_FILAMENT:
				Draw_Title(PSTR("Filament Change"));
			break;
		case PAUSE_MODE_LOAD_FILAMENT:
				Draw_Title(PSTR("Filament Load"));
			break;
		case PAUSE_MODE_UNLOAD_FILAMENT:
				Draw_Title(PSTR("Filament Unload"));
			break;			
		case PAUSE_MODE_SAME:
		default:break;
	}
}

static void Popup_window_Pause_Parking(const PauseMode mode) {
	 Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	 Draw_Popup_Bkgd_60();
	 DWIN_Show_ICON(ICON_WAITING, 86, 105);
	 DRAW_Pause_Mode(mode);
	 DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 15) / 2, 240, PSTR("Parking hotend!"));
}

static void Popup_window_Pause_Start(const PauseMode mode) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DRAW_Pause_Mode(mode);
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 15) / 2, 240, PSTR("Filament change"));
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 9) / 2, 269, PSTR("to start!"));
}

static void Popup_window_Pause_Heating(const PauseMode mode) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DRAW_Pause_Mode(mode);
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 8) / 2, 240, PSTR("Wait for"));
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 17) / 2, 269, PSTR("Nozzle Heating..."));
}

static void Popup_window_Pause_startHeat(const PauseMode mode) {
	DwinMenuID = DWMENU_POP_FROD_HEAT;
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DRAW_Pause_Mode(mode);
	DWIN_Show_ICON(ICON_CONFIRM_E, 86, 168);
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 15) / 2, 211, PSTR("Nozzle is cool."));
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 14) / 2, 240, PSTR("Press the knob"));
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 8) / 2, 269, PSTR("to heat!"));
}

static void Popup_window_Pause_Insert(const PauseMode mode) {
	DwinMenuID = DWMENU_POP_FROD_INSERT;
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DRAW_Pause_Mode(mode);
	DWIN_Show_ICON(ICON_CONFIRM_E, 86, 168);
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 19) / 2, 211, PSTR("Insert filament and"));
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 16) / 2, 240, PSTR("press the button"));
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 12) / 2, 269, PSTR("to continue!"));
}

static void Popup_window_Pause_Unload(const PauseMode mode) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DRAW_Pause_Mode(mode);
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 10) / 2, 240, PSTR("Wait for..."));
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 16) / 2, 269, PSTR("Filament Unload!"));
}

static void Popup_window_Pause_Load(const PauseMode mode) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DRAW_Pause_Mode(mode);
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 10) / 2, 240, PSTR("Wait for..."));
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 14) / 2, 269, PSTR("Filament Load!"));
}

static void Popup_window_Pause_Purge(const PauseMode mode) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DRAW_Pause_Mode(mode);
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 12) / 2, 240, PSTR("Wait for..."));
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 15) / 2, 269, PSTR("Filament Purge!"));
}

static void Popup_window_Pause_Option(const PauseMode mode) {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DRAW_Pause_Mode(mode);	
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 11) / 2, 160, PSTR("Purge more?"));
	DwinMenuID = DWMENU_POP_FROD_OPTION;
	TERN_(FILAMENT_RUNOUT_SENSOR, HMI_flag.IS_cancel_runout = false);
	ICON_YESorNO(DwinMenu_RunoutOption.now);	
}

static void Popup_window_Pause_Resume(const PauseMode mode) {	
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Show_ICON(ICON_WAITING, 86, 105);
	DRAW_Pause_Mode(mode);
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 13) / 2, 240, PSTR("Resume print..."));
}

static void DRAW_Pause_Message(PauseMessage message, PauseMode mode){
	#if ENABLED(DEBUG_DWIN_LCD)
		SERIAL_ECHOPAIR("Pause message = ", message);
		SERIAL_ECHOLNPAIR("  Pause mode = ", mode);		
	#endif
	DwinMenuID = DWMENU_POP_WAITING;
	if(IS_SD_PRINTING() || IS_SD_PAUSED()){
	#if ENABLED(FILAMENT_RUNOUT_SENSOR)		
		if(runout.enabled && runout.filament_ran_out)
				DWIN_status = ID_SM_RUNOUTING;
		else 
	#endif
			DWIN_status = ID_SM_PAUSING;		
	}
	switch (message){
		case PAUSE_MESSAGE_PARKING://0
			Popup_window_Pause_Parking(mode);
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Is parking, please wait..."));
			break;
		case PAUSE_MESSAGE_CHANGING:			
			tempbed = thermalManager.degTargetBed();
			temphot = thermalManager.degTargetHotend(0);			
			Popup_window_Pause_Start(mode);
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Is changing, please wait..."));			
			break;
		case PAUSE_MESSAGE_WAITING:
			Popup_window_Pause_Start(mode);
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Please wait restart..."));
			break;
		case PAUSE_MESSAGE_UNLOAD:
			Popup_window_Pause_Unload(mode);		
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Unload Filament..."));
			break;
		case PAUSE_MESSAGE_INSERT:
			Popup_window_Pause_Insert(mode);			
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Wait for inserting Filament"));
			break;
		case PAUSE_MESSAGE_LOAD://5
			Popup_window_Pause_Load(mode);			
			Clear_Dwin_Area(AREA_BOTTOM);
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Is Loading,  please wait!"));
			break;
		case PAUSE_MESSAGE_PURGE:
			Popup_window_Pause_Purge(mode);
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Is Purging, please wait!"));			
			break;
		case PAUSE_MESSAGE_OPTION:
			Popup_window_Pause_Option(mode);
			pause_menu_response = PAUSE_RESPONSE_WAIT_FOR;
			disable_e_steppers();
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Purge more?"));			
			break;
		case PAUSE_MESSAGE_RESUME:
			Popup_window_Pause_Resume(mode);			
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Is resuming, Please wait!"));
			break;			
		case PAUSE_MESSAGE_HEATING:
			Popup_window_Pause_Heating(mode);
			thermalManager.setTargetBed(tempbed);
			thermalManager.setTargetHotend(temphot,0);
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Is heating, please wait!"));
			break;
		case PAUSE_MESSAGE_HEAT://10
			Popup_window_Pause_startHeat(mode);			
			DWIN_Show_Status_Message(COLOR_RED, PSTR("Click to heat!"));
			break;
		case PAUSE_MESSAGE_STATUS:
			Clear_Dwin_Area(AREA_BOTTOM);
			break;
			
		case PAUSE_MESSAGE_RETURN:			
			if(IS_SD_PRINTING() || IS_SD_PAUSED()){
				DWIN_status = ID_SM_PRINTING;
				Draw_Printing_Menu();
			}
			else
				Draw_Main_Menu(true);			
			break;
		default:break;
	}
	dwinLCD.UpdateLCD();
}

void DWIN_Pause_Show_Message(const PauseMessage message,const PauseMode mode /*=PAUSE_MODE_SAME*/, const uint8_t extruder/*=0*/ ){	
	UNUSED(extruder);
	DRAW_Pause_Message(message,mode);
}

#if ENABLED(FILAMENT_RUNOUT_SENSOR)
inline void Popup_window_ask_Disable_Runout() {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	Draw_Popup_Bkgd_60();
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 18) / 2, 140, PSTR("Sensor is still on"));
	DWIN_Draw_String_FIL((DWIN_WIDTH - FIL_FONT_W * 16) / 2, 180, PSTR("Disable Runout?"));
	ICON_YESorNO(DwinMenu_RunoutOption.now);
}

/* Filament Runout Option window */
void HMI_Filament_Runout_Option() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	if (encoder_diffState == ENCODER_DIFF_CW) {
		if (DwinMenu_RunoutOption.inc(2)) ICON_YESorNO(DwinMenu_RunoutOption.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_RunoutOption.dec()) ICON_YESorNO(DwinMenu_RunoutOption.now);
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		if(HMI_flag.IS_cancel_runout){
			switch (DwinMenu_RunoutOption.now) {
				case 0: // say yes		
						if(runout.enabled && runout.filament_ran_out){
							runout.enabled = false;	
							runout.filament_ran_out = false;	
						}
				break;
				
				case 1: // say no																	
					break;
				default: break;
			}
			HMI_flag.IS_cancel_runout = false;
			pause_menu_response = PAUSE_RESPONSE_RESUME_PRINT;
			Draw_Printing_Menu();
		}
		else{
			switch (DwinMenu_RunoutOption.now) {
				case 0: // say yes					
						pause_menu_response = PAUSE_RESPONSE_EXTRUDE_MORE;
				break;
				
				case 1: // say no			
					if(runout.enabled && runout.filament_ran_out){
						HMI_flag.IS_cancel_runout = true;
						Popup_window_ask_Disable_Runout();
					}
					else{					
						pause_menu_response = PAUSE_RESPONSE_RESUME_PRINT;
						Draw_Printing_Menu();
					}
				break;
				default: break;
			}
		}		
	}
	dwinLCD.UpdateLCD();
}

void HMI_Filament_Runout_Confirm() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;
	if (encoder_diffState == ENCODER_DIFF_CW) return;
	if (encoder_diffState == ENCODER_DIFF_CCW) return;
	if (encoder_diffState == ENCODER_DIFF_ENTER) {
		if(DwinMenuID == DWMENU_POP_FROD_HEAT){
			thermalManager.setTargetBed(tempbed);
			thermalManager.setTargetHotend(temphot,0);
		}
		DwinMenuID = DWMENU_PRINTING;
		Draw_Printing_Menu(true);
	}	
}
#endif

void HMI_Waiting() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;
	if (encoder_diffState == ENCODER_DIFF_CW) return;
	if (encoder_diffState == ENCODER_DIFF_CCW) return;
	if (encoder_diffState == ENCODER_DIFF_ENTER) {
		DWIN_Kill_Waiting();
	}	
}

void HMI_Printing() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;	
	// Avoid flicker by updating only the previous menu
	if((encoder_diffState == ENCODER_DIFF_CW && DwinMenu_print.inc(PRINT_CASE_END))\
		|| (encoder_diffState == ENCODER_DIFF_CCW && DwinMenu_print.dec())){		
		Draw_ICON_Printingstatus();
		DWIN_FEEDBACK_TICK();
	}	
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		if(DWIN_status == ID_SM_RESUMING || DWIN_status == ID_SM_PAUSING){
			DWIN_Kill_Waiting();			
		}
		else{			
			switch (DwinMenu_print.now) {
				case PRINT_CASE_TUNE: // Tune					
			#if BOTH(BABYSTEPPING, DOUBLECLICK_FOR_Z_BABYSTEPPING)
					Babystep_firstclicktime = millis();
			#endif
					HMI_flag.show_mode = SHOWED_TUNE;
					Draw_Tune_Menu();
				break;

				case PRINT_CASE_PAUSE: // Pause
					if(DWIN_status == ID_SM_PAUSED){
						//resume from Pause			
						//ICON_Pause();
						if(printingIsPaused()){
							DWIN_resume_print();
							DWIN_status = ID_SM_RESUMING;
						}
					}
					else if(DWIN_status == ID_SM_PRINTING){
						DwinMenuID = DWMENU_POP_PAUSEORSTOP;
						Popup_window_PauseOrStop();
					}					
				break;

				case PRINT_CASE_STOP: // Stop
					if(IS_SD_PRINTING() || IS_SD_PAUSED()){
						DwinMenuID = DWMENU_POP_PAUSEORSTOP;
						Popup_window_PauseOrStop();
					}
					else{
						DWIN_status = ID_SM_IDLE;
						Draw_Main_Menu();
					}
				break;

				default: break;
			}
		}
	}
	dwinLCD.UpdateLCD();
}

void HMI_Stop_SDPrinting(){
	ENCODER_DiffState encoder_diffState = get_encoder_state();	
#if 0//BOTH(BABYSTEPPING, DOUBLECLICK_FOR_Z_BABYSTEPPING)	
	if(encoder_diffState == ENCODER_DIFF_CW || encoder_diffState == ENCODER_DIFF_CCW ){
		Babystep_firstclicktime = 0;
	}
	else
#endif
	if(encoder_diffState == ENCODER_DIFF_ENTER){		
#if 0//BOTH(BABYSTEPPING, DOUBLECLICK_FOR_Z_BABYSTEPPING)
		if((ENABLED(BABYSTEP_WITHOUT_HOMING) || all_axes_known()) && (ENABLED(BABYSTEP_ALWAYS_AVAILABLE) || printer_busy())){
			if(millis() < Babystep_firstclicktime + (DOUBLECLICK_MAX_INTERVAL/2) && !axes_should_home()) {
				DwinMenuID = DWMENU_TUNE_BABYSTEPS; 		
				Draw_Babystep_Menu();
				return;
			}
		}
#endif		
		//Stop_and_return_mainmenu();		
		DWIN_status = ID_SM_STOPED;
	}
}

#endif

