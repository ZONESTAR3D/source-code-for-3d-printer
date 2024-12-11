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
 * DwinMenu_Infor.cpp
 */

 #include "../../../inc/MarlinConfig.h"

#if HAS_DWIN_LCD
#include "dwin.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Info menu
//
static void Item_Info_Model(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Model: " CUSTOM_MACHINE_NAME));	
	Draw_Menu_Line(row);
}

static void Item_Info_Version(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Firmware Version: " FIRMWARE_VERSION));
	Draw_Menu_Line(row);
}

static void Item_Info_Date(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Date: " STRING_DISTRIBUTION_DATE));
	Draw_Menu_Line(row);
}

static void Item_Info_Website(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Website: " WEBSITE_URL));
	Draw_Menu_Line(row);
}



#if ENABLED(OPTION_GUIDE_QRCODE)
static void Item_Info_Guide(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("View User Guide"));	
	Draw_Menu_Line(row);
	Draw_More_Icon(row);
}
#endif

#if ENABLED(OPTION_NEWS_QRCODE)
static void Item_Info_News(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("View Update News"));	
	Draw_Menu_Line(row);
	Draw_More_Icon(row);
}
#endif

#if ENABLED(OPTION_FAQ_QRCODE)
static void Item_Info_FAQ(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("View Troubleshooting"));	
	Draw_Menu_Line(row);
	Draw_More_Icon(row);
}
#endif

static void Item_Info_Board(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Board: " BOARD_INFO_NAME));
	Draw_Menu_Line(row);
}

static void Item_Info_Firmware(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Base on: " SHORT_BUILD_VERSION));
	Draw_Menu_Line(row);
}


static void Item_Info_Extruder_Num(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Extruder Number: " STRINGIFY(E_STEPPERS)));
	Draw_Menu_Line(row);
}

static void Item_Info_Extruder_Model(const uint8_t row) {	
#if ENABLED(OPTION_BMG)
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Extruder: BMG R All"));	
#elif ENABLED(OPTION_BMG_LR)
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Extruder: BMG L + R"));	
#else
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Extruder: Titan"));
#endif
	Draw_Menu_Line(row);
}


#if ENABLED(OPTION_DUALZ_DRIVE)
static void Item_Info_DualZ_Drive(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Z Drives: Dual"));
	Draw_Menu_Line(row);
}
#endif

#if ENABLED(OPTION_Z2_ENDSTOP)
static void Item_Info_DualZ_Endstop(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Z Endstops: Dual"));
	Draw_Menu_Line(row);
}
#endif

static void Item_Info_Baudrate(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Baudrate: " STRINGIFY(BAUDRATE)));
	Draw_Menu_Line(row);
}

#if 0
static void Item_Info_Protocol(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Protocol: " PROTOCOL_VERSION));
	Draw_Menu_Line(row);
}
#endif

static void Item_Info_LevelSensor(const uint8_t row) {	
#if ENABLED(OPTION_3DTOUCH)	
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Level Sensor: 3D Touch"));	
#elif ENABLED(OPTION_ZLSENSOR)
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Level Sensor: ZL-Sensor"));
#else
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Level Sensor: PL-08N"));
#endif
	Draw_Menu_Line(row);
}

static void Item_Info_Thermistor(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Thermistor: NTC 100K B3960"));
	Draw_Menu_Line(row);
}

static void Item_Info_Bed(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("HotBed: MIN:" STRINGIFY(BED_MINTEMP) " MAX:" STRINGIFY(BED_MAXTEMP)));
	Draw_Menu_Line(row);
}

static void Item_Info_Hotend(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("HotEnd: MIN:" STRINGIFY(HEATER_0_MINTEMP) " MAX:" STRINGIFY(HEATER_0_MAXTEMP)));
	Draw_Menu_Line(row);
}

#if ENABLED(OPTION_REPEAT_PRINTING)
static void Item_Info_RepeatPrint(const uint8_t row) {
	DWIN_Draw_UnMaskString_Default(LBLX_INFO, MBASE(row), PSTR("Repeat Print: Support"));
	Draw_Menu_Line(row);
}
#endif

#define	STRING_PRODUCT_MODEL		"Model: " CUSTOM_MACHINE_NAME
#if ENABLED(OPTION_GUIDE_QRCODE)
void Popup_Window_UserGuideLink() {
	Clear_Dwin_Area(AREA_MENU|AREA_BOTTOM);
	Draw_Popup_Bkgd_60();
	DwinMenuID = DWMENU_POP_USERGUIDELINK;
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen("Scan this QR code")+1)*10)/2,75 , PSTR("Scan this QR code"));
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen("to view user guide")+1)*10)/2,95 , PSTR("to view user guide"));
	dwinLCD.Draw_QRCode((272-46*3)/2, 130, 3, PSTR(STRING_GUIDE_LINK));		
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen(STRING_PRODUCT_MODEL)+1)*10)/2, 130+46*3+15, PSTR(STRING_PRODUCT_MODEL));
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen("Press the knob to exit")+1)*10)/2, 130+46*3+35, PSTR("Press the knob to exit"));
}	
#endif

#if ENABLED(OPTION_NEWS_QRCODE)
void Popup_Window_NewsLink() {
	Clear_Dwin_Area(AREA_MENU|AREA_BOTTOM);
	Draw_Popup_Bkgd_60();
	DwinMenuID = DWMENU_POP_NEWSLINK;
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen("Scan this QR code")+1)*10)/2,75 , PSTR("Scan this QR code"));
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen("to view update news")+1)*10)/2,95 , PSTR("to view update news"));
	dwinLCD.Draw_QRCode((272-46*3)/2, 130, 3, PSTR(STRING_NEWS_LINK));		
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen(STRING_PRODUCT_MODEL)+1)*10)/2, 130+46*3+15, PSTR(STRING_PRODUCT_MODEL));
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen("Press the knob to exit")+1)*10)/2, 130+46*3+35, PSTR("Press the knob to exit"));
}	
#endif

#if ENABLED(OPTION_FAQ_QRCODE)
void Popup_Window_FAQLink() {
	Clear_Dwin_Area(AREA_MENU|AREA_BOTTOM);
	Draw_Popup_Bkgd_60();
	DwinMenuID = DWMENU_POP_FAQLINK;
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen("Scan this QR code to")+1)*10)/2,75 , PSTR("Scan this QR code to"));
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen("view troubleshooting")+1)*10)/2,95 , PSTR("view troubleshooting"));
	dwinLCD.Draw_QRCode((272-46*3)/2, 130, 3, PSTR(STRING_FAQ_LINK));		
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen("support@zonestar3d.com")+1)*10)/2, 130+46*3+15, PSTR("support@zonestar3d.com"));
	DWIN_Draw_MaskString_Default_PopMenu((272-(strlen("Press the knob to exit")+1)*10)/2, 130+46*3+35, PSTR("Press the knob to exit"));
}	
#endif

void Draw_Info_Menu() {
	DwinMenuID = DWMENU_INFO;
	DwinMenu_infor.reset();
	
#if(INFO_CASE_TOTAL > MROWS)
	const int8_t iscroll = MROWS - DwinMenu_infor.index; // Scrolled-up lines
	#define ICSCROL(L) (iscroll + (L))
#else
	#define ICSCROL(L) (L)
#endif
	#define ICLINE(L) MBASE(ICSCROL(L))
	#define ICVISI(L) WITHIN(ICSCROL(L), 0, MROWS)

	Clear_Dwin_Area(AREA_TITEL|AREA_MENU|AREA_STATUS|AREA_BOTTOM);
	
	dwinLCD.JPG_CacheTo1(get_title_picID());
	DWIN_Show_MultiLanguage_String(MTSTRING_TITLE_INFO, TITLE_X, TITLE_Y);
	dwinLCD.JPG_CacheTo1(HMI_flag.language+1);
	if (ICVISI(INFO_CASE_BACK)) Draw_Back_First(DwinMenu_infor.now == INFO_CASE_BACK);	
	
	if(ICVISI(INFO_CASE_MODEL)) Item_Info_Model(ICSCROL(INFO_CASE_MODEL));
	if(ICVISI(INFO_CASE_VERSION)) Item_Info_Version(ICSCROL(INFO_CASE_VERSION));
	
#if ENABLED(OPTION_GUIDE_QRCODE)
	if(ICVISI(INFO_CASE_VIEWGUIDE)) Item_Info_Guide(ICSCROL(INFO_CASE_VIEWGUIDE));
#endif
#if ENABLED(OPTION_NEWS_QRCODE)
	if(ICVISI(INFO_CASE_NEWS)) Item_Info_News(ICSCROL(INFO_CASE_NEWS));
#endif
#if ENABLED(OPTION_FAQ_QRCODE)	
	if(ICVISI(INFO_CASE_FAQ)) Item_Info_FAQ(ICSCROL(INFO_CASE_FAQ));	
#endif
	
	if(ICVISI(INFO_CASE_DATE)) Item_Info_Date(ICSCROL(INFO_CASE_DATE));	
	if(ICVISI(INFO_CASE_WEBSITE)) Item_Info_Website(ICSCROL(INFO_CASE_WEBSITE));	
	if(ICVISI(INFO_CASE_FIRMWARE)) Item_Info_Firmware(ICSCROL(INFO_CASE_FIRMWARE));
	if(ICVISI(INFO_CASE_BOARD)) Item_Info_Board(ICSCROL(INFO_CASE_BOARD));

/*	
	if(ICVISI(INFO_CASE_EXTRUDER_NUM)) Item_Info_Extruder_Num(ICSCROL(INFO_CASE_EXTRUDER_NUM));	
	if(ICVISI(INFO_CASE_EXTRUDER_MODEL)) Item_Info_Extruder_Model(ICSCROL(INFO_CASE_EXTRUDER_MODEL));	
#if ENABLED(OPTION_DUALZ_DRIVE)
	if(ICVISI(INFO_CASE_DUALZ_DRIVE)) Item_Info_DualZ_Drive(ICSCROL(INFO_CASE_DUALZ_DRIVE));	
#endif	
#if ENABLED(OPTION_Z2_ENDSTOP)
	if(ICVISI(INFO_CASE_DUALZ_ENDSTOP)) Item_Info_DualZ_Endstop(ICSCROL(INFO_CASE_DUALZ_ENDSTOP));	
#endif	
	if(ICVISI(INFO_CASE_BAUDRATE)) Item_Info_Baudrate(ICSCROL(INFO_CASE_BAUDRATE));	
	//if(ICVISI(INFO_CASE_PROTOCOL)) Item_Info_Protocol(ICSCROL(INFO_CASE_PROTOCOL));	
	if(ICVISI(INFO_CASE_LEVELSENSOR)) Item_Info_LevelSensor(ICSCROL(INFO_CASE_LEVELSENSOR));	
	if(ICVISI(INFO_CASE_THERMISTOR)) Item_Info_Thermistor(ICSCROL(INFO_CASE_THERMISTOR));		
	if(ICVISI(INFO_CASE_BED)) Item_Info_Bed(ICSCROL(INFO_CASE_BED)); 	
	if(ICVISI(INFO_CASE_HOTEND)) Item_Info_Hotend(ICSCROL(INFO_CASE_HOTEND)); 
#if ENABLED(OPTION_REPEAT_PRINTING)
	if(ICVISI(INFO_CASE_REPRINT)) Item_Info_RepeatPrint(ICSCROL(INFO_CASE_REPRINT));
#endif
*/
	if (DwinMenu_infor.now)	Draw_Menu_Cursor(ICSCROL(DwinMenu_infor.now));	
	Draw_Status_Area();
}

#if ENABLED(OPTION_GUIDE_QRCODE)
void HMI_Pop_UserGuideLink() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();	
	if(encoder_diffState == ENCODER_DIFF_NO) return;
	else if(encoder_diffState == ENCODER_DIFF_ENTER){
		if(HMI_flag.first_power_on){
			HMI_flag.first_power_on = false;
			settings.save();
		#if ENABLED(OPTION_ZOFFSETWIZARD)
			DWIN_PopMenu_ZOffsetWizard();
		#else
			Draw_Main_Menu(true);
		#endif
		}
		else Draw_Info_Menu();
	}
}
#endif

#if ENABLED(OPTION_NEWS_QRCODE)
void HMI_Pop_NewsLink() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze();	
	if(encoder_diffState == ENCODER_DIFF_NO) return;
	else if(encoder_diffState == ENCODER_DIFF_ENTER){
		Draw_Info_Menu();//Draw_Main_Menu(true);
	}
}
#endif

#if ENABLED(OPTION_FAQ_QRCODE)
void HMI_Pop_FAQLink() {
	ENCODER_DiffState encoder_diffState = Encoder_ReceiveAnalyze(); 
	if(encoder_diffState == ENCODER_DIFF_NO) return;
	else if(encoder_diffState == ENCODER_DIFF_ENTER){
		Draw_Info_Menu();//Draw_Main_Menu(true);
	}
}
#endif



/* Info */
void HMI_Info() {

	ENCODER_DiffState encoder_diffState = get_encoder_state();
	if (encoder_diffState == ENCODER_DIFF_NO) return;
	
	#if ENABLED(DWIN_AUTO_TEST)
	static uint8_t testmode_click_times = 0;
	#endif

	if (encoder_diffState == ENCODER_DIFF_CW) {
		testmode_click_times = 0;
		if (DwinMenu_infor.inc(INFO_CASE_END)) {
			if (DwinMenu_infor.now > MROWS && DwinMenu_infor.now > DwinMenu_infor.index) {
				DwinMenu_infor.index = DwinMenu_infor.now;

				// Scroll up and draw a blank bottom line
				Scroll_Menu(DWIN_SCROLL_UP);
				if(DwinMenu_infor.index == INFO_CASE_MODEL) Item_Info_Model(MROWS);
				else if(DwinMenu_infor.index == INFO_CASE_VERSION) Item_Info_Version(MROWS);
			#if ENABLED(OPTION_GUIDE_QRCODE)
				else if(DwinMenu_infor.index == INFO_CASE_VIEWGUIDE) Item_Info_Guide(MROWS);
			#endif
			#if ENABLED(OPTION_NEWS_QRCODE)
				else if(DwinMenu_infor.index == INFO_CASE_NEWS) Item_Info_News(MROWS);
			#endif
				else if(DwinMenu_infor.index == INFO_CASE_WEBSITE) Item_Info_Website(MROWS);
			#if ENABLED(OPTION_FAQ_QRCODE)
				else if(DwinMenu_infor.index == INFO_CASE_FAQ) Item_Info_FAQ(MROWS);
			#endif				
				else if(DwinMenu_infor.index == INFO_CASE_DATE) Item_Info_Date(MROWS);
				else if(DwinMenu_infor.index == INFO_CASE_FIRMWARE) Item_Info_Firmware(MROWS);
				else if(DwinMenu_infor.index == INFO_CASE_BOARD) Item_Info_Board(MROWS);
				else if(DwinMenu_infor.index == INFO_CASE_EXTRUDER_NUM) Item_Info_Extruder_Num(MROWS);
				else if(DwinMenu_infor.index == INFO_CASE_EXTRUDER_MODEL) Item_Info_Extruder_Model(MROWS);		
			#if ENABLED(OPTION_DUALZ_DRIVE)
				else if(DwinMenu_infor.index == INFO_CASE_DUALZ_DRIVE) Item_Info_DualZ_Drive(MROWS);
			#endif
			#if ENABLED(OPTION_Z2_ENDSTOP)
				else if(DwinMenu_infor.index == INFO_CASE_DUALZ_ENDSTOP) Item_Info_DualZ_Endstop(MROWS);
			#endif
				else if(DwinMenu_infor.index == INFO_CASE_BAUDRATE) Item_Info_Baudrate(MROWS);
				//else if(DwinMenu_infor.index == INFO_CASE_PROTOCOL) Item_Info_Protocol(MROWS);
				else if(DwinMenu_infor.index == INFO_CASE_LEVELSENSOR) Item_Info_LevelSensor(MROWS);				
				else if(DwinMenu_infor.index == INFO_CASE_THERMISTOR) Item_Info_Thermistor(MROWS);
				else if(DwinMenu_infor.index == INFO_CASE_BED) Item_Info_Bed(MROWS);
				else if(DwinMenu_infor.index == INFO_CASE_HOTEND) Item_Info_Hotend(MROWS);
			#if ENABLED(OPTION_REPEAT_PRINTING)
				else if(DwinMenu_infor.index == INFO_CASE_REPRINT) Item_Info_RepeatPrint(MROWS);
			#endif										
			}
			else 
				Move_Highlight(1, DwinMenu_infor.now + MROWS - DwinMenu_infor.index);
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_CCW) {
		testmode_click_times = 0;
		if (DwinMenu_infor.dec()) {
			if (DwinMenu_infor.now < DwinMenu_infor.index - MROWS) {
				DwinMenu_infor.index--;
				Scroll_Menu(DWIN_SCROLL_DOWN);		
				if(DwinMenu_infor.index - MROWS == INFO_CASE_BACK) Draw_Back_First();				
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_BAUDRATE) Item_Info_Baudrate(0);
			#if ENABLED(OPTION_Z2_ENDSTOP)
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_DUALZ_ENDSTOP) Item_Info_DualZ_Endstop(0);
			#endif
			#if ENABLED(OPTION_DUALZ_DRIVE)
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_DUALZ_DRIVE) Item_Info_DualZ_Drive(0);
			#endif
			#if ENABLED(OPTION_REPEAT_PRINTING)
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_REPRINT) Item_Info_RepeatPrint(0);
			#endif		
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_EXTRUDER_MODEL) Item_Info_Extruder_Model(0);
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_EXTRUDER_NUM) Item_Info_Extruder_Num(0);
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_BOARD) Item_Info_Board(0);		
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_FIRMWARE) Item_Info_Firmware(0);
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_WEBSITE) Item_Info_Website(0);
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_DATE) Item_Info_Date(0);				
			#if ENABLED(OPTION_FAQ_QRCODE)
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_FAQ) Item_Info_FAQ(0);	
			#endif
			#if ENABLED(OPTION_NEWS_QRCODE)
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_NEWS) Item_Info_News(0);
			#endif	
			#if ENABLED(OPTION_GUIDE_QRCODE)
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_VIEWGUIDE) Item_Info_Guide(0);
			#endif
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_VERSION) Item_Info_Version(0);
				else if(DwinMenu_infor.index - MROWS == INFO_CASE_MODEL) Item_Info_Model(0);							
			}
			else {
				Move_Highlight(-1, DwinMenu_infor.now + MROWS - DwinMenu_infor.index);
			}
		}
	}
	else if (encoder_diffState == ENCODER_DIFF_ENTER) {
		switch (DwinMenu_infor.now) {
			case 0: // Back
				Draw_Main_Menu(false, MAIN_CASE_INFO);
			break;
			
		#if ENABLED(SDCARD_TEST)
			case INFO_CASE_MODEL:				
				if(++testmode_click_times >= 5){
					testmode_click_times = 0;					
					SDtest.SDCardtest_Start();
				}
			break;
		#endif
		
		#if ENABLED(DWIN_AUTO_TEST)
			case INFO_CASE_DATE:				
				if(++testmode_click_times >= 5){
					testmode_click_times = 0;					
					autotest.Autotest_Start();
				}
			break;
		#endif

		#if ENABLED(OPTION_GUIDE_QRCODE)
			case INFO_CASE_VIEWGUIDE:
				Popup_Window_UserGuideLink();
			break;
		#endif

		#if ENABLED(OPTION_NEWS_QRCODE)
			case INFO_CASE_NEWS:
				Popup_Window_NewsLink();
			break;
		#endif

		#if ENABLED(OPTION_FAQ_QRCODE)
			case INFO_CASE_FAQ:
				Popup_Window_FAQLink();
			break;
		#endif
			
			default:	break;
		}
	}
	dwinLCD.UpdateLCD();
}
#endif

