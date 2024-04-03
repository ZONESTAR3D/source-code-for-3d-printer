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
#include "../rotary_encoder.h"
#include "../dwin_lcd.h"
#include "dwin_comm.h"
#include "dwin.h"
#include "../../../feature/pause.h"

#define PAUSE_HEAT

enum{
  PRINT_CASE_TUNE = 0,
  PRINT_CASE_PAUSE,
  PRINT_CASE_STOP,
  PRINT_CASE_END,
};
  
enum{
	TUNE_CASE_BACK = 0,
  TUNE_CASE_SPEED,
#if ENABLED(OPTION_FLOWRATE_MENU)
	TUNE_CASE_FLOWRATE,
#endif
#if HAS_HOTEND
  TUNE_CASE_ETEMP,
#endif
#if HAS_HEATED_BED
  TUNE_CASE_BTEMP,
#endif
#if HAS_FAN
  TUNE_CASE_FAN,
#endif
#if ENABLED(BABYSTEPPING)
  TUNE_CASE_ZOFF,
#endif
	TUNE_CASE_CONFIG,
#if ENABLED(MIXING_EXTRUDER)
  TUNE_CASE_MIXER,
#endif   
  TUNE_CASE_END
};
#define	TUNE_CASE_TOTAL	(2+ENABLED(OPTION_FLOWRATE_MENU)+HAS_HOTEND+HAS_HEATED_BED+HAS_FAN+ENABLED(BABYSTEPPING)+ENABLED(MIXING_EXTRUDER))

#if ENABLED(BABYSTEPPING)
typedef struct {
	float first;
	float current;
	float last;
	float changed;
} baby_step_t;
#endif

#if ENABLED(BABYSTEPPING)
extern baby_step_t babyzoffset;
#endif 

#if ENABLED(MIXING_EXTRUDER)
void DWIN_Refresh_ExtruerFAN_State();
#endif

void Draw_FileList();
void DWIN_start_SDPrint();
void Draw_Printing_Menu(const uint8_t MenuItem = 0, const bool with_update = false);
void Draw_Print_ProgressBar();
void Draw_Print_ElapsedTime();
void Draw_Print_RemainTime();
void DWIN_Draw_PrintDone_Confirm();
void Draw_Print_ProgressMixModel();

void DWIN_Pause_Show_Message(const PauseMessage message,	const PauseMode mode=PAUSE_MODE_SAME, const uint8_t extruder = 0);
void Draw_Tune_Menu(const uint8_t MenuItem = 0);

void Popup_Window_Resume(); 
void HMI_SDCardInit();
void HMI_SelectFile();
void HMI_Pop_BabyZstep();
void HMI_TuneZoffset();
void HMI_TunePrintSpeed();
#if ENABLED(OPTION_FLOWRATE_MENU)
void HMI_TuneFlowrate();
#endif
void DWIN_StopPrintFromSDCard();
void HMI_PauseOrStop();

void HMI_Tune();

#if ENABLED(FILAMENT_RUNOUT_SENSOR)
void HMI_Filament_Runout_Option();
void HMI_Filament_Runout_Confirm();
#endif

void HMI_Waiting();
void HMI_Printing();
void HMI_Stop_SDPrinting();

#endif
