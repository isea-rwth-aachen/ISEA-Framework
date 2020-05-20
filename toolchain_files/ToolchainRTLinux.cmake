SET(CMAKE_SYSTEM_NAME Generic)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER   "C:/Program Files (x86)/dSPACE RCPHIL 2014-B/Compiler/x86Tools/bin/i686-elf-gcc.exe")
SET(CMAKE_CXX_COMPILER "C:/Program Files (x86)/dSPACE RCPHIL 2014-B/Compiler/x86Tools/bin/i686-elf-g++.exe")

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH "C:/Program Files (x86)/dSPACE RCPHIL 2014-B/Compiler/x86Tools" )

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
