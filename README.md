# ZONESTAR 3D Printer Firmware Source Code
***this firmware can be applied to the below printer model***
- **D805S_ZM3E4**  *Printer model:* D805S(M2/R2)   *Control board:* ZM3E4
- **D805S_ZRIB**   *Printer model:* D805S(M2/R2)   *Control board:* ZRIBV5/ZRIBV6
- **Z5S_ZM3E2**	   *Printer model:* Z5S(M2/R2)	   *Control board:* ZM3E2
- **Z5XM3_ZM3E4**  *Printer model:* Z5X(M3/R3)	   *Control board:* ZM3E4
- **Z8P_ZM3E4**	   *Printer model:* Z8P(M3/M4)	   *Control board:* ZM3E4
- **Z8R3_ZRIB**	   *Printer model:* Z8-R3	   *Control board:* ZRIBV5/ZRIBV6
- **Z8T_ZRIB**     *Printer model:* Z8T		   *Control board:* ZRIBV6
- **Z8XM2_ZM3E4**  *Printer model:* Z8X(M2)	   *Control board:* ZM3E4
- **Z9M3_ZRIB**    *Printer model:* Z9M3(R3)	   *Control board:* ZRIBV6
- **Z9M3_ZM3E4**   *Printer model:* Z9M3(R3)	   *Control board:* ZM3E4
- **Z9M4_ZM3E4**   *Printer model:* Z9M4	   *Control board:* ZM3E4
- **Z9V5** 	   *Printer model:* Z9V5(Pro)	   *Control board:* ZM3E4
- **Z10M2_ZM3E4**  *Printer model:* Z10M2	   *Control board:* ZM3E4

---
# Build Firmware

## Prepare the compilation environment and software
**Please refer to this link: https://marlinfw.org/docs/basics/install_platformio_vscode.html**
1. **Install "Visual Studio Code**  
Visit the Setting up "Visual Studio Code" page [https://code.visualstudio.com/docs/setup/setup-overview] to download and install the latest Visual Studio Code for your particular platform.*
2. **Install the PlatformIO IDE**  
Head over to the “Get PlatformIO IDE” page [https://platformio.org/install/ide?install=vscode]    to learn how to install PlatformIO IDE in Visual Studio Code.

## Prepare the compilation environment and software
1. **Choose and replace config files**  
   - find and copy the configuration files of your machine from the "config" directory  
   - copy and replace configuration.h and configuration_adv.h to "marlin" directory  
   - copy and replace platformio.ini to root directory of the source code  
   - You may need to modify the file "configuration.h" and "configuration_adv.h" according to the setting of your machine.  
2. **Open the project in Visual Stuido Code**  
Use the "Open Folder…" command in the Visual Studio Code "File" menu to open the source code directory which you stored, e.g. "main".  


## Build firmware
1. Run the "Run build task" command in the Visual Studio Code "Terminal" menu, and then choose "PlatformIO:build" task to build the firmware bin (or Hex) file.
- 32bits control board(ZM3E2/ZM3E4): firmware.bin file
- 8bits control board(ZMIB/ZRIB): firmware.hex file
2. Wait the build finished
3. find the *firmware.bin or firmware.HEX* file in the ".pio/build/STM32F103VE_ZM3E4V2_USB(or others)" of your source code directory

## upload firmware to the control board
1. for 32 bits control board, you can copy the *firmware.bin* to SD card and upload the firmware to your PC
2. for 8bits control board, you need to upload firmwre to the control board by PC (from USB cable)
About detail, please refer to: https://github.com/ZONESTAR3D/Firmware

---
# License

Marlin is published under the [GPL license](/LICENSE) because we believe in open development. The GPL comes with both rights and obligations. Whether you use Marlin firmware as the driver for your open or closed-source product, you must keep Marlin open, and you must provide your compatible Marlin source code to end users upon request. The most straightforward way to comply with the Marlin license is to make a fork of Marlin on Github, perform your modifications, and direct users to your modified fork.

While we can't prevent the use of this code in products (3D printers, CNC, etc.) that are closed source or crippled by a patent, we would prefer that you choose another firmware or, better yet, make your own.
