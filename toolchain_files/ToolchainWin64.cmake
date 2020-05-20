set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR "64")
set(ARCH_TYPE
    "64"
    CACHE STRING "")

# which compilers to use for C and C++
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc-posix)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-posix)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# some variables are apparently not propagated from the toolchain file, but the
# cache should always work
set(TOOLCHAIN_COMPILE_OPTIONS
    "-O2 -Wa,-mbig-obj -static -static-libgcc -static-libstdc++ -lgcc -lstdc++ -Wl,-Bstatic -lpthread"
    CACHE STRING "Compile options specified by the toolchain file")
set(TOOLCHAIN_LINK_OPTIONS
    "-Bstatic -static-libgcc -static-libstdc++"
    CACHE STRING "Compile options specified by the toolchain file")

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32/FrameworkExtensions)

set(MATLAB_INCLUDE_DIR
    /usr/x86_64-w64-mingw32/matlab/include
    CACHE STRING "Path to matlab header files")
set(SIMULINK_INCLUDE_DIR
    ${MATLAB_INCLUDE_DIR}
    CACHE STRING "Path to simulink header files")
set(MATLAB_LINK_DIR
    /usr/x86_64-w64-mingw32/matlab/lib
    CACHE STRING "Path to matlab library files")

# adjust the default behaviour of the FIND_XXX() commands: search headers and
# libraries in the target environment, search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
