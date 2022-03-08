@cls
@echo off
set "lj=%~p0"
set "lj=%lj:\= %"
for %%a in (%lj%) do set wjj=%%a
color fc
@echo "Are you sure overwrite %wjj% by the settings of current project?"
@pause

copy /Y ..\..\Marlin\Configuration.h .
copy /Y ..\..\Marlin\Configuration_adv.h .
copy /Y ..\..\platformio.ini .

@pause