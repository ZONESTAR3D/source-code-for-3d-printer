@cls
@echo off

@echo backup the current Configuration?
@pause

set dst=%date:~0,4%_%date:~5,2%_%date:~8,2%
if not exist backup md backup
if not exist backup\Z8PM4Pro md backup\Z8PM4Pro 
if not exist backup\Z8PM4Pro\%dst% md backup\Z8PM4Pro\%dst%

copy /Y .\Configuration.h backup\Z8PM4Pro\%dst%
copy /Y .\Configuration_adv.h backup\Z8PM4Pro\%dst%
copy /Y .\platformio.ini backup\Z8PM4Pro\%dst%

@pause