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
#define	REPRINT_ARM_BUMP_LENGTH				3			//

#define MAX_REPRINT_TIMES						999			//

#define DEFAULT_REPRINT_ARM_LENGHT	(Y_MAX_POS+30)			//
#define MIN_REPRINT_ARM_LENGHT			0										//
#define MAX_REPRINT_ARM_LENGHT			(Y_MAX_POS+50)			//

#define	DEFAULT_REPRINT_ZHEIGTH			30			//
#define	MIN_REPRINT_ZHEIGTH					25			//
#define	MAX_REPRINT_ZHEIGTH	(Z_MAX_POS/2)		//

#define	DEFAULT_REPRINT_BEDTEMP			25			//
#define	MIN_REPRINT_BEDTEMP					15			//
#define	MAX_REPRINT_BEDTEMP					50			//

#define	WAIT_SECONDS_AFTER_BEDCOOL	30			// wait seconds after cool down

#define	RPM_REPEATPRINTING_MOTOR		90			//the rotating speed of repeat printing motor (rotate/minute)
#define	ARM_MM_TO_MS(A) ((A*1000*60)/(RPM_REPEATPRINTING_MOTOR*8))

#define	BIT_ARM_L			0
#define	BIT_ARM_R			1

typedef enum {
	REPRINT_IDLE = 0,
	REPRINT_PREPARE,	
	REPRINT_HOMING,
	REPRINT_PREPARE_YMOVE,
	REPRINT_PREPARE_ZMOVE,
	REPRINT_PREPARE_STARTCOOL,	
	REPRINT_COOLDOWN,
	REPRINT_COOLDOWN_WAIT,	
	REPRINT_PUSHING,
	REPRINT_HOMEAGAIN,
	REPRINT_CHECK_FILEEXIST,
	REPRINT_PRINTNEXTONT
}RePrint_state_t;

class RePrint {
  public:
		static bool enabled;		
		static bool is_AutoRepeatPrinting;
		static bool is_RepeatPrintOnce;
		
		static int16_t RepeatTimes;
		static int16_t Push_length;
		static int16_t Bedtemp;
		static int16_t RePrintZHeigth;
		static int16_t RePrint_wait_seconds;
		static uint8_t RePrint_status;

		static float RePrintPassZ;
				
		static bool RepeatPrint_HomeArm(bool isblocked = true); 		
		static void RepeatPrint_MoveArm(const int16_t pos = DEFAULT_REPRINT_ARM_LENGHT, bool isblocked = true);
		static void getRefernceBedTemp(const float hotend_temp, const float bed_temp);
		static void RepeatPrinting_Reset();
		static void initialize();
		static void RepeatPrinting_wait_bedCool();
		static void RepeatPrinting_process();
		static void Prepare_RepeatPrint();
		
  private:				
		static bool is_ArmHomed;
		static bool is_ArmL_Stopped;
		static bool is_ArmR_Stopped;		
		static bool gotReferenceBedTemp;		
		static int16_t arm_current_pos;
		
		static void RepeatPrint_ArmPort_Init();
		static void RepeatPrint_Arm_Push(const uint8_t lor);
		static void RepeatPrint_Arm_Pull(const uint8_t lor); 
		static void RepeatPrint_Arm_Stop(const uint8_t lor);
		
};

extern RePrint ReprintManager;
extern char rePrint_filename[20];

