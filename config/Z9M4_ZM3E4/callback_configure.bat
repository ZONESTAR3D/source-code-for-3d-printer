@cls
@ECHO "Are you sure overwrite current configurations and platformio?"
@PAUSE

copy /Y ..\..\Marlin\Configuration.h .
copy /Y ..\..\Marlin\Configuration_adv.h .
copy /Y ..\..\platformio.ini .

@PAUSE