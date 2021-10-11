@cls
@echo off
set "lj=%~p0"
set "lj=%lj:\= %"
for %%a in (%lj%) do set wjj=%%a
color fc
@echo "!!!Are you sure to apply %wjj% as the current project?!!!"
@pause

copy /Y .\Configuration.h ..\..\Marlin\.
copy /Y .\Configuration_adv.h ..\..\Marlin\.
copy /Y .\platformio.ini ..\..\.

@pause