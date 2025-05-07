cmake_minimum_required(VERSION 3.2)
project(ISEAFrame)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} "${CMAKE_SOURCE_DIR}/cmake/")

cmake_policy(SET CMP0009 NEW)
set(ISEAFrameVERSION_MAJOR 2)
set(ISEAFrameVERSION_MINOR 26)
set(ISEAFramePATCH_LEVEL 1)
set(ISEAFrameNAME "simu_matrix_model_bugfix")
set(ISEAFrameVERSION
    "${ISEAFrameVERSION_MAJOR}.${ISEAFrameVERSION_MINOR}.${ISEAFramePATCH_LEVEL}"
)
set(ARCH_TYPE
    "64"
    CACHE STRING "")

option(BUILD_SYMBOLIC "Build the symbolic version of the framework" OFF)
option(BUILD_NUMERIC "Build the numeric version of the framework" ON)
option(BUILD_AGING "Build the classes required for the aging simulation" ON)
option(USE_DEBUG_GDB "Create everything with debug symbols" OFF)
option(CREATE_RELEASE_DIRS
       "Create directories containing all files needed to run the simulation"
       OFF)
option(BUILD_FOR_RT "Build a version for the realtime system" OFF)
option(USE_BOOST_THREADS "Use multithreading for the thermal model" OFF)
option(USE_EXCEPTIONS "Use exceptions" ON)
option(BUILD_FOR_NATIVE_ARCH "Optimize for current CPU ( -march=native )" OFF)
set(MATIRX_IMPLEMENTATION
    Eigen
    CACHE STRING "Choose matrix implementation")

if(CMAKE_TOOLCHAIN_FILE)
  link_directories(${CMAKE_FIND_ROOT_PATH}/lib)
  include_directories(${CMAKE_FIND_ROOT_PATH}/include)
  # the compile options need to be seperated by semicolons so that they are
  # interpreted as a list
  if(TOOLCHAIN_COMPILE_OPTIONS)
    string(REPLACE " " ";" COMPILE_OPTIONS_LIST ${TOOLCHAIN_COMPILE_OPTIONS})
    add_compile_options(${COMPILE_OPTIONS_LIST})
  endif()
  if(TOOLCHAIN_LINK_OPTIONS)
    string(REPLACE " " ";" LINK_OPTIONS_LIST ${TOOLCHAIN_LINK_OPTIONS})
    add_link_options(${LINK_OPTIONS_LIST})
  endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
  set(WIN_TARGET ON)
else()
  set(LINUX_TARGET ON)
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
  set(WIN_HOST ON)
else()
  set(LINUX_HOST ON)
endif()

if(WIN_HOST)
  # set hints for all find_package calls
  set(FRAMEWORK_EXTENSION_FOLDER
      "C:\\Data\\$ENV{USERNAME}\\FrameworkExtensions"
      CACHE PATH "Folder with required libraries")
  set(CMAKE_FIND_ROOT_PATH ${FRAMEWORK_EXTENSION_FOLDER})
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
  set(BOOST_ROOT ${FRAMEWORK_EXTENSION_FOLDER}/boost)
  include_directories(${FRAMEWORK_EXTENSION_FOLDER}/include)
  link_directories(${FRAMEWORK_EXTENSION_FOLDER}/lib)
  add_compile_options(/bigobj)
  if(${USE_DEBUG_GDB})
    add_compile_options(/DEBUG)
  endif()

  if(USE_SIMULINK)
    set(MATLAB_INCLUDE_DIR
        "C:/Program Files/matlab/R2019b/extern/include"
        CACHE STRING "Path to matlab header files")
    set(SIMULINK_INCLUDE_DIR
        "C:/Program Files/matlab/R2019b/simulink/include"
        CACHE STRING "Path to simulink header files")
    set(MATLAB_LINK_DIR
        "C:/Program Files/matlab/R2019b/extern/lib/win64/microsoft"
        CACHE STRING "Path to matlab library files")

  endif(USE_SIMULINK)
else()
  if(${USE_DEBUG_GDB})
    add_compile_options(-ggdb)
  endif()
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fPIC)
  endif()
  if(${BUILD_FOR_NATIVE_ARCH})
    add_compile_options(-march=native)
  endif()
  if(WIN_TARGET)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      link_libraries("bcrypt")
    endif()
  endif()
endif()

if(BUILD_FOR_RT)
  set(USE_BOOST_THREADS
      OFF
      CACHE BOOL "Use multithreading for the thermal model" FORCE)
  set(USE_EXCEPTIONS
      OFF
      CACHE BOOL "Use exceptions" FORCE)

  set(NUMERIC_DEFINITIONS
      ${NUMERIC_DEFINITIONS}
      -D__KERNEL__
      -D_DS1006
      -DNDEBUG
      -D__RTLIB__
      -DBOOST_NO_TYPEID
      -DBOOST_NO_RTTI
      -DBOOST_NO_EXCEPTIONS
      -DBOOST_EXCEPTION_DISABLE
      -DFALSE=0
      -DTRUE=1
      -DBOOST_UBLAS_UNSUPPORTED_COMPILER=0
      -DBOOST_NO_CXX11_HDR_ARRAY
      -DBOOST_NO_CXX11_HDR_TUPLE
      -DBOOST_NO_CXX11_RVALUE_REFERENCES
      -DBOOST_NO_CXX11_REF_QUALIFIERS
      -DBOOST_HAS_NRVO
      -DBOOST_NO_COMPLETE_VALUE_INITIALIZATION
      -DBOOST_NO_CXX11_AUTO_DECLARATIONS
      -DBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
      -DBOOST_NO_CXX11_CHAR16_T
      -DBOOST_NO_CXX11_CHAR32_T
      -DBOOST_NO_CXX11_CONSTEXPR
      -DBOOST_NO_CXX11_DECLTYPE
      -DBOOST_NO_CXX11_DECLTYPE_N3276
      -DBOOST_NO_CXX11_DEFAULTED_FUNCTIONS
      -DBOOST_NO_CXX11_DELETED_FUNCTIONS
      -DBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
      -DBOOST_NO_CXX11_FINAL
      -DBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
      -DBOOST_NO_CXX11_LAMBDAS
      -DBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
      -DBOOST_NO_CXX11_NOEXCEPT
      -DBOOST_NO_CXX11_RANGE_BASED_FOR
      -DBOOST_NO_CXX11_RAW_LITERALS
      -DBOOST_NO_CXX11_REF_QUALIFIERS
      -DBOOST_NO_CXX11_RVALUE_REFERENCES
      -DBOOST_NO_CXX11_SCOPED_ENUMS
      -DBOOST_NO_CXX11_STATIC_ASSERT
      -DBOOST_NO_CXX11_TEMPLATE_ALIASES
      -DBOOST_NO_CXX11_UNICODE_LITERALS
      -DBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
      -DBOOST_NO_CXX11_USER_DEFINED_LITERALS
      -DBOOST_NO_CXX11_VARIADIC_MACROS
      -DBOOST_NO_CXX11_VARIADIC_TEMPLATES
      -DBOOST_NO_SFINAE_EXPR
      -DBOOST_NO_TWO_PHASE_NAME_LOOKUP
      -D__NO_STRING__)
endif()

if(USE_EXCEPTIONS)
  set(COMMON_DEFINITIONS ${COMMON_DEFINITIONS} __EXCEPTIONS__)
else()
  set(COMMON_DEFINITIONS ${COMMON_DEFINITIONS} BOOST_NO_EXCEPTIONS)
endif(USE_EXCEPTIONS)

if(USE_BOOST_THREADS)
  set(NUMERIC_DEFINITIONS ${NUMERIC_DEFINITIONS} -DBOOST_THREAD
                          -DBOOST_HAS_THREADS)
endif()

set(NUMERIC_DEFINITIONS ${NUMERIC_DEFINITIONS} _EIGEN_)
set(SYMBOLIC_DEFINITIONS ${SYMBOLIC_DEFINITIONS} _SYMBOLIC_)

if(BUILD_AGING)
  set(NUMERIC_DEFINITIONS ${NUMERIC_DEFINITIONS} BUILD_AGING_SIMULATION)
endif()

# the following variables are used by cmake files in subdirectories to specify
# which dependencies are needed
set(NEED_SIMULINK OFF)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED)

set(COMMON_LIBRARIES ${COMMON_LIBRARIES} Boost::boost)

find_package(Threads REQUIRED)
set(COMMON_LIBRARIES ${COMMON_LIBRARIES} Threads::Threads)

set(COMMON_LIBRARIES ${COMMON_LIBRARIES} tinyxml2)

# used by subdirectories to copy DLLs for windows to the binary output dir when
# building the target
function(copy_windows_files_to_dir binary_dir)
  if(WIN_TARGET AND LINUX_HOST)
    configure_file(
      /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/64/libwinpthread-1.dll
      ${CMAKE_BINARY_DIR}/${binary_dir} COPYONLY)
  endif()
endfunction(copy_windows_files_to_dir)

# copy files to the binary folder if CREATE_RELEASE_DIRS is set. target is the
# CMake target and must have RUNTIME_OUTPUT_DIRECTORY set
function(add_release_files target files_to_copy)
  if(CREATE_RELEASE_DIRS)
    add_custom_command(
      TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${files_to_copy}
              $<TARGET_PROPERTY:${target},RUNTIME_OUTPUT_DIRECTORY>)
  endif()
endfunction(add_release_files)

add_subdirectory(libraries/tinyxml2)
add_subdirectory(src/version)
add_subdirectory(src/exceptions)
add_subdirectory(src)
add_subdirectory(standalone)
add_subdirectory(src/export/mex)
add_subdirectory(src/unittests)
add_subdirectory(doxygen)
add_subdirectory(src/cinterface)
