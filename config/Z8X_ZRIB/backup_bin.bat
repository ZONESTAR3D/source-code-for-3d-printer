@cls
@echo off

@echo backup the bin file?
@pause

set dst=%date:~0,4%_%date:~5,2%_%date:~8,2%
if not exist hex md hex 
if not exist hex\%dst% md hex\%dst%

set "lj=%~p0"
set "lj=%lj:\= %"
for %%a in (%lj%) do set wjj=%%a

copy /Y ..\..\.pio\build\ZRIBV6\firmware.hex .

if exist firmware.hex (copy /Y firmware.hex .\hex\%dst%\.) else (@echo Oops! firmware file doesn't exist!)
if exist firmware.hex del firmware.hex
@pause