## ZONESTAR 3D Printer Firmware Source Code
### This source code can be applied to the below Machine     
|     Config     |         Model       |    Control board    |  
|----------------|---------------------|---------------------|  
|  D805S_ZM3E4   |    D805S(M2/R2)     |       ZM3E4         |  
|  D805S_ZRIB    |    D805S(M2/R2)     |       ZRIB          |  
|  Z5S_ZM3E2     |     Z5S(M2/R2)      |       ZM3E2         |  
|  Z5XM3_ZM3E4   |     Z5X(M3/R3)      |       ZM3E4         |  
|  Z8P_ZM3E4     |     Z8P(M3/R3)      |       ZM3E4         |  
|  Z8R3_ZRIB     |     Z8X(R3)         |       ZRIB          |  
|  Z8T_ZRIB      |     Z8T(M3)         |       ZRIB          |  
|  Z8XM2_ZM3E4   |     Z8X(M2/R2)      |       ZM3E4         |  
|  Z9M3_ZRIB     |     Z9M3(R3)        |       ZRIB          |  
|  Z9M3_ZM3E4    |     Z9M3(R3)        |       ZM3E4         |  
|  Z9M4_ZM3E4    |     Z9M4            |       ZM3E4         |  
|  	Z9V5   		 |    Z9V5(Pro)-MK1(2) |       ZM3E4         |  
|  Z9V5_MK3      |    Z9V5(Pro)-MK3    |       ZM3E4         |
|  Z10M2_ZRIB    |      Z10(M2/R2)     |       ZRIB          |  
|  Z10M2_ZM3E4   |      Z10(M2/R2)     |       ZM3E4         |  

---
## How to build firmware
### 1. Prepare the compilation environment
1. Install "Visual Studio Code: Visit the [***Setting up Visual Studio Code***](https://code.visualstudio.com/docs/setup/setup-overview) page to download and install the latest Visual Studio Code for your particular platform.  
2. Install the PlatformIO IDE: Head over to the [***Get PlatformIO IDE***](https://platformio.org/install/ide?install=vscode) page to learn how to install PlatformIO IDE in Visual Studio Code.  
**More detail, please refer to** [***Install marlin compilation environment***](https://marlinfw.org/docs/basics/install_platformio_vscode.html)  

### 2. Prepare firmware source code
1. Download this firmware source code and stored to you PC  
2. Choose and replace config files  
   - find and copy the configuration files of your machine from the "config" directory  
   - copy and replace configuration.h and configuration_adv.h to "marlin" directory  
   - copy and replace platformio.ini to root directory of the source code  
   - You may need to modify the file "configuration.h" and "configuration_adv.h" according to the setting of your machine.  
3. Open the project in Visual Stuido Code  
Use the "Open Folder…" command in the Visual Studio Code "File" menu to open the source code directory which you stored, e.g. "main".  

### 3. Build firmware
1. Run the "Run build task" command in the Visual Studio Code "Terminal" menu, and then choose "PlatformIO:build" task to build the firmware bin (or Hex) file.
   - **32bits control board(ZM3E2/ZM3E4)**: firmware.bin file.
   - **8bits control board(ZMIB/ZRIB)**: firmware.hex file.
2. Wait the build finished
3. find the *firmware.bin* or *firmware.HEX* file in the ".pio/build/STM32F103VE_ZM3E4V2_USB(or others)" of your source code directory

---
## Upload firmware to your 3d printer
1. For 32 bits control board, you can copy the *firmware.bin* to SD card and upload the firmware to your PC
2. For 8bits control board, you need to upload firmwre to the control board by PC (from USB cable)
About detail, please refer to [**upload firmware**](https://github.com/ZONESTAR3D/firmware/readme.md)

---
## License

Marlin is published under the [GPL license](/LICENSE) because we believe in open development. The GPL comes with both rights and obligations. Whether you use Marlin firmware as the driver for your open or closed-source product, you must keep Marlin open, and you must provide your compatible Marlin source code to end users upon request. The most straightforward way to comply with the Marlin license is to make a fork of Marlin on Github, perform your modifications, and direct users to your modified fork.

While we can't prevent the use of this code in products (3D printers, CNC, etc.) that are closed source or crippled by a patent, we would prefer that you choose another firmware or, better yet, make your own.
