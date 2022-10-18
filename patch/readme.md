## 1. tone.cpp   
For the ZONESTAR ZM3E4 and ZM3E2 mainboards, the tone library needs to be used because the passive buzzer needs to use a timer to generate sound. However, the default timer serial number used in the framework arduinoststm32 maple is inconsistent with the actual situation, so the tone needs to be modified The sequence number of the timer used in CPP.
>
Copy tone.cpp to replace of the same file in the below directory:
.platformio\packages\framework-arduinoststm32-maple\STM32F1\cores\maple

## 2. U8glib
When using 128x64 dots LCD screen, u8g library is required. Although it will be downloaded automatically when compiling, but the downloaded library may cause display problems, and the compilation time is very long. You can use this library directly.
>
Copy "U8glib-HAL" directory to ".pio\libdeps\{board_type}\"  

## 3. LiquidCrystal.h
When using 32-bit control board and 1604 or 2004 LCD screen, the delay setting of the library automatically downloaded is too small, which will case display issue and need to be modified.
>
Copy "LiquidCrystal.h" to ".pio\libdeps\{board_type}\LiquidCrystal\." to replace the orignal one.
