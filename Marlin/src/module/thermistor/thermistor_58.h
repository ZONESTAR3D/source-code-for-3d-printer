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
// QWG 104F B3950 thermistor 4.7 kOhm pull-up
const temp_entry_t temptable_58[] PROGMEM = {
#if 1 // correct in 2023-07-11
	{  OV(  17), 325},
  {  OV(  18), 320},
 	{  OV(  19), 315},
  {  OV(  20), 310},
  {  OV(  21), 305},
  {  OV(  22), 300},
	{  OV(  23), 295},
	{  OV(  24), 290},
	{  OV(  26), 285},
	{  OV(  28), 280},
	{  OV(  30), 275},
	{  OV(  33), 270},
	{  OV(  35), 265},
	{  OV(  38), 260},
	{  OV(  41), 255},
	{  OV(  44), 250},
	{  OV(  48), 245},
	{  OV(  52), 240},
	{  OV(  56), 235},
	{  OV(  61), 230},
	{  OV(  66), 225},
	{  OV(  72), 220},
	{  OV(  79), 215},
	{  OV(  86), 210},
	{  OV(  93), 205},
	{  OV( 102), 200},
	{  OV( 111), 195},
	{  OV( 122), 190},
	{  OV( 133), 185},
	{  OV( 146), 180},
	{  OV( 160), 175},
	{  OV( 175), 170},
	{  OV( 192), 165},
	{  OV( 210), 160},
	{  OV( 230), 155},
	{  OV( 251), 150},
	{  OV( 275), 145},
	{  OV( 300), 140},
	{  OV( 327), 135},
	{  OV( 378), 130},
	{  OV( 410), 125},
	{  OV( 444), 120},
	{  OV( 479), 115},
	{  OV( 515), 110},
	{  OV( 552), 105},
	{  OV( 589), 100},
	{  OV( 642),  95},
	{  OV( 686),  90},
	{  OV( 714),  85},
	{  OV( 767),  80},
	{  OV( 798),  75},
	{  OV( 828),  70},
	{  OV( 854),  65},
	{  OV( 888),  60},
	{  OV( 911),  55},
	{  OV( 934),  50},
	{  OV( 950),  45},
	{  OV( 965),  40},
	{  OV( 976),  37},
	{  OV( 994),  33},
	{  OV( 999),  27},
	{  OV(1010),  21},
	{  OV(1015),  15},
	{  OV(1018),  10},
	{  OV(1019),   3},
	{  OV(1020),  -3},
	{  OV(1021), 	-8},
	{  OV(1022), -12}
#endif
};
