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

#include "../inc/MarlinConfig.h"

#if ENABLED(MIXING_EXTRUDER)
//#define MIXER_NORMALIZER_DEBUG

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  #include "pause.h"
#endif

#ifndef __AVR__ // 
  // Use 16-bit (or fastest) data for the integer mix factors
  typedef uint_fast16_t mixer_comp_t;
  typedef uint_fast16_t mixer_accu_t;
  #define COLOR_A_MASK 0x8000
  #define COLOR_MASK 0x7FFF
#else
  // Use 8-bit data for the integer mix factors
  // Exactness is sacrificed for speed
  #define MIXER_ACCU_SIGNED
  typedef uint8_t mixer_comp_t;
  typedef int8_t mixer_accu_t;
  #define COLOR_A_MASK 0x80
  #define COLOR_MASK 0x7F
#endif

#if !defined(DEFAULT_MIXING_SWITCH)
#define	DEFAULT_MIXING_SWITCH 	true
#endif

typedef int8_t mixer_perc_t;

#ifndef MIXING_VIRTUAL_TOOLS
  #define MIXING_VIRTUAL_TOOLS 1
#endif

enum MixTool {
    FIRST_USER_VIRTUAL_TOOL = 0
  , LAST_USER_VIRTUAL_TOOL = MIXING_VIRTUAL_TOOLS - 1
  , NR_USER_VIRTUAL_TOOLS
  , MIXER_DIRECT_SET_TOOL = NR_USER_VIRTUAL_TOOLS
  #if HAS_MIXER_SYNC_CHANNEL
  , MIXER_AUTORETRACT_TOOL
  #endif	
  , NR_MIXING_VIRTUAL_TOOLS
};

#define MAX_VTOOLS TERN(HAS_MIXER_SYNC_CHANNEL, 254, 255)
static_assert(NR_MIXING_VIRTUAL_TOOLS <= MAX_VTOOLS, "MIXING_VIRTUAL_TOOLS must be <= " STRINGIFY(MAX_VTOOLS) "!");

#define MIXER_STEPPER_LOOP(VAR) for (uint_fast8_t VAR = 0; VAR < MIXING_STEPPERS; VAR++)
#define MIXER_VTOOL_LOOP(VAR) for (uint_fast8_t VAR = 0; VAR < MIXING_VIRTUAL_TOOLS; VAR++)


#if ENABLED(GRADIENT_MIX)
typedef struct {
  bool enabled;                         			// This gradient is enabled
  mixer_comp_t color[MIXING_STEPPERS];  			// The current gradient color
  float start_z, end_z;                 			// Region for gradient
  int8_t start_vtool, end_vtool;        			// Start and end virtual tools
  mixer_perc_t start_mix[MIXING_STEPPERS],   	// Start and end mixes from those tools
               end_mix[MIXING_STEPPERS];
  TERN_(GRADIENT_VTOOL, int8_t vtool_index); // Use this virtual tool number as index
} gradient_t;
#endif

#if ENABLED(RANDOM_MIX)
typedef struct {
  bool enabled;    
  float start_z, end_z;
  float height;							//Minimum height of changing mixing rate  
  uint8_t extruders;
}randommix_t;
#endif

#if (MIXING_STEPPERS <= 4 )
constexpr uint8_t INIT_MIX_RATE[16][MIXING_STEPPERS] PROGMEM = {
#if (MIXING_STEPPERS == 2)
	{5, 0},
	{0, 5},
	{5, 5},
	{9, 1},
	{17,3},
	{4, 1},
	{3, 1},
	{7, 3},
	{13, 7},
	{6, 4},
	{4, 6},
	{7, 13},
	{3, 7},
	{1, 3},
	{1, 4},
	{1, 9}
#elif (MIXING_STEPPERS == 3)
	#if ENABLED(DEFAULT_MIX_CMY)
	{5,	0, 0},	//Cyan
	{0, 5, 0},	//Magenta
	{0, 0, 5},	//Yellow
	{0,	1, 5},	//
	{5,	1, 0},	//Ocean-blue
	{1,	1, 0},	//Blue
	{1,	5, 0},	//Violet
	{0,	5, 1},	//Red
	{0,	1, 1},	//Orange
	{1,	0, 5},	//Spring-Green
	{1,	0, 1},	//Green
	{5,	0, 1},	//Turquoise
	{2,	1, 1},	//Cyan-Brown
	{1,	2, 1},	//Magenta-Brown
	{1,	1, 2},	//Yellow-Brown
	{1,	1, 1}		//Brown	
	#else			
	{ 5, 0, 0},
	{ 0, 5, 0},
	{ 0, 0, 5},
	{ 1, 1, 1},
	{ 1, 1, 0},
	{ 1, 0, 1},
	{ 0, 1, 1},
	{ 2, 1, 1},
	{ 1, 2, 1},
	{ 1, 1, 2},
	{14, 3, 3},
	{ 3,14, 3},
	{ 3, 3,14},
	{11, 3, 6},
	{11, 6, 3},
	{ 3, 6,11}	
	#endif
#elif (MIXING_STEPPERS == 4)
	#if ENABLED(DEFAULT_MIX_CMY)
	{5,	0, 0, 0},	//White
	{0,	5, 0, 0},	//Cyan
	{0,	0, 5, 0},	//Magenta
	{0,	0, 0, 5},	//Yellow	
	{0,	5, 1, 0},	//Ocean-blue
	{0,	1, 1, 0},	//Blue
	{0,	1, 5, 0},	//Violet
	{0,	0, 5, 1},	//Red
	{0,	0, 1, 1},	//Orange
	{0,	1, 0, 5},	//Spring-Green
	{0,	1, 0, 1},	//Green
	{0,	5, 0, 1},	//Turquoise
	{0,	2, 1, 1},	//Cyan-Brown
	{0,	1, 2, 1},	//Magenta-Brown
	{0,	1, 1, 2},	//Yellow-Brown
	{0,	1, 1, 1}	//Brown	
	#else
	{5,	0, 0, 0},	//White
	{0,	5, 0, 0},	//Red
	{0,	0, 5, 0},	//Green
	{0,	0, 0, 5},	//Blue
	{1,	1, 1, 1},	//
	{5,	5, 0, 0},	//
	{5,	0, 5, 0},	//
	{5,	0, 0, 5},	//
	{0,	5, 5, 0},	//
	{0,	5, 0, 5},	//
	{1,	4, 0, 0},	//
	{1,	0, 4, 0},	//
	{1,	0, 0, 4},	//
	{4,	1, 0, 0},	//
	{4,	0, 1, 0},	//
	{4,	0, 0, 1}
	#endif
#endif
};
#endif


/**
 * @brief Mixer class
 * @details Contains data and behaviors for a Mixing Extruder
 */
class Mixer {
  public:	
	static bool mixing_enabled;
  static mixer_perc_t percentmix[MIXING_STEPPERS];  // Scratch array for the Mix in proportion to 100, also editable from LCD
  static float collector[MIXING_STEPPERS];    			// M163 components
  static int8_t selected_vtool;
  static mixer_comp_t color[NR_MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS];
  
  static void init(); // Populate colors at boot time
  static void reset_vtools(bool force_reset = false);
  static void refresh_collector(const float proportion=1.0, const uint8_t t=selected_vtool, float (&c)[MIXING_STEPPERS]=collector);

  // Used up to Planner level
  FORCE_INLINE static void set_collector(const uint8_t c, const float f) { collector[c] = _MAX(f, 0.0f); }
  FORCE_INLINE static void set_Percentmix(const uint8_t c, const uint8_t d) { percentmix[c] = d; }
  FORCE_INLINE static void reset_collector(const uint8_t t){
  	MIXER_STEPPER_LOOP(i) set_collector(i, i == t ? 1.0 : 0.0);
	}

  static void copy_percentmix_to_collector();
	#if ENABLED(USE_PRECENT_MIXVALUE)
  static void copy_collector_to_percentmix();
	static void normalize_mixer_percent(mixer_perc_t mix[MIXING_STEPPERS]);	
	#endif
  static void init_collector(const uint8_t index);
	
	
  //
  static void normalize(const uint8_t tool_index);
  static void normalize() { normalize(selected_vtool); }

  FORCE_INLINE static uint8_t get_current_vtool() { return selected_vtool; }
	static void update_mix_from_vtool(const uint8_t j = selected_vtool);
  static void T(const uint_fast8_t c);
	
  #ifdef GRADIENT_MIX
  static gradient_t gradient;   
	static void gradientmix_reset();
  static void gradient_control(const float z);
  static void update_mix_from_gradient();
  // Refresh the gradient after a change
  static void refresh_gradient();
  #endif // GRADIENT_MIX
  
  #ifdef RANDOM_MIX
	static randommix_t random_mix;
	static void randommix_reset();
	static void randommix_control(const float z);
  // Refresh the random after a change
  static void refresh_random_mix();
  #endif

	// Used when dealing with blocks
  FORCE_INLINE static void populate_block(mixer_comp_t b_color[MIXING_STEPPERS]) {
    #if ENABLED(GRADIENT_MIX)
    if (gradient.enabled) {
      MIXER_STEPPER_LOOP(i) b_color[i] = gradient.color[i];
      return;
    }
    #endif
    MIXER_STEPPER_LOOP(i) b_color[i] = color[selected_vtool][i];
  }

  FORCE_INLINE static void stepper_setup(mixer_comp_t b_color[MIXING_STEPPERS]) {
    MIXER_STEPPER_LOOP(i) s_color[i] = b_color[i];
  }

  // Used in Stepper
  FORCE_INLINE static uint8_t get_stepper() { return runner; }
  FORCE_INLINE static uint8_t get_next_stepper() {
    MIXER_STEPPER_LOOP(i) {
      if (--runner < 0) runner = MIXING_STEPPERS - 1;
      accu[runner] += s_color[runner];
      if (
        #ifdef MIXER_ACCU_SIGNED
          accu[runner] < 0
        #else
          accu[runner] & COLOR_A_MASK
        #endif
      ) {
        accu[runner] &= COLOR_MASK;
        return runner;
      }
    }
		return 0;
  }

  private:
  // Used up to Planner level
  //static uint_fast8_t selected_vtool;
  //static mixer_comp_t color[NR_MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS];

  // Used in Stepper
  static float mix_prev_z;	
  static int_fast8_t  runner;
  static mixer_comp_t s_color[MIXING_STEPPERS];
  static mixer_accu_t accu[MIXING_STEPPERS];

	static inline void copy_percentmix_to_color(mixer_comp_t (&tcolor)[MIXING_STEPPERS]);
		
	#if ENABLED(GRADIENT_MIX)
	// Update the current mix from the gradient for a given Z
  static void update_gradient_for_z(const float z, const bool force = false);
  static void update_gradient_for_planner_z(const bool force = false);	
	#endif
	
	#if ENABLED(RANDOM_MIX)
	static int8_t backup_vtool;
	// Update the current mix randomly for a given Z	
	static void update_randommix_for_z(const float z, const bool force = false);
	static void update_randommix_for_planner_z(const bool force = false);  
	#endif

};

extern Mixer mixer;

#endif // MIXING_EXTRUDER
