@echo off
devenv.com "buildRelease64/libISEAFrame.vcxproj" /build "Release|x64" /out 
if errorlevel 1 (
   exit /b %errorlevel%
)
exit 0
