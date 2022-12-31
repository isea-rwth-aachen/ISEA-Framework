set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)

set(CMAKE_RC_COMPILER llvm-rc-13)

set(CMAKE_C_FLAGS "-Wno-unused-command-line-argument -fuse-ld=lld-link /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/crt/include /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/include/ucrt /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/include/um /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/include/shared")
set(CMAKE_CXX_FLAGS "-Wno-unused-command-line-argument -fuse-ld=lld-link /GR /EHsc /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/crt/include /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/include/ucrt /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/include/um /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/include/shared")
set(CMAKE_RC_FLAGS "-Wno-unused-command-line-argument -fuse-ld=lld-link /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/crt/include /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/include/ucrt /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/include/um /imsvc/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/include/shared")

set(CMAKE_EXE_LINKER_FLAGS "/libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/lib/um/x86_64 /libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/crt/lib/x86_64 /libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/lib/ucrt/x86_64")
set(CMAKE_SHARED_LINKER_FLAGS "/libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/lib/um/x86_64 /libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/crt/lib/x86_64 /libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/lib/ucrt/x86_64")
set(CMAKE_MODULE_LINKER_FLAGS "/libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/lib/um/x86_64 /libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/crt/lib/x86_64 /libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/lib/ucrt/x86_64")
set(CMAKE_STATIC_LINKER_FLAGS "/libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/lib/um/x86_64 /libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/crt/lib/x86_64 /libpath:/usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/sdk/lib/ucrt/x86_64")

set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-w64-mingw32/FrameworkExtensions")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(Matlab_INCLUDE_DIRS
    /usr/x86_64-w64-mingw32/matlab/include
    CACHE STRING "Path to matlab header files")
file(GLOB LIBRARIES /usr/x86_64-w64-mingw32/matlab/lib/*.lib)
set(Matlab_LIBRARIES
    ${LIBRARIES}
    CACHE STRING "Path to matlab library files")

set(TOOLCHAIN_COMPILE_OPTIONS "" CACHE STRING "Compile options specified by the toolchain file")
set(TOOLCHAIN_LINK_OPTIONS "" CACHE STRING "Compile options specified by the toolchain file")

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)