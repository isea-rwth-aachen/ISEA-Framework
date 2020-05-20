@echo off
devenv.com "buildRelease32/libISEAFrame.vcxproj" /build Release  /out 
if errorlevel 1 (
   exit /b %errorlevel%
)
exit 0