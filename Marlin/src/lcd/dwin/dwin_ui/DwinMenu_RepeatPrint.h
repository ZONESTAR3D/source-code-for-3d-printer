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
#if (HAS_DWIN_LCD && ENABLED(OPTION_REPEAT_PRINTING))
#include "../rotary_encoder.h"
#include "../dwin_lcd.h"
#include "dwin_comm.h"
#include "dwin.h"

enum{
	REPRINT_CASE_BACK,
	REPRINT_CASE_ONOFF,		
	REPRINT_CASE_TIMES,
	REPRINT_CASE_LENGTH,
	REPRINT_CASE_BEDTEMP,	
	REPRINT_CASE_ZHEIGTH,
#if HAS_REPEATPRINT_BASE	
	REPRINT_CASE_BASEHEIGTH,
#endif
	REPRINT_TUNE_CASE_END,
	REPRINT_CASE_HOMEARM = REPRINT_TUNE_CASE_END,
	REPRINT_CASE_PUSHARM,
	REPRINT_CASE_END
};
#define	REPRINT_CASE_TOTAL	(REPRINT_CASE_END-1)

void Draw_RepeatPrint_Menu();
void HMI_Reprint_Times(); 
void HMI_RePrint_ArmPushLength();
void HMI_RePrint_BedTemp();
void HMI_RepeatPrint_ZHeigth();
#if HAS_REPEATPRINT_BASE	
void HMI_RepeatPrint_BaseHeigth();
#endif
void HMI_RepeatPrint();
void Popup_Window_RepeatPrint();
void Updata_RePrint_Popup_Window(uint8_t status);
void HMI_Cancel_RePrint();
#endif
