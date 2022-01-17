@cls
@echo off

@echo backup the current Configuration?
@pause

copy /Y .\Configuration.h .\backup\.
copy /Y .\Configuration_adv.h .\backup\.
copy /Y .\platformio.ini .\backup\.

@pause