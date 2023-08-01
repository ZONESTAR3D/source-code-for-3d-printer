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
 * SDCard_Test.cpp
 */

#include "../../../inc/MarlinConfig.h"
#if (HAS_DWIN_LCD && ENABLED(SDCARD_TEST))
#include "dwin.h"
#include "autotest.h"

SDCardtest SDtest;

void SDCardtest::Check_Rotary(void){
	ENCODER_DiffState encoder_diffState = get_encoder_state();

	if (encoder_diffState == ENCODER_DIFF_NO) return;	
	else if (encoder_diffState == ENCODER_DIFF_CW || encoder_diffState == ENCODER_DIFF_CCW) DWIN_FEEDBACK_TICK();	
	else if(encoder_diffState == ENCODER_DIFF_ENTER) {
		DWIN_FEEDBACK_TIPS();
		DWIN_status = ID_SM_RETURN_MAIN;
	}	
}

void SDCardtest::SDTest_Loop(void){
	static millis_t test_next_rts_update_ms = 0;
	static uint16_t test_counter = 0;
	static uint16_t test_timer;
	static uint16_t test_seconds = 0;
	static uint16_t cardsize_MB = 128;
	static uint32_t blocknumber = 128;
	static bool test_dir;
	static bool test_loops;

	Check_Rotary();
	const millis_t test_ms = millis();
	if (PENDING(test_ms, test_next_rts_update_ms)) return;
	test_next_rts_update_ms = test_ms + 100;

	if(!IS_SD_INSERTED()){
		if(test_timer++ >= 5) {
			test_timer = 0;
			test_dir = !test_dir;
			if(test_dir){
				dwinLCD.Draw_Rectangle(1, COLOR_BG_RED, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD2)+ROW_GAP);
				DRAW_STRING_FONT10(COLOR_WHITE, COLOR_BG_RED, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("  SD Card Not Find!  "));
			}
			else{
				dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
				DRAW_STRING_FONT10(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("  SD Card Not Find!  "));
				test_seconds++;
				if(test_seconds > 20) {
					DWIN_FEEDBACK_TIPS();
					DWIN_status = ID_SM_RETURN_MAIN;
				}
			}
		}
		test_loops = false;
		return;
	}
	if(!test_loops){
		test_loops = true;
		blocknumber = CardReader::sd2card.cardSize();
		cardsize_MB = blocknumber/2048;
		dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1), DWIN_WIDTH, YPOS(ID_LINE_SD1)+ROW_GAP);
		if(blocknumber == 0){
			DRAW_STRING_FONT10(COLOR_RED, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("SD Size(MB):"));
			DRAW_STRING_FONT10(COLOR_RED, COLOR_BG_BLACK, LSTART+(strlen("SD Size(MB):")+1)*10, YPOS_MSG(ID_LINE_SD1), PSTR("Fail!"));
			dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1)+ROW_GAP, DWIN_WIDTH, YPOS(ID_LINE_KNOB)-1);
			test_loops = false;
			DWIN_FEEDBACK_TIPS();
			DWIN_status = ID_SM_RETURN_MAIN;			
		}
		else if(cardsize_MB < 1024){
		  DRAW_STRING_FONT10(COLOR_GREEN, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("SD Size(MB):"));
			DRAW_INT_RED_FONT10(COLOR_BG_BLACK, 4, LSTART+(strlen("SD Size(MB):")+1)*10, YPOS_MSG(ID_LINE_SD1), cardsize_MB);
			dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1)+ROW_GAP, DWIN_WIDTH, YPOS(ID_LINE_KNOB)-1);
		}
		else{
			DRAW_STRING_FONT10(COLOR_GREEN, COLOR_BG_BLACK, LSTART, YPOS_MSG(ID_LINE_SD1), PSTR("SD Size(GB):"));
			DRAW_INT_RED_FONT10(COLOR_BG_BLACK, 4, LSTART+(strlen("SD Size(GB):")+1)*10, YPOS_MSG(ID_LINE_SD1), cardsize_MB/1024);
			dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, YPOS(ID_LINE_SD1)+ROW_GAP, DWIN_WIDTH, YPOS(ID_LINE_KNOB)-1);
		}
 		test_timer = 0;
		test_seconds = 0;
	}
	//write file
}

inline void SDCardtest::Draw_SDtest_Menu(void){
	//clean screen	
	dwinLCD.Draw_Rectangle(1, COLOR_BG_BLACK, 0, 0, DWIN_WIDTH, DWIN_HEIGHT);
		
	//titel
	dwinLCD.Draw_Rectangle(1, COLOR_BG_BLUE, 0, 0, DWIN_WIDTH, ROW_GAP);
	DRAW_STRING_FONT12(COLOR_WHITE, COLOR_BG_BLUE, XCENTER(-8), (F_GAP/2), PSTR("SD Card Test"));

	//bottom bar
	dwinLCD.Draw_Rectangle(1, COLOR_BG_BLUE, 0, YPOS(ID_LINE_KNOB), DWIN_WIDTH, YPOS(ID_LINE_KNOB)+ROW_GAP);	
	DRAW_STRING_FONT8(COLOR_WHITE, COLOR_BG_BLUE, LSTART+10, YPOS_MSG(ID_LINE_KNOB), PSTR("Click knob to exit!"));
}

void SDCardtest:: SDtest_Start() {	
	DWIN_status = ID_SM_SDCARDTESTING;
	thermalManager.setTargetHotend(0, 0);
	thermalManager.setTargetBed(0);
	thermalManager.set_fan_speed(0, 0);
	Draw_SDtest_Menu();
}
#endif
