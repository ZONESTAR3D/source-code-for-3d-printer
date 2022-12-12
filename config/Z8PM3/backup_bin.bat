@cls
@echo off

@echo backup the bin file?
@pause

set dst=%date:~0,4%_%date:~5,2%_%date:~8,2%
if not exist bin md bin 
if not exist bin\%dst% md bin\%dst%

set "lj=%~p0"
set "lj=%lj:\= %"
for %%a in (%lj%) do set wjj=%%a

copy /Y ..\..\.pio\build\ZM3E4V2\firmware.bin .

if exist firmware.bin (copy /Y firmware.bin .\bin\%dst%\.) else (@echo Oops! firmware file doesn't exist!)
if exist firmware.bin del firmware.bin
@pause