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

#include "../../inc/MarlinConfig.h"

#if ENABLED(SDSUPPORT)

#include "../gcode.h"
#include "../../sd/cardreader.h"

#if HAS_DWIN_LCD
  #include "../../lcd/dwin/dwin_ui/dwin.h"
#endif

/**
 * M524: Abort the current SD print job (started with M24)
 */
void GcodeSuite::M524() {
  if (IS_SD_PRINTING()){
		TERN_(HAS_DWIN_LCD, DWIN_StopPrintFromSDCard());
    card.flag.abort_sd_printing = true;
  }
  else if (card.isMounted())
    card.closefile();

}

#endif // SDSUPPORT
