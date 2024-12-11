@cls
@echo off

@echo backup the current Configuration?
@pause

set dst=%date:~0,4%_%date:~5,2%_%date:~8,2%
if not exist backup md backup
if not exist backup\%dst% md backup\%dst%

copy /Y .\Configuration.h backup\%dst%
copy /Y .\Configuration_adv.h backup\%dst%
copy /Y .\platformio.ini backup\%dst%

@pause