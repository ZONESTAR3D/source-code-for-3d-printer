@cls
@echo off

@echo backup marlin soure code?
@pause

set dst=Marlin_%date:~2,2%%date:~5,2%%date:~8,2%
if not exist %dst% md %dst%

xcopy /E/Y ..\Marlin\*.* 	.\%dst%\.

@pause