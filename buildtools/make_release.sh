#!/bin/bash -e

##### Messages,  warnings and errors #####
function PrintErrorMessage
{
    >&2 echo -en "\033[1;31mERROR: $1\033[0m\n"
}

function PrintWarningMessage
{
    >&2 echo -en "\033[1;31mWarning: $1\033[0m\n"
}

function PrintMessage
{
    echo -en "\033[1;32m$1\033[0m\n"
}

function PrintHelp
{
    PrintMessage "make_release.sh [flags] ROOT_DIRECTORY"
    PrintMessage "The script uses argument 1 as the root directory for the framework"
    PrintMessage "Command line options:"
    PrintMessage "-h\t\tShow this message and exit."
    PrintMessage "-w\t\tIf on linux, build a release for windows."
    PrintMessage "-b <builddir>\t\tSpecify the build dir. Default is \"build\""
    PrintMessage "-c\t\tOnly configure, do not start the build process."
}

function RunCMake
{
    WIN_HOST=$1
    WIN_TARGET=$2
    CMAKE_OPTIONS=""
    if [ $WIN_HOST -eq 0 ]
    then
        if [ $WIN_TARGET -eq 1 ] # crosscompile
        then
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_TOOLCHAIN_FILE=../toolchain_files/ToolchainWin64.cmake"
        fi
    fi
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_BUILD_TYPE:STRING=Release -DARCH_TYPE:STRING=64 -DBUILD_AGING:BOOL=ON -DBUILD_NUMERIC:BOOL=ON -DBUILD_SYMBOLIC:BOOL=ON -DBUILD_UNITTESTS:BOOL=ON -DBUILD_DOCS:BOOL=ON -DCREATE_RELEASE_DIRS:BOOL=ON"
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_VISUALIZER:BOOL=ON -DBUILD_DOT_EXPORT:BOOL=ON -DBUILD_QUICKVERIFICATION:BOOL=ON -DBUILD_SVG_EXPORT:BOOL=ON -DBUILD_ELECTRICAL_SIMULATION:BOOL=ON -DBUILD_THERMAL_SIMULATION:BOOL=ON -DBUILD_THERMAL_ELECTRICAL_SIMULATION:BOOL=ON -DBUILD_AGING_SIMULATION:BOOL=ON -DBUILD_GETTOTALCAPACITY:BOOL=ON"
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_EIGENWERTE:BOOL=ON -DBUILD_SYMBOLICOPTIMIZATION:BOOL=ON -DBUILD_SYMBOLIC_2_TEX:BOOL=ON -DBUILD_SYSTEM_2_TEX:BOOL=ON -DBUILD_SYSTEM_2_MATLAB:BOOL=ON -DBUILD_SYSTEM_2_SYMBOLIC:BOOL=ON"
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_S_FUNCTIONS:BOOL=ON -DBUILD_SIMULINK_MATRIX_MODEL:BOOL=ON"

    cmake $CMAKE_OPTIONS ..
    cmake $CMAKE_OPTIONS ..
}

function IsWindowsSystem
{
    # check if we have a windows command
    type wmic >/dev/null 2>&1 ||
    {
        echo 0
        return
    }
    echo 1
}

function main
{
    WIN_TARGET=0
    ONLY_CONFIGURE=0
    BUILDDIR=build
    while getopts "hwb:c" opt; do
        case "$opt" in
        h)  PrintHelp
            exit 0
            ;;
        w)  WIN_TARGET=1
            ;;
        b)  BUILDDIR=$OPTARG
            ;;
        c)  ONLY_CONFIGURE=1
            ;;
        esac
    done

    shift $((OPTIND-1))

    if [ -z $1 ]
    then
        PrintErrorMessage "Missing required argument."
        PrintHelp
        exit 1
    fi

    FRAME_ROOT_DIR=$1
    cd $FRAME_ROOT_DIR
    echo `pwd`

    if [ $(IsWindowsSystem) -eq 1 ]
    then
        WIN_HOST=1
        WIN_TARGET=1
    else
        WIN_HOST=0
    fi

	source ./buildtools/buildOptions.sh
    if [ ! -d $BUILDDIR ]
    then
        mkdir $BUILDDIR
    fi

    cd $BUILDDIR
    RunCMake $WIN_HOST $WIN_TARGET
    cd ..

    if [ $ONLY_CONFIGURE -eq 0 ]
    then
        if [ $WIN_HOST -eq 1 ]
        then
            cmake --build $BUILDDIR --target all
        else
            cmake --build $BUILDDIR --target all -- -j 20
        fi
    fi
}

main $@
