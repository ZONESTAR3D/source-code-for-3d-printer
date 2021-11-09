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
 * repeat_printing.h - repeat printing control
 */

#include "../core/millis_t.h"

#define	RPL_MIN_ENDSTOP_INVERTING		true
#define	RPR_MIN_ENDSTOP_INVERTING		true

// @repeat printing control
#define REPEAT_PRINTING_TIMES				10			//
#define REPEAT_PRINTING_PUSH_LENGHT	360			//
#define REPEAT_PRINTING_ARM_LENGHT	400
#define REPEAT_PRINTING_MAX_TIMES		9999		//

#define	RPM_REPEATPRINTING_MOTOR		50			//the rotating speed of repeat printing motor (rotate/minute)
#define	MM_TO_MS(A) (A * 1000 * 60 / (RPM_REPEATPRINTING_MOTOR * 8))

#define	ARM_L			0x01
#define	ARM_R			0x02
#define	ARM_ALL		0x03

typedef enum {
	REPRINT_ARM_INIT = 0,
	REPRINT_ARM_IDLE,
	REPRINT_ARM_HOMING,
	REPRINT_ARM_HOMING_BUMP,
	REPRINT_ARM_PUSHING,
}RePrint_Armstate_t;

class RePrint {
  public:
		static bool enabled;
		 
		static int16_t Reprint_times;
		static uint8_t endstops_state:
		static int16_t Forward_lenght;
		static millis_t reprt_timer;		
		//hot bed temperature for repeat printing, if hot bed temperature must lower than this temperature 
		//before remove the prints
		static float bedtemp_threshold;
				
		static void RepeatPrint_HomeArm() 
		static void RepeatPrint_PushArm();
		static void Init();
		static bool RepeatPrint_ArmControl();
		
  private:		
		static void RepeatPrint_Arm_Init();
		static void RepeatPrint_Arm_Push(const uint8_t lor);
		static void RepeatPrint_Arm_Back(const uint8_t lor); 
		static void RepeatPrint_Arm_Stop(const uint8_t lor);			
};

extern RePrint ReprintManager;

