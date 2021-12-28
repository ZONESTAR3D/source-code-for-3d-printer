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
 * M180: Home Repeat_Print Arm
 * 
 */
void GcodeSuite::M180() {
	if(ReprintManager.enabled)	ReprintManager.RepeatPrint_HomeArm(false);
}

/**
 * M181: Push or Pull Repeat printing Arm 
 * S[int] - move the Repeat printing arm to absolute position
 * Example: M181 S200
 */
void GcodeSuite::M181() {
	if(ReprintManager.enabled){
		if(parser.seen('S'))	
			ReprintManager.RepeatPrint_MoveArm(parser.value_int(), false);
		else
			ReprintManager.RepeatPrint_MoveArm(ReprintManager.Push_length, false);
	}
}

/**
 * M182: wait hot bed cool down
 * S[int] -  hotbed temperature
 * Example: M182 S25
 */
void GcodeSuite::M182() {
	if(ReprintManager.enabled){
		if(parser.seen('S')){	
			ReprintManager.Bedtemp = parser.value_int();
			ReprintManager.RepeatPrinting_wait_bedCool();
		}
	}
} 

/**
 * M183: Start repeat printing current or a specified gcode file
 * 
 * Example: M183 							- print current file
 * Example: M183 12345678.gco - print file "12345678.gco"
 */
void GcodeSuite::M183() {	
	if(ReprintManager.enabled){	
		if(parser.string_arg){
			for (char *fn = parser.string_arg; *fn; ++fn) if(*fn == ' ') *fn = '\0';
			if(strlen(parser.string_arg) > 4)
		  	strcpy(rePrint_filename, parser.string_arg);
		}
		ReprintManager.is_RepeatPrintOnce = true;
	}
}

/**
 * M184: Start print from Z heighth
 * 
 */
 void GcodeSuite::M184() {	
	if(ReprintManager.enabled){	
		if(parser.seen('S')){
			ReprintManager.RePrintPassZ = parser.value_float();
			return;
		}
		ReprintManager.RePrintPassZ = 0.0;
	}
}
#endif // OPTION_REPEAT_PRINTING