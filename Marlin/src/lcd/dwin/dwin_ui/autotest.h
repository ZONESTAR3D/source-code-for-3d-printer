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

/**
 * DWIN LCD
 */
#include "../../../inc/MarlinConfig.h"
#include "dwin.h"

#if (HAS_DWIN_LCD && ENABLED(DWIN_AUTO_TEST))
#include "../dwin_lcd.h"
#include "../rotary_encoder.h"

#define	USED_FONT							font12x24
#define WIDTH  								12
#define HEIGH  								24
#define F_GAP	 								8
#define LSTART	 							5
#define SWWIDTH	 							45
#define AUTO_FAN_TEMPERATURE	40


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

#define	DRAW_INT_WHITE_FONT12(a,b,x,y,v) dwinLCD.Draw_IntValue(true, true, 0, font12x24, COLOR_WHITE, a, b, x, y, v)
#define	DRAW_INT_RED_FONT12(a,b,x,y,v) dwinLCD.Draw_IntValue(true, true, 0, font12x24, COLOR_RED, a, b, x, y, v)
#define	DRAW_INT_RED_FONT10(a,b,x,y,v) dwinLCD.Draw_IntValue(true, true, 0, font10x20, COLOR_RED, a, b, x, y, v)
#define	DRAW_INT_RED_FONT8(a,b,x,y,v) dwinLCD.Draw_IntValue(true, true, 0, font8x16, COLOR_RED, a, b, x, y, v)
#define	DRAW_INT_GREEN_FONT12(a,b,x,y,v) dwinLCD.Draw_IntValue(true, true, 0, font12x24, COLOR_GREEN, a, b, x, y, v)
#define	DRAW_INT_GREEN_FONT10(a,b,x,y,v) dwinLCD.Draw_IntValue(true, true, 0, font10x20, COLOR_GREEN, a, b, x, y, v)
#define	DRAW_INT_GREEN_FONT8(a,b,x,y,v) dwinLCD.Draw_IntValue(true, true, 0, font8x16, COLOR_GREEN, a, b, x, y, v)
#define	DRAW_STRING_FONT12(color,color_bg,x,y,s) dwinLCD.Draw_String(false, true, font12x24, color, color_bg, x, y, s)
#define	DRAW_STRING_FONT10(color,color_bg,x,y,s) dwinLCD.Draw_String(false, true, font10x20, color, color_bg, x, y, s)
#define	DRAW_STRING_FONT8(color,color_bg,x,y,s) dwinLCD.Draw_String(false, true, font8x16, color, color_bg, x, y, s)

enum {
  CHECK_START = 0,
	CHECK_SD,
	CHECK_PREPARE_HEAT,
	CHECK_HEATER,
	CHECK_FANS_PREPARE,
	CHECK_FANS,
	CHECK_XY_MOTOR,
#if DISABLED(COREXY)
	CHECK_Y_MOTOR,
#endif
	CHECK_Z_MOTOR,
	CHECK_EXTRUDER_MOTOR,
	#if (E_STEPPERS > 1)
	CHECK_MOTOR_E2,
	#endif
	#if (E_STEPPERS > 2)
	CHECK_MOTOR_E3,
	#endif
	#if (E_STEPPERS > 3)
	CHECK_MOTOR_E4,
	#endif
	CHECK_MOTOR_END,
	CHECK_ENDSTOPS_PREPARE,
	CHECK_ENDSTOPS,
	CHECK_FINISHED,
	CHECK_KNOB,	
	CHECK_END,	
};

typedef struct { 
	uint8_t state = 0;
	uint8_t rotary_counter = 0;
	uint8_t click_counter = 0;
	uint8_t Endstops;	
	bool fan_fg:1;
	bool autorun:1;
}_stAutotest_t;

class Autotest {
public:		
		static _stAutotest_t testflag;

		static uint8_t swstatus[6];
		static uint8_t old_swstatus[6];
		
		void Check_Rotary();
		void AutoTest_Loop();		
		void Autotest_Start(uint8_t check_state = CHECK_START, bool auto_loop = true);

private:		
		inline void Draw_Autotest_Menu(void);
		inline void AutoTest_ShowSWStatus(bool bfirst);
		inline void AutoTest_ShowTemperature();
		inline void Autotest_ShowKnob();
		inline void AutoTest_Watch_SW();	
};

extern Autotest autotest;

#endif

