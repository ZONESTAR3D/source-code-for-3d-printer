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

#include "../inc/MarlinConfig.h"

#if ENABLED(MIXING_EXTRUDER)

//#define MIXER_NORMALIZER_DEBUG

#include "mixing.h"
#include "../module/motion.h"
#include "../module/planner.h"

Mixer mixer;

#ifdef MIXER_NORMALIZER_DEBUG
  #include "../core/serial.h"
#endif
bool Mixer::mixing_enabled = DEFAULT_MIXING_SWITCH;

// Used up to Planner level
int8_t	Mixer::selected_vtool = 0;
float         Mixer::collector[MIXING_STEPPERS]; // mix proportion. 0.0 = off, otherwise <= COLOR_A_MASK.
mixer_comp_t  Mixer::color[NR_MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS];
mixer_perc_t Mixer::percentmix[MIXING_STEPPERS];

// Used in Stepper
int_fast8_t   Mixer::runner = 0;
mixer_comp_t  Mixer::s_color[MIXING_STEPPERS];
mixer_accu_t  Mixer::accu[MIXING_STEPPERS] = { 0 };

//
float Mixer::mix_prev_z = 0.0;
void Mixer::normalize(const uint8_t tool_index) {
  float cmax = 0;
  float csum = 0;
  MIXER_STEPPER_LOOP(i) {
    const float v = collector[i];
    NOLESS(cmax, v);
    csum += v;
  }
	if(csum < 1.0){
		MIXER_STEPPER_LOOP(i){
			collector[i] = 0.0;
		}
		cmax = collector[0] = 1.0;
	}
  #ifdef MIXER_NORMALIZER_DEBUG
	SERIAL_ECHOLNPGM("normalize");
	SERIAL_EOL();
	SERIAL_ECHOLNPAIR("tool_index=",uint16_t(tool_index));
	SERIAL_ECHOPGM("Mixer: Old relation : [ ");
	MIXER_STEPPER_LOOP(i) {
		SERIAL_ECHO_F(collector[i] / csum, 3);
		SERIAL_ECHOPGM(", ");
  }
  SERIAL_ECHOLNPGM("]");
	SERIAL_EOL();
  #endif

  // Scale all values so their maximum is COLOR_A_MASK  
  const float scale = float(COLOR_A_MASK) / cmax;
  MIXER_STEPPER_LOOP(i) color[tool_index][i] = collector[i] * scale;

#ifdef MIXER_NORMALIZER_DEBUG
  csum = 0;
  SERIAL_ECHOPGM("Mixer: Normalize to : [ ");
  MIXER_STEPPER_LOOP(i) {
    SERIAL_ECHO(uint16_t(color[tool_index][i]));
    SERIAL_ECHOPGM(", ");
    csum += color[tool_index][i];
  }
  SERIAL_ECHOLNPGM("]");
  SERIAL_ECHOPGM("Mixer: New relation : [ ");
  MIXER_STEPPER_LOOP(i) {
    SERIAL_ECHO_F(uint16_t(color[tool_index][i]) / csum, 3);
    SERIAL_ECHOPGM(", ");
  }
  SERIAL_ECHOLNPGM("]");
	SERIAL_EOL();

  SERIAL_ECHOPGM("Mix Percent: [ ");
  MIXER_STEPPER_LOOP(i) {
    SERIAL_ECHO(uint16_t(percentmix[i]));
    SERIAL_ECHOPGM(", ");
  }
  SERIAL_ECHOLNPGM("]");
	SERIAL_EOL();
#endif
}

void Mixer::copy_percentmix_to_color(mixer_comp_t (&tcolor)[MIXING_STEPPERS]) {
	 normalize_mixer_percent(&percentmix[0]);
  // Scale each component to the largest one in terms of COLOR_A_MASK
  // So the largest component will be COLOR_A_MASK and the other will be in proportion to it
  const float scale = (COLOR_A_MASK) * RECIPROCAL(_MAX(
    LIST_N(MIXING_STEPPERS, percentmix[0], percentmix[1], percentmix[2], percentmix[3], percentmix[4], percentmix[5])
  ));

  // Scale all values so their maximum is COLOR_A_MASK
  MIXER_STEPPER_LOOP(i) tcolor[i] = percentmix[i] * scale;

  #ifdef MIXER_NORMALIZER_DEBUG
  SERIAL_ECHOLNPGM("copy_percentmix_to_color >> ");
    SERIAL_ECHOPGM("Percentmix [ ");
    SERIAL_ECHOLIST_N(MIXING_STEPPERS, int(percentmix[0]), int(percentmix[1]), int(percentmix[2]), int(percentmix[3]), int(percentmix[4]), int(percentmix[5]));
    SERIAL_ECHOPGM(" ] to Color [ ");
    SERIAL_ECHOLIST_N(MIXING_STEPPERS, int(tcolor[0]), int(tcolor[1]), int(tcolor[2]), int(tcolor[3]), int(tcolor[4]), int(tcolor[5]));
    SERIAL_ECHOLNPGM(" ]");
  #endif
}

void Mixer::update_mix_from_vtool(const uint8_t j/*=selected_vtool*/) {
  float ctot = 0;
  MIXER_STEPPER_LOOP(i) ctot += color[j][i];
  MIXER_STEPPER_LOOP(i) percentmix[i] = (mixer_perc_t)(100.0f * color[j][i] / ctot);	  
  normalize_mixer_percent(&percentmix[0]);	

  #ifdef MIXER_NORMALIZER_DEBUG
  	SERIAL_ECHOLNPGM("update_mix_from_vtool");
  	SERIAL_EOL();		
    SERIAL_ECHOPAIR("V-tool ", int(j), " [ ");
    SERIAL_ECHOLIST_N(MIXING_STEPPERS, int(color[j][0]), int(color[j][1]), int(color[j][2]), int(color[j][3]), int(color[j][4]), int(color[j][5]));
    SERIAL_ECHOPGM(" ] to Percentmix [ ");
    SERIAL_ECHOLIST_N(MIXING_STEPPERS, int(percentmix[0]), int(percentmix[1]), int(percentmix[2]), int(percentmix[3]), int(percentmix[4]), int(percentmix[5]));
    SERIAL_ECHOLNPGM(" ]");
		SERIAL_EOL();
  #endif
	copy_percentmix_to_collector();
}

void Mixer::init_collector(const uint8_t index) {
	MIXER_STEPPER_LOOP(i) 
		collector[i] = pgm_read_byte(&INIT_MIX_RATE[index][i]);
}

void Mixer::reset_vtools(bool force_reset/* = false */) {
	if(!mixer.mixing_enabled && !force_reset) return;	
	
	//reset mix rate to default value
#if (MIXING_STEPPERS <= 4)
  MIXER_VTOOL_LOOP(t){
  	MIXER_STEPPER_LOOP(i){
			collector[i] = pgm_read_byte(&INIT_MIX_RATE[t][i]); 
  	}
		normalize(t);
		//TERN_(USE_PRECENT_MIXVALUE,copy_collector_to_percentmix());
  }	
#else
	// Virtual Tools 0, 1, 2, 3 = Filament 1, 2, 3, 4, etc.
  // Every virtual tool gets a pure filament
  LOOP_L_N(t, _MIN(MIXING_VIRTUAL_TOOLS, MIXING_STEPPERS)){
    MIXER_STEPPER_LOOP(i) color[t][i] = (t == i) ? COLOR_A_MASK : 0;
	}
  // Remaining virtual tools are 100% filament 1
  #if MIXING_VIRTUAL_TOOLS > MIXING_STEPPERS
  LOOP_S_L_N(t, MIXING_STEPPERS, MIXING_VIRTUAL_TOOLS){
    MIXER_STEPPER_LOOP(i) color[t][i] = (i == 0) ? COLOR_A_MASK : 0;
  }
  #endif 
#endif

	mixer.selected_vtool = 0;
	TERN_(GRADIENT_MIX, gradientmix_reset());
	TERN_(RANDOM_MIX, randommix_reset());

#ifdef MIXER_NORMALIZER_DEBUG
  SERIAL_EOL();
  SERIAL_ECHOLNPGM("reset_vtools!");
  SERIAL_EOL();
  MIXER_VTOOL_LOOP(t){
    MIXER_STEPPER_LOOP(i){ 
    	SERIAL_ECHOPGM("color[ ");
      SERIAL_ECHO(uint16_t(t));
			SERIAL_ECHOPGM("]");
			SERIAL_ECHOPGM("[");
			SERIAL_ECHO(uint16_t(i));
			SERIAL_ECHOPGM("]=");
			SERIAL_ECHO(uint16_t(color[t][i]));
			SERIAL_ECHOPGM(", ");
			SERIAL_EOL();
  	}
  }  
#endif
}

// called at boot
void Mixer::init() {
  
  reset_vtools(true);

  #if HAS_MIXER_SYNC_CHANNEL
    // AUTORETRACT_TOOL gets the same amount of all filaments
    MIXER_STEPPER_LOOP(i) color[MIXER_AUTORETRACT_TOOL][i] = COLOR_A_MASK;
  #endif
	MIXER_STEPPER_LOOP(i) { 
		if(i==0) (color[MIXER_DIRECT_SET_TOOL][i] = COLOR_A_MASK); 
		else color[MIXER_DIRECT_SET_TOOL][i] = 0;
	}
  ZERO(collector);  
  
  update_mix_from_vtool(); 

  TERN_(GRADIENT_MIX, update_gradient_for_planner_z());
  TERN_(RANDOM_MIX, update_randommix_for_planner_z());
}

void Mixer::refresh_collector(const float proportion/*=1.0*/, const uint8_t t/*=selected_vtool*/, float (&c)[MIXING_STEPPERS]/*=collector*/) {
  float csum = 0, cmax = 0;
  MIXER_STEPPER_LOOP(i) {
    const float v = color[t][i];
    cmax = _MAX(cmax, v);
    csum += v;
  }

  #ifdef MIXER_NORMALIZER_DEBUG
  SERIAL_ECHOPAIR("Mixer::refresh_collector(", proportion, ", ", int(t), ") cmax=", cmax, "  csum=", csum, "	color");
  #endif
  
  const float inv_prop = proportion / csum;
  MIXER_STEPPER_LOOP(i) {
    c[i] = color[t][i] * inv_prop;
	#ifdef MIXER_NORMALIZER_DEBUG
		SERIAL_ECHOPAIR(" [", int(t), "][", int(i), "] = ", int(color[t][i]), " (", c[i], ")	");
	#endif
  }
  #ifdef MIXER_NORMALIZER_DEBUG
  SERIAL_EOL();
  #endif
}


void Mixer::copy_percentmix_to_collector() {
  MIXER_STEPPER_LOOP(i){
  	collector[i] = (float)percentmix[i]/10.0;
  }
  #ifdef MIXER_NORMALIZER_DEBUG
    SERIAL_ECHOPGM("copy_percentmix_to_collector");
    SERIAL_EOL();
    SERIAL_ECHOPGM("Mix [ ");
    MIXER_STEPPER_LOOP(i){ 
		SERIAL_ECHO(int(percentmix[i]));
		SERIAL_ECHOPGM(", ");
    }
		SERIAL_ECHOPGM(" ] to collector [ ");
		MIXER_STEPPER_LOOP(i){ 
		SERIAL_ECHO(int(collector[i]));
		SERIAL_ECHOPGM(", ");
    }    
    SERIAL_ECHOLNPGM(" ]");
		SERIAL_EOL();
  #endif
}

#if ENABLED(USE_PRECENT_MIXVALUE)
void Mixer::normalize_mixer_percent(mixer_perc_t mix[MIXING_STEPPERS]){
		static uint16_t sum_mix;
		static uint8_t remainder_mix;	
		sum_mix = 0;
		MIXER_STEPPER_LOOP(i) sum_mix += mix[i];	
		MIXER_STEPPER_LOOP(i) mix[i] =	(mixer_perc_t)((mix[i]*100)/sum_mix);
		sum_mix = 0;
		MIXER_STEPPER_LOOP(i) sum_mix += mix[i];
		if(sum_mix < 100){
			sum_mix = 0;
			for(uint8_t i=0; i<MIXING_STEPPERS-1; i++) sum_mix += mix[i];
			if(mix[MIXING_STEPPERS-1] > 0)
				mix[MIXING_STEPPERS-1] = 100 - sum_mix;
			else {
				remainder_mix = 100 - sum_mix;
				for(uint8_t i=0; i<MIXING_STEPPERS-1; i++){
					if(mix[i] > 0 && mix[i] < 100-remainder_mix){
						mix[i] += remainder_mix;
						return;
					}
				}
			}
		}
		else if(sum_mix > 100){
			remainder_mix = sum_mix - 100;
			for(uint8_t i=0; i<MIXING_STEPPERS-1; i++){
				if(mix[i] > 5*remainder_mix){
						mix[i] -= remainder_mix;
						return;
				}
			}
		}
		else
			return;
	}	

void Mixer::copy_collector_to_percentmix() {
	bool need_mux = false;
	float sum_collector = 0.0;
	MIXER_STEPPER_LOOP(i) sum_collector += collector[i];
	if(sum_collector <= 20) 
		need_mux = true;
	else {
		MIXER_STEPPER_LOOP(i)	{
			if(collector[i] > 0.0 && collector[i] <= 1.0) {
				need_mux = true;
				break;
			}
		}
	}
	if(need_mux)
		MIXER_STEPPER_LOOP(i)	percentmix[i] = (mixer_perc_t)(collector[i]*10);
	else
  	MIXER_STEPPER_LOOP(i)	percentmix[i] = (mixer_perc_t)(collector[i]);
  normalize_mixer_percent(&percentmix[0]);
  
#ifdef MIXER_NORMALIZER_DEBUG
  SERIAL_ECHOLNPGM("copy_collector_to_percentmix");
  SERIAL_EOL();
  SERIAL_ECHOPGM("collector [ ");
  MIXER_STEPPER_LOOP(i){ 
		SERIAL_ECHO(int(collector[i]));
		SERIAL_ECHOPGM(", ");
  }
	SERIAL_ECHOPGM(" ] to Mix [ ");
	MIXER_STEPPER_LOOP(i){ 
		SERIAL_ECHO(int(percentmix[i]));
		SERIAL_ECHOPGM(", ");
  }    
  SERIAL_ECHOLNPGM(" ]");
	SERIAL_EOL();
#endif
}  
#endif

void Mixer::T(const uint_fast8_t c) {
  selected_vtool = c;
  TERN_(GRADIENT_VTOOL, refresh_gradient());
  update_mix_from_vtool();		
}


#if ENABLED(GRADIENT_MIX)
gradient_t Mixer::gradient = {
  false,    // enabled
  {0},      // color (array)
  0, 0,     // start_z, end_z
  0, 1,     // start_vtool, end_vtool
  {0}, {0}  // start_mix[], end_mix[]
  #if ENABLED(GRADIENT_VTOOL)
    , -1    // vtool_index
  #endif
};

void Mixer::gradientmix_reset(){
		gradient.enabled = false;
		gradient.start_z = 0.0;
		gradient.end_z = 0.0;
		gradient.start_vtool= 0;
		gradient.end_vtool = 1;		
}

void Mixer::gradient_control(const float z){
	if (gradient.enabled && did_pause_print == 0) {
		if (z >= gradient.end_z){
			gradient.enabled = false;
			T(gradient.end_vtool);
		}
		else
			update_gradient_for_z(z);
	}
}

void Mixer::update_mix_from_gradient() {
	float ctot = 0;
	MIXER_STEPPER_LOOP(i) ctot += gradient.color[i];
	MIXER_STEPPER_LOOP(i) percentmix[i] = (mixer_perc_t)CEIL(100.0f * gradient.color[i] / ctot);
#ifdef MIXER_NORMALIZER_DEBUG
	SERIAL_ECHOLNPGM("update_mix_from_gradient");
	SERIAL_EOL();
	SERIAL_ECHOPGM("Gradient [ ");
	SERIAL_ECHOLIST_N(MIXING_STEPPERS, int(gradient.color[0]), int(gradient.color[1]), int(gradient.color[2]), int(gradient.color[3]), int(gradient.color[4]), int(gradient.color[5]));
	SERIAL_ECHOPGM(" ] to Mix [ ");
	SERIAL_ECHOLIST_N(MIXING_STEPPERS, int(percentmix[0]), int(percentmix[1]), int(percentmix[2]), int(percentmixmix[3]), int(percentmix[4]), int(percentmix[5]));
	SERIAL_ECHOLNPGM(" ]");
	SERIAL_EOL();
#endif
}

// Refresh the gradient after a change
void Mixer::refresh_gradient() {	
	#if ENABLED(GRADIENT_VTOOL)
		const bool is_grd = (gradient.vtool_index == -1 || selected_vtool == (uint8_t)gradient.vtool_index);
	#else
		constexpr bool is_grd = true;
	#endif
	gradient.enabled = is_grd && gradient.start_vtool != gradient.end_vtool && gradient.start_z < gradient.end_z;
	if (gradient.enabled) {
		TERN_(RANDOM_MIX, randommix_reset());		
		update_mix_from_vtool(gradient.start_vtool);
		COPY(gradient.start_mix, percentmix);
		update_mix_from_vtool(gradient.end_vtool);
		COPY(gradient.end_mix, percentmix);
		mix_prev_z = -999.9;
		update_gradient_for_planner_z(true);
	}	
}

void Mixer::update_gradient_for_z(const float z, const bool force/* = false*/) {
	int16_t pct = 0;
	const float slice = gradient.end_z - gradient.start_z;
	if(slice < 0.1) return;
	
  if (z - mix_prev_z < 0.05 && !force) return;		
  mix_prev_z = z;	
	
	if(z >= gradient.start_z  && z <= gradient.end_z){
		pct = (int16_t)(((z - gradient.start_z) / slice)*100);
    NOLESS(pct, 0);
		NOMORE(pct, 100);
		MIXER_STEPPER_LOOP(i) {
    	const mixer_perc_t sm = gradient.start_mix[i];
    	percentmix[i] = sm + (mixer_perc_t)(((gradient.end_mix[i] - sm) * pct)/100);
  	}
  	copy_percentmix_to_color(gradient.color);		
	}
}

void Mixer::update_gradient_for_planner_z(const bool force/* = false*/) {
  update_gradient_for_z(planner.get_axis_position_mm(Z_AXIS), force);
}

#endif // GRADIENT_MIX

#if ENABLED(RANDOM_MIX)
int8_t Mixer::backup_vtool = 0;
randommix_t Mixer::random_mix = {
    false,    // enabled
    0, 0,     // start_z, end_z
    0.2,      // height
    MIXING_STEPPERS
};

void Mixer::randommix_reset(){
	random_mix.enabled = false;
	random_mix.start_z = 0.0;
	random_mix.end_z = 0.0;
	random_mix.height= 0.2;
	random_mix.extruders = MIXING_STEPPERS;
	T(backup_vtool);
}

void Mixer::randommix_control(const float z){
	if (random_mix.enabled && did_pause_print == 0) {
		if (z > random_mix.end_z){
			random_mix.enabled = false;
			T(backup_vtool);
		}
		else
			update_randommix_for_z(z);
	}
}
 
// Refresh the random after a change
void Mixer::refresh_random_mix() {	
	random_mix.enabled = (random_mix.start_z < random_mix.end_z) ;
	if (random_mix.enabled) {
		TERN_(GRADIENT_MIX, gradientmix_reset());		
		if(selected_vtool != MIXER_DIRECT_SET_TOOL && selected_vtool != backup_vtool) 
			backup_vtool = selected_vtool;
		mix_prev_z = -999.9;
		update_randommix_for_planner_z(true);
	}
	else 
		T(backup_vtool);
}

void Mixer::update_randommix_for_z(const float z, const bool force/* = false*/) {	
	if(z - mix_prev_z < random_mix.height && !force) return;	
	mix_prev_z = z;
	if(z >= random_mix.start_z && z <= random_mix.end_z + random_mix.height){		
		if(random_mix.extruders == 1){
			uint8_t cur_ext = random(100)%MIXING_STEPPERS;			
			MIXER_STEPPER_LOOP(i){
				collector[i] = 0.0;
			}
			collector[cur_ext] = 10.0;
		}
		else{
			MIXER_STEPPER_LOOP(i){
			  if(i < random_mix.extruders) 
					collector[i] = ((float)random(100)/10);
			  else
					collector[i] = 0.0;
			}
		}
		//collector==>percentmix,it is for display
		TERN_(USE_PRECENT_MIXVALUE, copy_collector_to_percentmix());
		//collector==>color, it is for printing
		if(selected_vtool != MIXER_DIRECT_SET_TOOL){
			backup_vtool = selected_vtool;
			selected_vtool = MIXER_DIRECT_SET_TOOL;
		}
		normalize(MIXER_DIRECT_SET_TOOL);		
  }
}

void Mixer::update_randommix_for_planner_z(const bool force/* = false*/) {
    update_randommix_for_z(planner.get_axis_position_mm(Z_AXIS), force);
}

#endif//RANDOM_MIX
#endif // MIXING_EXTRUDER
