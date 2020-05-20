#!/bin/bash

# This script is used to specify the locations of all external dependencies

FRAMEWORK_EXTENSIONS_PATH="C:\Data\\$(whoami)\FrameworkExtensions"

MSVC_INCLUDE_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.16.27023\include"
MSVC_LIB_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.16.27023\lib"
MSVC_BIN_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.16.27023\bin"
VCVARS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
VISUAL_STUDIO_IDE_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE"
MSBUILD_BIN_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\bin"

WINDOWS_KIT_INCLUDE_PATH="C:\Program Files (x86)\Windows Kits\10\Include\10.0.17763.0"
WINDOWS_KIT_LIB_PATH="C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17763.0"

MATLAB_64_PATH="C:\Program Files\matlab\R2018b"
MATLAB_32_PATH="C:\Program Files (x86)\matlab\R2018b"
MATLAB_LINUX_PATH="/usr/local/MATLAB/R2019b/bin/matlab"

CUSTOM_BINARY_PATH="C:\mingw64\bin"

# converts an absolute windows path to an absolute linux path
# is safe to use on already converted paths
function linuxpath()
{
	LINUXPATH=$( echo $* | sed -e 's/\\/\//g' -e 's/^/\//' -e 's/^\/*/\//' -e 's/://' )
	echo $LINUXPATH
}

# converts an absolute linux path to an absolute windows path
# is safe to use on already converted paths
function windowspath()
{
	WINPATH=$( echo $* | sed -e 's/\//\\/g' -e 's/^\\//' -e 's/^./\0:/' -e 's/:\+/:/' )
	echo $WINPATH
}
