@cls
@ECHO OFF
@ECHO "Are you sure to copy the configuration and platformio ini files to marlin?"
@PAUSE

copy /Y .\Configuration.h ..\..\Marlin\.
copy /Y .\Configuration_adv.h ..\..\Marlin\.
copy /Y .\platformio.ini ..\.

@PAUSE