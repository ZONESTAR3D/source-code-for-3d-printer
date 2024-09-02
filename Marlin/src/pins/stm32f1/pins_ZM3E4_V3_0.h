/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


// =====================NOTE=====================
// #define TONE_TIMER		1
// #define TONE_CHANNEL		2
// Please redefine the TONE_TIMER and TONE_CHANNEL in 
// .platformio\packages\framework-arduinoststm32-maple\STM32F1\cores\maple\tone.cpp
// line 31 and line 34
// =====================NOTE=====================


#pragma once
#if NOT_TARGET(__STM32F1__)
  #error "Oops! Select an STM32F1 board in 'Tools > Board.'"
#endif

#define BOARD_INFO_NAME "ZONESTAR ZM3E4 V3.0"

//#define DISABLE_DEBUG
#define DISABLE_JTAG

#define FLASH_EEPROM_EMULATION
#define EEPROM_PAGE_SIZE     	(0x800) // 2KB
#define	MARLIN_EEPROM_SIZE		(EEPROM_PAGE_SIZE*2) //4KB
#define EEPROM_START_ADDRESS uint32(0x8000000 + (STM32_FLASH_SIZE) * 1024 - MARLIN_EEPROM_SIZE)
#define E2END                (EEPROM_PAGE_SIZE - 1)

//=============================================================================
// ZONESTAR ZM3E4 V3.0 (STM32F130VET6) board pin assignments
//=============================================================================
//	PA0									//	PB0		HEAT_1			//	PC0	   		AXU_SDA			  
//	PA1     		    		//	PB1     FAN1            //	PC1         TEMP_E1               
//	PA2	    HEAT_BED		//	PB2	    BOOT1           //	PC2	        TEMP_E0           
//	PA3	    PWR_HOLD  	//	PB3	    SPI3_SCK        //	PC3	        TEMP_BED
//	PA4     SD_CS				//	PB4     SPI3_MISO       //	PC4         SD_DETECT
//	PA5     SD_SCK    	//	PB5     SPI3_MOSI       //	PC5         HEAT_0
//	PA6     SD_MISO 		//	PB6     SERVO3          //	PC6         E1_STEP
//	PA7     SD_MOSI     //	PB7     SERVO2          //	PC7         E1_EN
//	PA8     X_DIR      	//	PB8     FAN2/SERVO1     //	PC8         FIL_RUNOUT_PIN
//	PA9     UART1_RX   	//	PB9     SERVO0          //	PC9         E0_DIR
//	PA10    UART1_TX   	//	PB10    TX3             //	PC10        E0_EN
//	PA11    USB_D-      //	PB11    RX3             //	PC11        Z2_EN
//	PA12    USB_D+      //	PB12    LED          	//	PC12        Z2_STEP
//	PA13    SWD_SDO     //	PB13    Z1_MAX          //	PC13        X_MIN
//	PA14    SWD_SCK     //	PB14    Y_MAX           //	PC14        WIFI_RST
//	PA15    SPI3_CS     //	PB15    PWR_DET         //	PC15        WIFI_CS

//	PD0	 	Z2_DIR		//	PE0		Y_EN
//	PD1     Z2_MIN		//	PE1		Y_STEP
//	PD2	    Z1_EN		//	PE2		Y_DIR
//	PD3	    Z1_STEP		//	PE3		Y_MIN
//	PD4	    Z1_DIR		//	PE4		X_DIR
//	PD5	    WIFI_RXD	//	PE5		X_STEP
//	PD6	    WIFI_TXD	//	PE6		X_EN
//	PD7	    Z1_MIN		//	PE7		AXU_SCL
//	PD8	    X_MAX		//	PE8		BTN_EN1
//	PD9	    E3_DIR		//	PE9		LCD_SCK
//	PD10    E3_STEP		//	PE10	LCD_MOSI
//	PD11    E3_EN		//	PE11	BEEPER
//	PD12    E2_DIR		//	PE12	LCD_EN
//	PD13    E2_STEP		//	PE13	KILL
//	PD14    E2_EN		//	PE14	BTN_EN2
//	PD15    E1_DIR		//	PE15	BTN_ENC
//=============================================================================
//EXP1 connector																
//	   MARK I/O     ZONESTAR_LCD12864		REPRAPDISCOUNT_LCD12864   	ANET_FULL_GRAPHICS_LCD 	
//	10 RS		PE13   	KILL								BTN_ENC											BEEP
//	9  BP		PE11		BEEP								BEEP												LCDE
//	8  EN		PE12		DOGLCD_CS						LCDRS												BTN_ENC
//	7  MOSI	PE10 		DOGLCD_SCK					LCDE												NC
//	6  EN1	PE14    BTN_EN1							NC													BTN_EN1
//	5  SCK	PE9   	DOGLCD_MOSI					LCD4												LCD4
//	4  ENC 	PE15   	BTN_ENC							NC													BTN_EN2
//	3  EN2  PE8			BTN_EN2							NC													LCDRS
//	2  +5V															+5V													+5V							
//	1  GND															GND													GND					

//EXP2 connector								
//	  MARK   I/O   LCD_DWIN				REPRAPDISCOUNT_LCD12864
// 10	SDA     PE7 										
//	9	SCL  		PC0
//	8	RX1  		PA9		RXD									NC
//	7	TX1  		PA10	TXD									BTN_EN2
//	6	CS3	 		PA15  BEEPER							NC
//	5	MISO3  	PB4		BTN_ENC							BTN_EN1
//	4	MOSI3  	PB5		BTN_EN2							KILL
//	3	SCK3  	PB3		BTN_EN1							NC						
//	2	+5V																+5V
//	1	GND																GND

//WiFi
//	1	+5V 	    	
//	2	GND
//	3	WIFI_TXD  	PD5		UART2_RX
//	4	WIFI_RXD  	PD6		UART2_TX
//	5	WIFI_RST  	PC14		
//	6	WIFI_CS  	  PC15		

//SERVOs
// SERVO0 		PB9
// SERVO1 		PB8	(FAN1_PIN)
// SERVO2 		PB7
// SERVO3 		PB6
//=============================================================================


//
// Limit Switches
//
#ifdef USE_XMIN_PLUG
#define X_STOP_PIN         PC13
#endif

#ifdef USE_YMIN_PLUG
#define Y_STOP_PIN         PE3
#endif

#ifdef USE_ZMIN_PLUG
#define Z_MIN_PIN          PD7
#endif

#ifdef USE_XMAX_PLUG
#define X_MAX_PIN          PD8
#endif

#ifdef USE_YMAX_PLUG
#define Y_MAX_PIN          PB14
#endif

#ifdef USE_ZMAX_PLUG
#define Z_MAX_PIN          PB13
#endif

//
// Steppers
//
#ifdef COREXY
#define X_ENABLE_PIN       PE0
#define X_STEP_PIN         PE1
#define X_DIR_PIN          PE2

#define Y_ENABLE_PIN       PE6
#define Y_STEP_PIN         PE5
#define Y_DIR_PIN          PE4
#else
#define X_ENABLE_PIN       PE6
#define X_STEP_PIN         PE5
#define X_DIR_PIN          PE4

#define Y_ENABLE_PIN       PE0
#define Y_STEP_PIN         PE1
#define Y_DIR_PIN          PE2
#endif

#define Z_ENABLE_PIN       PD2
#define Z_STEP_PIN         PD3
#define Z_DIR_PIN          PD4

#ifdef OPTION_DUALZ_DRIVE
#define Z2_ENABLE_PIN      PC11
#define Z2_STEP_PIN        PC12
#define Z2_DIR_PIN         PD0
#endif

#ifdef OPTION_Z2_ENDSTOP
#define Z2_MIN_PIN		  	 PD1	//Z2_MIN_PIN
#endif

//#define Z2_MAX_PIN         PB12

#ifdef	SWITCH_EXTRUDER_SQUENCY
#define E3_ENABLE_PIN      PC10
#define E3_STEP_PIN        PA8
#define E3_DIR_PIN         PC9

#define E2_ENABLE_PIN      PC7
#define E2_STEP_PIN        PC6
#define E2_DIR_PIN         PD15

#define E1_ENABLE_PIN      PD14
#define E1_STEP_PIN        PD13
#define E1_DIR_PIN         PD12

#define E0_ENABLE_PIN      PD11
#define E0_STEP_PIN        PD10
#define E0_DIR_PIN         PD9
#else
#define E0_ENABLE_PIN      PC10
#define E0_STEP_PIN        PA8
#define E0_DIR_PIN         PC9

#define E1_ENABLE_PIN      PC7
#define E1_STEP_PIN        PC6
#define E1_DIR_PIN         PD15

#define E2_ENABLE_PIN      PD14
#define E2_STEP_PIN        PD13
#define E2_DIR_PIN         PD12

#define E3_ENABLE_PIN      PD11
#define E3_STEP_PIN        PD10
#define E3_DIR_PIN         PD9
#endif
//
// Temperature Sensors
//
#define TEMP_0_PIN         PC2   // TH0
#define TEMP_1_PIN         PC1   // TH1
#define TEMP_BED_PIN       PC3   // TB1
//
// Heaters
//
#define HEATER_0_PIN       PC5   // HEATER0
#define HEATER_1_PIN       PB0   // HEATER1
#define HEATER_BED_PIN     PA2   // HOT BED

#if ENABLED(OPTION_CHAMBER)
#undef TEMP_1_PIN
#undef HEATER_1_PIN
#define TEMP_CHAMBER_PIN	PC1
#define HEATER_CHAMBER_PIN	PB0
#endif
//
// Fans
//
#define FAN_PIN            PB1
#define FAN1_PIN    			 PB8

//
// Misc. Functions
//
//#define POWER_LOSS_PIN     PB15   //
#define LED_PIN            PA0

//Power control pin
#define SUICIDE_PIN        PA3
//#define	PS_ON_PIN						PB6//PB7//PB9
#define FIL_RUNOUT_PIN	   PC8


//SD card
#define ENABLE_SPI1
#define SD_DETECT_PIN      PC4
#define SCK_PIN            PA5
#define MISO_PIN           PA6
#define MOSI_PIN           PA7
#if SD_CONNECTION_IS(ONBOARD)
#define ONBOARD_SD_CS_PIN	 PA4
#else
#define SS_PIN             PA4
#endif

//WiFi. Functions
#define WIFI_RST          PC15
#define WIFI_EN          	PC14


#if ENABLED(ZONESTAR_12864LCD)
#define	LCDSCREEN_NAME		"ZONESTAR LCD12864"
#define LCD_PINS_RS 			PE12		//7 CS make sure for zonestar zm3e4!
#define LCD_PINS_ENABLE 	PE9			//6 DATA make sure for zonestar zm3e4!
#define LCD_PINS_D4 			PE10    //8 SCK make sure for zonestar zm3e4!
#define BEEPER_PIN        PE11
#define KILL_PIN          	-1//PE13
#define BTN_EN1 					PE8
#define BTN_EN2 					PE14
#define BTN_ENC 					PE15

#elif ENABLED(ANET_FULL_GRAPHICS_LCD)
#define	LCDSCREEN_NAME		"ANET_12864LCD"
#define LCD_PINS_RS 			PE14
#define LCD_PINS_ENABLE 	PE11
#define LCD_PINS_D4 			PE9
#define BEEPER_PIN        PE13
#define KILL_PIN          -1
#define BTN_EN1 					PE8
#define BTN_EN2 					PE15
#define BTN_ENC 					PE12

#elif ENABLED(REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER)
#define	LCDSCREEN_NAME		"REPRAPDISCOUNT LCD12864"
#define LCD_PINS_RS 			PE12	//7 CS make sure for zonestar zm3e4!
#define LCD_PINS_ENABLE 	PE10	//6 DATA make sure for zonestar zm3e4!
#define LCD_PINS_D4 			PE9    	//8 SCK make sure for zonestar zm3e4!
#define LCD_PINS_D5 			-1    	//mosi
#define LCD_PINS_D6 			-1      
#define LCD_PINS_D7 			-1
#define LCD_RESET_PIN     -1

#define BEEPER_PIN    		PE11
#define KILL_PIN      		PB5
#define BTN_EN1 					PB4
#define BTN_EN2 					PA10
#define BTN_ENC 					PE13

#define DOGLCD_A0           -1
#define DOGLCD_CS           -1
#define DOGLCD_MOSI         -1
#define DOGLCD_SCK          -1
#elif EITHER(REPRAP_DISCOUNT_SMART_CONTROLLER, ZONESTAR_LCD2004_KNOB)
#define	LCDSCREEN_NAME	"LCD2004 KNOB"
#define LCD_PINS_RS         PE12    
#define LCD_PINS_ENABLE     PE10    
#define LCD_PINS_D4         PE9    
#define LCD_PINS_D5         PE14    
#define LCD_PINS_D6         PE8    
#define LCD_PINS_D7         PE15

#define BTN_EN1 						PA10		//EXP2_PIN7
#define BTN_EN2 						PB4			//EXP2_PIN5
#define BTN_ENC 						PE13		//EXP1_PIN10
#define BEEPER_PIN 					PE11		//EXP1_PIN9
#define KILL_PIN 						PB5			//EXP2_PIN4
#elif ENABLED(ZONESTAR_LCD2004_ADCKEY)
#define	LCDSCREEN_NAME	"LCD2004 5KEY"
#define LCD_PINS_RS         PE12    
#define LCD_PINS_ENABLE     PE10    
#define LCD_PINS_D4         PE9    
#define LCD_PINS_D5         PE14    
#define LCD_PINS_D6         PE8    
#define LCD_PINS_D7         PE15 

#define ADC_KEYPAD_PIN		PE13//PC0			//PIN6 of AUX1
#endif

#if ENABLED(ZONESTAR_DWIN_LCD)
//Connect to EXP2 connector
#define	LCDSCREEN_NAME		"ZONESTAR DWIN LCD"

#define BEEPER_PIN          PA15    //
#define KILL_PIN          	-1			//PC0
#define BTN_EN1 						PB3    	//
#define BTN_EN2 						PB5    	//
#define BTN_ENC 						PB4   	//
#endif

//SERVO
//Remap SERVO0 PIN for BLTouch
#if ENABLED(BLTOUCH)
#if ENABLED(BLTOUCH_ON_EXP1)
//BLTouch connect to EXP1
#define	BLTOUCH_PROBE_PIN 		PE8
#define PROBE_GND_PIN		  		PE15
#define	SERVO0_PIN		  			PE14
#elif ENABLED(BLTOUCH_ON_EXP2)
//BLTouch connect to EXP2
#define	BLTOUCH_PROBE_PIN 		PB3
#define PROBE_GND_PIN		  		PB5
#define	SERVO0_PIN		  			PA15
#else
#define	SERVO0_PIN		  			PB9	
#define	BLTOUCH_PROBE_PIN 		PB6//PB13//PB6//PB7
#endif
#endif
//#define	SERVO2_PIN		  PB7
//#define	SERVO3_PIN		  PB6

#if ENABLED(SPINDLE_FEATURE)
#define SPINDLE_LASER_ENA_PIN		PA2
#endif

#if HAS_MARLINUI_U8GLIB
#define BOARD_ST7920_DELAY_1 DELAY_NS(200)    //Tclk_fall <200ns
#define BOARD_ST7920_DELAY_2 DELAY_NS(250)    //Tdata_width >200ns
#define BOARD_ST7920_DELAY_3 DELAY_NS(200)    //Tclk_rise <200ns
#endif

//===========================================
//Repeat printing
//
#if ENABLED(OPTION_REPEAT_PRINTING)
//ENDSTOP pin of ARM
#ifdef X_MAX_PIN
#undef X_MAX_PIN
#endif
#ifdef Y_MAX_PIN
#undef Y_MAX_PIN
#endif
#define RPARML_MIN_PIN   			PD8		//X_MAX_PIN
#define RPARMR_MIN_PIN   			PB14	//Y_MAX_PIN

//Motor drive pin
//Push: 	P = HIGH, N = LOW 
//Pull: 	P = LOW, 	N = HIGH 
//Brake: 	P = HIGH, N = HIGH 
//Stop: 	P = LOW, 	N = LOW 
#if DISABLED(BLTOUCH)
 #if ENABLED(ZONESTAR_DWIN_LCD)
	 #define RP_ARMLP_PIN       	PE15		//L+ EXP1_PIN4
	 #define RP_ARMLN_PIN      		PE14		//L- EXP1_PIN6
	 #define RP_ARMRP_PIN       	PE9			//R+ EXP1_PIN5
	 #define RP_ARMRN_PIN      		PE8			//R- EXP1_PIN3
 #elif ENABLED(ZONESTAR_12864LCD)
	 #define RP_ARMLP_PIN       	PB5			//L+ EXP2_PIN4
	 #define RP_ARMLN_PIN      		PA15		//L- EXP2_PIN6
	 #define RP_ARMRP_PIN       	PB4			//R+ EXP2_PIN5
	 #define RP_ARMRN_PIN      		PB3			//R- EXP2_PIN3
 #endif
#else
  #if ENABLED(BLTOUCH_ON_EXP1)
		#define RP_ARMLP_PIN        PE13		//L+ EXP2_PIN10
		#define RP_ARMLN_PIN    		PE12		//L- EXP2_PIN8
		#define RP_ARMRP_PIN        PE11		//R+ EXP2_PIN9
		#define RP_ARMRN_PIN    		PE10		//R- EXP2_PIN7
	#elif ENABLED(BLTOUCH_ON_EXP2)
		#define RP_ARMLP_PIN        PA10		//L+ EXP2_PIN7
		#define RP_ARMLN_PIN   		  PA9			//L- EXP2_PIN8
		#define RP_ARMRP_PIN        PB6			//R+	SEVERO_S1
		#define RP_ARMRN_PIN    	  PB7			//R- SEVERO_S2
	#endif
#endif

#endif
//===========================================
//Special
#ifdef EXCHANGE_XMIN_XMAX
#ifdef USE_XMIN_PLUG
#undef X_STOP_PIN
#define X_STOP_PIN         PD8
#endif
#ifdef USE_XMAX_PLUG
#undef X_MAX_PIN
#define X_MAX_PIN          PC13
#endif
#endif//EXCHANGE_XMIN_XMAX

#ifdef EXCHANGE_YMIN_YMAX
#ifdef USE_YMIN_PLUG
#undef Y_STOP_PIN
#define Y_STOP_PIN         PB14
#endif
#ifdef USE_YMAX_PLUG
#undef Y_MAX_PIN
#define Y_MAX_PIN          PE3
#endif
#endif//EXCHANGE_YMIN_YMAX

#ifdef EXCHANGE_XDRIVER_Z2DRIVER
#ifdef USE_XMIN_PLUG
#undef X_STOP_PIN
#define X_STOP_PIN         PD1
#endif
#ifdef COREXY
#undef Y_ENABLE_PIN
#undef Y_STEP_PIN
#undef Y_DIR_PIN
#define Y_ENABLE_PIN      PC11
#define Y_STEP_PIN        PC12
#define Y_DIR_PIN         PD0
#else
#undef X_ENABLE_PIN
#undef X_STEP_PIN
#undef X_DIR_PIN
#define X_ENABLE_PIN      PC11
#define X_STEP_PIN        PC12
#define X_DIR_PIN         PD0
#endif
#ifdef OPTION_DUALZ_DRIVE
#undef Z2_ENABLE_PIN
#undef Z2_STEP_PIN
#undef Z2_DIR_PIN
#define Z2_ENABLE_PIN       PE6
#define Z2_STEP_PIN         PE5
#define Z2_DIR_PIN          PE4
#endif
#endif//EXCHANGE_XAXIS_Z2AXIS

#if ENABLED(EXCHANGE_E0T_E1T)
#undef TEMP_0_PIN
#undef TEMP_1_PIN
#define TEMP_0_PIN					PC1
#define TEMP_1_PIN					PC2
#endif

#if HAS_TMC220x
#define	X_SERIAL_TX_PIN			PB10
#define	Y_SERIAL_TX_PIN			PB10
#define	Z_SERIAL_TX_PIN			PB10
#define	Z2_SERIAL_TX_PIN		PB10
#define	X_SERIAL_RX_PIN			PB11
#define	Y_SERIAL_RX_PIN			PB11
#define	Z_SERIAL_RX_PIN			PB11
#define	Z2_SERIAL_RX_PIN		PB11
#endif

//===========================================
