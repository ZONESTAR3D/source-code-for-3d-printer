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
 * DwinMenu_RepeatPrint.cpp
 */

 #include "../../../inc/MarlinConfig.h"

#if (HAS_DWIN_LCD && ENABLED(OPTION_REPEAT_PRINTING))
#include "dwin.h"
#include "../../../feature/repeat_printing.h"
#include "dwinmenu_repeatprint.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Control >> Config >> Repeat print
//
static void Item_RepeatPrint_Enabled(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Repeat Printing"));	
#if HAS_ONOFF_ICON	
	Draw_ONOFF_Icon(row, ReprintManager.enabled);
#else
	DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(row), F_STRING_ONOFF(ReprintManager.enabled));
#endif
	Draw_Menu_Line(row,ICON_CURSOR);
}

static void Item_RepeatPrint_Times(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Repeat times:"));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), ReprintManager.RepeatTimes);
	Draw_Menu_Line(row,ICON_CURSOR);
}

static void Item_RepeatPrint_ArmPushLength(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Push length:"));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), ReprintManager.Push_length);
	Draw_Menu_Line(row,ICON_CURSOR);
}

static void Item_RepeatPrint_BedTemp(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Bed Temp:"));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), ReprintManager.Bedtemp);
	Draw_Menu_Line(row,ICON_CURSOR);
}

static void Item_RepeatPrint_ZHeigth(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Z Heigth:"));
	DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(row), ReprintManager.RePrintZHeigth);
	Draw_Menu_Line(row,ICON_CURSOR);
}
#if HAS_REPEATPRINT_BASE
static void Item_RepeatPrint_BaseHeigth(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Base Heigth:"));
	DWIN_Draw_Small_Float21(MENUVALUE_X, MBASE(row), HMI_Value.RePrintBaseHeigth);
	Draw_Menu_Line(row,ICON_CURSOR);
}
#endif

static void Item_RepeatPrint_HomeArm(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Home Arm Test >>"));	
	Draw_Menu_Line(row,ICON_CURSOR);
	//Draw_More_Icon(row);
}

static void Item_RepeatPrint_PushArm(const uint8_t row) {
	DWIN_Draw_MaskString_Default(LBLX, MBASE(row), PSTR("Push Arm Test >>"));	
	Draw_Menu_Line(row,ICON_CURSOR);
	//Draw_More_Icon(row);
}


void Draw_RepeatPrint_Menu() {
	Clear_Dwin_Area(AREA_TITEL|AREA_MENU);
	DwinMenu_reprint.reset();

#if REPRINT_CASE_TOTAL > MROWS
	const int8_t scroll = MROWS - DwinMenu_reprint.index;
	#define RESCROL(L) (scroll + (L))
#else
	#define RESCROL(L) (L)
#endif
	#define REVISI(L) WITHIN(RESCROL(L), 0, MROWS)
	
	//title
	DWIN_Draw_UnMaskString_Default(14, 7, PSTR("RePrint")); 
	if (REVISI(REPRINT_CASE_BACK)) Draw_Back_First(DwinMenu_reprint.now == REPRINT_CASE_BACK);
	
	if (REVISI(REPRINT_CASE_ONOFF)) Item_RepeatPrint_Enabled(RESCROL(REPRINT_CASE_ONOFF));	
	if (REVISI(REPRINT_CASE_TIMES)) Item_RepeatPrint_Times(RESCROL(REPRINT_CASE_TIMES));
	if (REVISI(REPRINT_CASE_LENGTH)) Item_RepeatPrint_ArmPushLength(RESCROL(REPRINT_CASE_LENGTH));
	if (REVISI(REPRINT_CASE_BEDTEMP)) Item_RepeatPrint_BedTemp(RESCROL(REPRINT_CASE_BEDTEMP));
	if (REVISI(REPRINT_CASE_ZHEIGTH)) Item_RepeatPrint_ZHeigth(RESCROL(REPRINT_CASE_ZHEIGTH));
	
	#if HAS_REPEATPRINT_BASE
	if (REVISI(REPRINT_CASE_BASEHEIGTH)) Item_RepeatPrint_BaseHeigth(RESCROL(REPRINT_CASE_BASEHEIGTH));
	#endif
	
	if(!IS_SD_PRINTING() && !IS_SD_PAUSED() && !IS_SD_FILE_OPEN()) {
		if (REVISI(REPRINT_CASE_HOMEARM)) Item_RepeatPrint_HomeArm(RESCROL(REPRINT_CASE_HOMEARM));
		if (REVISI(REPRINT_CASE_PUSHARM)) Item_RepeatPrint_PushArm(RESCROL(REPRINT_CASE_PUSHARM));
	}	
	if (DwinMenu_reprint.now) Draw_Menu_Cursor(RESCROL(DwinMenu_reprint.now));
}

void HMI_Reprint_Times() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &ReprintManager.RepeatTimes)) {
			DwinMenuID = DWMENU_SET_REPRINT;
			EncoderRate.enabled = false;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_TIMES), ReprintManager.RepeatTimes);		
			dwinLCD.UpdateLCD();
			return;
		}
		NOLESS(ReprintManager.RepeatTimes, 0);
		NOMORE(ReprintManager.RepeatTimes, MAX_REPRINT_TIMES);
		DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_TIMES), ReprintManager.RepeatTimes);
		dwinLCD.UpdateLCD();
	}
}

void HMI_RePrint_ArmPushLength() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &ReprintManager.Push_length)) {
			DwinMenuID = DWMENU_SET_REPRINT;
			EncoderRate.enabled = false;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_LENGTH), ReprintManager.Push_length);
			dwinLCD.UpdateLCD();
			return;
		}
		NOLESS(ReprintManager.Push_length, 0);
		NOMORE(ReprintManager.Push_length, MAX_REPRINT_ARM_LENGHT);
		DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_LENGTH), ReprintManager.Push_length);
		dwinLCD.UpdateLCD();
	}
}

void HMI_RePrint_BedTemp() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &ReprintManager.Bedtemp)) {
			DwinMenuID = DWMENU_SET_REPRINT;
			EncoderRate.enabled = false;
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_BEDTEMP), ReprintManager.Bedtemp);			
			dwinLCD.UpdateLCD();			
			return;
		}
		NOLESS(ReprintManager.Bedtemp, MIN_REPRINT_BEDTEMP);
		NOMORE(ReprintManager.Bedtemp, MAX_REPRINT_BEDTEMP);		
		DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_BEDTEMP), ReprintManager.Bedtemp);
		dwinLCD.UpdateLCD();
	}
}

void HMI_RepeatPrint_ZHeigth() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &ReprintManager.RePrintZHeigth)) {
			DwinMenuID = DWMENU_SET_REPRINT;			
			DWIN_Draw_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_ZHEIGTH), ReprintManager.RePrintZHeigth);
			EncoderRate.enabled = false;
			dwinLCD.UpdateLCD();			
			return;
		}
		NOLESS(ReprintManager.RePrintZHeigth, MIN_REPRINT_ZHEIGTH);
		NOMORE(ReprintManager.RePrintZHeigth, MAX_REPRINT_ZHEIGTH);		
		DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_ZHEIGTH), ReprintManager.RePrintZHeigth);
		dwinLCD.UpdateLCD();
	}
}

#if HAS_REPEATPRINT_BASE
void HMI_RepeatPrint_BaseHeigth() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();
	if (encoder_diffState != ENCODER_DIFF_NO) {
		if (Apply_Encoder_int16(encoder_diffState, &HMI_Value.RePrintBaseHeigth)) {
			DwinMenuID = DWMENU_SET_REPRINT;
			ReprintManager.RePrintBaseHeigth = (float)HMI_Value.RePrintBaseHeigth/MINUNITMULT;			
			DWIN_Draw_Small_Float21(MENUVALUE_X, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_BASEHEIGTH), HMI_Value.RePrintBaseHeigth);
			EncoderRate.enabled = false;
			dwinLCD.UpdateLCD();			
			return;
		}
		NOLESS(HMI_Value.RePrintBaseHeigth, MIN_REPEATPRINT_BASE_HEIGTH*MINUNITMULT);
		NOMORE(HMI_Value.RePrintBaseHeigth, MAX_REPEATPRINT_BASE_HEIGTH*MINUNITMULT);		
		DWIN_Draw_Selected_Small_Float21(MENUVALUE_X, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_BASEHEIGTH), HMI_Value.RePrintBaseHeigth);
		dwinLCD.UpdateLCD();
	}
}
#endif

void HMI_RepeatPrint() {
	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;

	 // Avoid flicker by updating only the previous menu
	if (encoder_diffState == ENCODER_DIFF_CW) {
		if(IS_SD_PRINTING() || IS_SD_PAUSED()){
			if(DwinMenu_reprint.inc(REPRINT_TUNE_CASE_END)){					
				if (DwinMenu_reprint.now > MROWS && DwinMenu_reprint.now > DwinMenu_reprint.index) {
					DwinMenu_reprint.index = DwinMenu_reprint.now;
					// Scroll up and draw a blank bottom line
					Scroll_Menu(DWIN_SCROLL_UP);
					//if(DwinMenu_reprint.index == REPRINT_CASE_ONOFF) Item_RepeatPrint_Enabled(MROWS);
					//if(DwinMenu_reprint.index == REPRINT_CASE_TIMES) Item_RepeatPrint_Times(MROWS);
					//if(DwinMenu_reprint.index == REPRINT_CASE_LENGTH) Item_RepeatPrint_ArmPushLength(MROWS);
					//if(DwinMenu_reprint.index == REPRINT_CASE_BEDTEMP) Item_RepeatPrint_BedTemp(MROWS);
					//if(DwinMenu_reprint.index == REPRINT_CASE_ZHEIGTH) Item_RepeatPrint_ZHeigth(MROWS);
				#if HAS_REPEATPRINT_BASE
					if(DwinMenu_reprint.index == REPRINT_CASE_BASEHEIGTH) Item_RepeatPrint_BaseHeigth(MROWS);
				#endif				
				}
				else 
					Move_Highlight(1, DwinMenu_reprint.now + MROWS - DwinMenu_reprint.index);
			}
		}
		else if(DwinMenu_reprint.inc(REPRINT_CASE_END)){
			if (DwinMenu_reprint.now > MROWS && DwinMenu_reprint.now > DwinMenu_reprint.index) {				
				// Scroll up and draw a blank bottom line
				DwinMenu_reprint.index = DwinMenu_reprint.now;
				Scroll_Menu(DWIN_SCROLL_UP);
				//if(DwinMenu_reprint.index == REPRINT_CASE_ONOFF) Item_RepeatPrint_Enabled(MROWS);
				//if(DwinMenu_reprint.index == REPRINT_CASE_TIMES) Item_RepeatPrint_Times(MROWS);
				//if(DwinMenu_reprint.index == REPRINT_CASE_LENGTH) Item_RepeatPrint_ArmPushLength(MROWS);
				//if(DwinMenu_reprint.index == REPRINT_CASE_BEDTEMP) Item_RepeatPrint_BedTemp(MROWS);
				//if(DwinMenu_reprint.index == REPRINT_CASE_ZHEIGTH) Item_RepeatPrint_ZHeigth(MROWS);
			#if HAS_REPEATPRINT_BASE
				if(DwinMenu_reprint.index == REPRINT_CASE_BASEHEIGTH) Item_RepeatPrint_BaseHeigth(MROWS);
			#endif
				if(DwinMenu_reprint.index == REPRINT_CASE_HOMEARM) Item_RepeatPrint_HomeArm(MROWS);
				if(DwinMenu_reprint.index == REPRINT_CASE_PUSHARM) Item_RepeatPrint_PushArm(MROWS);
			}
			else {
				Move_Highlight(1, DwinMenu_reprint.now + MROWS - DwinMenu_reprint.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		if (DwinMenu_reprint.dec()) {
			if (DwinMenu_reprint.now < DwinMenu_fwretract.index - MROWS) {
				DwinMenu_reprint.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);
				if(DwinMenu_reprint.index - MROWS == REPRINT_CASE_BACK) Draw_Back_First();
				else if(DwinMenu_reprint.index - MROWS == REPRINT_CASE_ONOFF) Item_RepeatPrint_Enabled(0);
				else if(DwinMenu_reprint.index - MROWS == REPRINT_CASE_TIMES) Item_RepeatPrint_Times(0);
				else if(DwinMenu_reprint.index - MROWS == REPRINT_CASE_LENGTH) Item_RepeatPrint_ArmPushLength(0);
				//if(DwinMenu_reprint.index - MROWS == REPRINT_CASE_BEDTEMP) Item_RepeatPrint_BedTemp(0);
				//if(DwinMenu_reprint.index - MROWS == REPRINT_CASE_ZHEIGTH) Item_RepeatPrint_ZHeigth(0);
			#if HAS_REPEATPRINT_BASE
				//if(DwinMenu_reprint.index - MROWS == REPRINT_CASE_BASEHEIGTH) Item_RepeatPrint_BaseHeigth(0);
			#endif
				//if(DwinMenu_reprint.index - MROWS == REPRINT_CASE_HOMEARM) Item_RepeatPrint_HomeArm(0);
				//if(DwinMenu_reprint.index - MROWS == REPRINT_CASE_PUSHARM) Item_RepeatPrint_PushArm(0);
			}
			else {
				Move_Highlight(-1, DwinMenu_reprint.now + MROWS - DwinMenu_reprint.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_reprint.now) {
			case 0: 									// Back				
				Draw_Config_Menu(CONFIG_CASE_REPRINT);				
			break;

			case REPRINT_CASE_ONOFF:  			// times
				DwinMenuID = DWMENU_SET_REPRINT;
				ReprintManager.enabled = !ReprintManager.enabled;
			#if HAS_ONOFF_ICON					
				Draw_ONOFF_Icon(MROWS -DwinMenu_reprint.index + REPRINT_CASE_ONOFF, ReprintManager.enabled);
			#else
				DWIN_Draw_MaskString_Default(MENUONOFF_X, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_ONOFF), F_STRING_ONOFF(ReprintManager.enabled));
			#endif
			break;
	 
			case REPRINT_CASE_TIMES:  			// times
				DwinMenuID = DWMENU_SET_REPRINT_TIMES;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_TIMES), ReprintManager.RepeatTimes);
				EncoderRate.enabled = true;
			break;

			case REPRINT_CASE_LENGTH:  			// Push Length
				DwinMenuID = DWMENU_SET_REPRINT_PUSHLENGTH;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_LENGTH), ReprintManager.Push_length);
				EncoderRate.enabled = true;
			break;

			case REPRINT_CASE_BEDTEMP:  		// Bed Temp
				DwinMenuID = DWMENU_SET_REPRINT_BEDTEMP;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_BEDTEMP), ReprintManager.Bedtemp);
				EncoderRate.enabled = true;
			break;

			case REPRINT_CASE_ZHEIGTH:  			// Z Heigth
				DwinMenuID = DWMENU_SET_REPRINT_ZHEIGTH;
				DWIN_Draw_Select_IntValue_Default(3, MENUVALUE_X+8, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_ZHEIGTH), ReprintManager.RePrintZHeigth);
				EncoderRate.enabled = true;
			break;
			
		#if HAS_REPEATPRINT_BASE	
			case REPRINT_CASE_BASEHEIGTH:  			// Z Heigth
				DwinMenuID = DWMENU_SET_REPRINT_BASEHEIGTH;
				HMI_Value.RePrintBaseHeigth = ReprintManager.RePrintBaseHeigth*MINUNITMULT;			
				DWIN_Draw_Selected_Small_Float21(MENUVALUE_X, MBASE(MROWS -DwinMenu_reprint.index + REPRINT_CASE_BASEHEIGTH), HMI_Value.RePrintBaseHeigth);
				EncoderRate.enabled = true;
			break;
		#endif		
		
			case REPRINT_CASE_HOMEARM: 			// HOME Arm
				if(IS_SD_PRINTING() || IS_SD_PAUSED() || IS_SD_FILE_OPEN())	break;
			
				queue.inject_P(PSTR("M180"));
			break;

			case REPRINT_CASE_PUSHARM: 			// Push Arm
				if(IS_SD_PRINTING() || IS_SD_PAUSED() || IS_SD_FILE_OPEN())	break;
				
				queue.inject_P(PSTR("M181"));
			break;
   
   default: break;
  }
 }
 dwinLCD.UpdateLCD();
}

void Popup_Window_RepeatPrint(){
	DWIN_status = ID_SM_REPEATPRITING;
	DwinMenuID = DWMENU_POP_REPEATPRINTING;
	Clear_Dwin_Area(AREA_TITEL|AREA_POPMENU);
	Draw_Popup_Bkgd_60();	//60_390
	DWIN_Draw_UnMaskString_FONT10_TITLE(14 ,4, PSTR("Repeat Print"));
	DWIN_Draw_MaskString_FONT12(POP_TEXT_COLOR, COLOR_BG_WINDOW, (272-12*15)/2, 90, PSTR("Repeat Printing"));
	DWIN_Draw_MaskString_Default_PopMenu( (272-10*20)/2, 120, PSTR("Remain times = "));
	DWIN_Draw_IntValue_PopMenu(3, (272-10*20)/2+16*10, 120, (ReprintManager.RepeatTimes-1));
	DWIN_Show_ICON(ICON_CANCEL_E, 85, 240);
	dwinLCD.Draw_Rectangle(0, COLOR_RED, 85, 240, 186, 279);
	dwinLCD.Draw_Rectangle(0, COLOR_RED, 84, 239, 187, 280);
	dwinLCD.Draw_String(false, true, font10x20, COLOR_RED, COLOR_BG_WINDOW, (272-10*16)/2, 290, PSTR("Click to cancel!"));
  dwinLCD.UpdateLCD();
}

void Updata_RePrint_Popup_Window(uint8_t status){
	if(DwinMenuID != DWMENU_POP_REPEATPRINTING) return;
	
	dwinLCD.Draw_Rectangle(1, COLOR_BG_WINDOW, 14, 140, 256, 220);
	switch(status){
		case REPRINT_COOLDOWN:
			DWIN_Draw_MaskString_Default_PopMenu( (272-10*18)/2, 160, PSTR("Wait bed cool down"));		
			//Targer Temp = xxx
			DWIN_Draw_MaskString_Default_PopMenu( (272-10*19)/2, 190, PSTR("Targer Temp. = "));
			DWIN_Draw_IntValue_PopMenu(3, (272-10*19)/2+15*10, 190, ReprintManager.Bedtemp);
			break;

		case REPRINT_COOLDOWN_WAIT:	
			//Wait more xxx senconds
			DWIN_Draw_MaskString_Default_PopMenu( (272-10*23)/2, 190, PSTR("Wait more "));
			DWIN_Draw_IntValue_PopMenu(3, (272-10*23)/2+10*10, 190, ReprintManager.RePrint_wait_seconds);
			DWIN_Draw_MaskString_Default_PopMenu( (272-10*23)/2+14*10, 190, PSTR("seconds."));
			break;		
			
		case REPRINT_HOMING:
			DWIN_Draw_MaskString_Default_PopMenu( (272-10*17)/2, 190, PSTR("Homing the arm..."));
			break;

		case REPRINT_PUSHING:
			DWIN_Draw_MaskString_Default_PopMenu( (272-10*18)/2, 190, PSTR("Pushing the arm..."));
			break;

		case REPRINT_PRINTNEXTONT:			
			DWIN_Draw_MaskString_Default_PopMenu( (272-10*21)/2, 190, PSTR("Print the next one..."));
			DWIN_status = ID_SM_PRINTING;
			break;

		default:
			break;
	}
	dwinLCD.UpdateLCD();
}

void HMI_Cancel_RePrint(){		
	ENCODER_DiffState encoder_diffState = get_encoder_state();	
	if (encoder_diffState == ENCODER_DIFF_ENTER){
		ReprintManager.RepeatPrinting_Reset();
		DwinMenuID = DWMENU_PRINTING;
		DWIN_status = ID_SM_STOPED;
	}	
}
#endif

