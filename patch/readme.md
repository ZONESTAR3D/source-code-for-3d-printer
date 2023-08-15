## 1. U8glib
When using 128x64 dots LCD screen, u8g library is required. Although it will be downloaded automatically when compiling, but the downloaded library may cause display problems, and the compilation time is very long. You can use this library directly.
>
Copy "U8glib-HAL" directory to ".pio\libdeps\{board_type}\"  

## 2. LiquidCrystal.h
When using 32-bit control board and 1604 or 2004 LCD screen, the delay setting of the library automatically downloaded is too small, which will case display issue and need to be modified.
>
Copy "LiquidCrystal.h" to ".pio\libdeps\{board_type}\LiquidCrystal\." to replace the orignal one.
