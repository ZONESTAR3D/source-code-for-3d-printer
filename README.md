## ZONESTAR 3D Printer Firmware source code
# this firmware can be applied to the below printer model
# Z5X_ZMIB		Control board: ZMIB
# Z6_ZMIB			Control board: ZMIB
# Z8T_ZRIB  	Control board: ZRIBV6
# Z8P_ZM3E4 	Control board: ZM3E4
# Z9M3_ZRIB 	Control board: ZRIBV6
# Z9M4_ZM3E4 	Control board: ZM3E4
# Z9V5_ZM3E4 	Control board: ZM3E4


## Build Firmware
#1. Install VSCode
Visit the Setting up "Visual Studio Code" page [https://code.visualstudio.com/docs/setup/setup-overview] to download and install the latest VSCode for your particular platform.

#2. Install the PlatformIO IDE
Head over to the “Get PlatformIO IDE” page [https://platformio.org/install/ide?install=vscode]  to learn how to install PlatformIO IDE in VSCode.

#3 find and copy the configuration files for your machine from the "config" directory, you need to:
1). copy and replace configuration.h and configuration_adv.h to "marlin" directory

2). copy and replace platformio.ini to "main" directory

You may need to modify the file "configuration.h" and "configuration_adv.h" according to the setting of your machine.


#3. Use the "Open Folder…" command in the VSCode "File" menu to open the source code directory which you stored, e.g. "main".

#4. Run the "Run build task" command in the VSCode "Terminal" menu, and then choose "PlatformIO:build" task to build the source code.


## License

Marlin is published under the [GPL license](/LICENSE) because we believe in open development. The GPL comes with both rights and obligations. Whether you use Marlin firmware as the driver for your open or closed-source product, you must keep Marlin open, and you must provide your compatible Marlin source code to end users upon request. The most straightforward way to comply with the Marlin license is to make a fork of Marlin on Github, perform your modifications, and direct users to your modified fork.

While we can't prevent the use of this code in products (3D printers, CNC, etc.) that are closed source or crippled by a patent, we would prefer that you choose another firmware or, better yet, make your own.
