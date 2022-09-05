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

#include "../../../inc/MarlinConfig.h"

#if ENABLED(OPTION_REPEAT_PRINTING)

#include "../../gcode.h"
#include "../../../feature/repeat_printing.h"

/**
 * M180: Home Arm 
 * 
 */
void GcodeSuite::M180() {
	ReprintManager.RepeatPrint_HomeArm(false);
}

/**
 * M181: Push or Pull Repeat printing Arm 
 * S[int] - Move arm to this position, Absolute coordinate system
 * Example: M181 S200
 */
void GcodeSuite::M181() {	
	if(parser.seen('S'))	
		ReprintManager.RepeatPrint_MoveArm(parser.value_ushort(), false);
	else
		ReprintManager.RepeatPrint_MoveArm(ReprintManager.Push_length, false);
}

/**
 * M182: set auto Repeat Printing settings
 * S[bool]: enable (= 1) or disable(= 0) auto repeat printing
 * N[int]:  Repeat Printing times (1~999)
 * Z[int]:  Move Z axis to this heigth before pushing the arm 
 * T[int]:  Bed temperature (15~100)
 * L[int]:  Push arm length
 * B[int]:  Base heigth, [## need HAS_REPEATPRINT_BASE]
 * Example: M182 S1 N10 Z30 T25 L350 B1.6
 */
void GcodeSuite::M182() {
	if(ReprintManager.is_repeatPrinting) return;
	if(parser.seen('S')) ReprintManager.enabled = parser.value_bool();
	if(ReprintManager.enabled) {
		if(parser.seen('N')) {
			ReprintManager.RepeatTimes = parser.value_ushort();
			NOMORE(ReprintManager.RepeatTimes, MAX_REPRINT_TIMES);
		}
		if(parser.seen('Z')) {
			ReprintManager.RePrintZHeigth = parser.value_ushort();
			NOLESS(ReprintManager.RePrintZHeigth, MIN_REPRINT_ZHEIGTH);
			NOMORE(ReprintManager.RePrintZHeigth, MAX_REPRINT_ZHEIGTH);
		}
		if(parser.seen('T')) { 
			ReprintManager.Bedtemp = parser.value_ushort();
			NOLESS(ReprintManager.Bedtemp, MIN_REPRINT_BEDTEMP);
			NOMORE(ReprintManager.Bedtemp, MAX_REPRINT_BEDTEMP);
		}
		if(parser.seen('L')) {
			ReprintManager.Push_length = parser.value_ushort();
			NOMORE(ReprintManager.Push_length, MAX_REPRINT_ARM_LENGHT);
		}
		#if HAS_REPEATPRINT_BASE
		if(parser.seen('B')) {
			ReprintManager.RePrintBaseHeigth = parser.value_float();
			NOLESS(ReprintManager.RePrintBaseHeigth, MIN_REPEATPRINT_BASE_HEIGTH);
			NOMORE(ReprintManager.RePrintBaseHeigth, MAX_REPEATPRINT_BASE_HEIGTH);
		}
		#endif
	}
}

/**
 * M183: Set the repeat printing file
 * 
 * Example: M183 12345678.gco - print file "12345678.gco"
 */
void GcodeSuite::M183() {
	if(ReprintManager.is_repeatPrinting) return;
	if(parser.string_arg){
		for (char *fn = parser.string_arg; *fn; ++fn) if(*fn == ' ') *fn = '\0';
		if(strstr(parser.string_arg, ".gco")){ 
			strcpy(rePrint_filename_next, parser.string_arg);			
		}
	}
}
#endif // OPTION_REPEAT_PRINTING