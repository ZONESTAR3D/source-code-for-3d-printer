/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once
// QWG 104F B3950 thermistor
const temp_entry_t temptable_56[] PROGMEM = {
  { OV(  17), 330 },
  { OV(  19), 315 },
  { OV(  20), 300 },
  { OV(  21), 295 },
  { OV(  23), 290 },
  { OV(  25), 285 },
  { OV(  27), 280 },
  { OV(  28), 275 },
  { OV(  31), 270 },
  { OV(  33), 265 },
  { OV(  35), 260 },
  { OV(  38), 255 },
  { OV(  41), 250 },
  { OV(  44), 245 },
  { OV(  48), 240 },  
  { OV(  52), 235 },
  { OV(  56), 230 },  
  { OV(  61), 225 },
  { OV(  66), 220 },  
  { OV(  78), 215 },
  { OV(  92), 210 },
  { OV( 100), 205 },
  { OV( 110), 200 },
  { OV( 120), 195 },
  { OV( 130), 190 },
  { OV( 140), 185 },
  { OV( 148), 180 },
  { OV( 156), 175 },
  { OV( 171), 170 },
  { OV( 187), 165 },
  { OV( 205), 160 },
  { OV( 224), 155 },
  { OV( 268), 150 },
  { OV( 293), 145 },
  { OV( 320), 140 },
  { OV( 348), 135 },
  { OV( 379), 130 },
  { OV( 411), 125 },
  { OV( 445), 120 },
  { OV( 480), 115 },
  { OV( 516), 110 },
  { OV( 553), 105 },
  { OV( 591), 100 },
  { OV( 628),  95 },
  { OV( 665),  90 },
  { OV( 702),  85 },
  { OV( 737),  80 },
  { OV( 770),  75 },
  { OV( 801),  70 },
  { OV( 830),  65 },
  { OV( 857),  60 },
  { OV( 881),  55 },
  { OV( 903),  50 },
  { OV( 922),  45 },
  { OV( 939),  40 },
  { OV( 954),  35 },
  { OV( 966),  30 },
  { OV( 977),  25 },
  { OV( 985),  23 },
  { OV( 993),  20 },
  { OV(1000),  18 },
  { OV(1010),  15 },
  { OV(1014),  12 },
  { OV(1017),   8 },
  { OV(1019),   5 },
  { OV(1021),   0 },
  { OV(1023),  -5 }
};
