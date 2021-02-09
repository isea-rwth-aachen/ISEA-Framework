# set(REPO_VERSION "unknown")
find_package(Git)
if(GIT_FOUND)
  if(EXISTS ${PROJECT_SOURCE_DIR}/.git)
    message("${GIT_EXECUTABLE}")
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      OUTPUT_VARIABLE REPO_VERSION
      RESULT_VARIABLE RES)
    if(RES)
      message(FATAL_ERROR "Git command failed with\n${REPO_VERSION}")
    endif(RES)
    string(STRIP ${REPO_VERSION} REPO_VERSION)
  endif(EXISTS ${PROJECT_SOURCE_DIR}/.git)
endif(GIT_FOUND)

function(write_to_version)
  file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/version.h
       "#ifndef _VERSION_\n#define _VERSION_\n")
  file(
    WRITE ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
    "const char * VERSION_STRING = \"VERSION ${ISEAFrameVERSION_MAJOR}.${ISEAFrameVERSION_MINOR}.${ISEAFramePATCH_LEVEL} ${ISEAFrameNAME} GIT ${REPO_VERSION}\";"
  )
  file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/version.h
       "\nextern const char * VERSION_STRING;")
  set(UNINCLUDE
      "OUTPUT"
      "CIFS"
      "DOXYGEN"
      "UNINCLUDE"
      "MY_INFO"
      "SG_Filter"
      "SOURCE"
      "SUBDIRECTORIES"
      "Subversion"
      "unittest"
      "CXXTEST"
      "HEADERS"
      "SOURCE"
      "_boost_"
      "ISEAFrame_BINARY_DIR"
      "FIND_PACKAGE_MESSAGE_DETAILS_CxxTest"
      "FIND_PACKAGE_MESSAGE_DETAILS_Doxygen"
      "ISEAFrame_BINARY_DIR"
      "PROJECT_BINARY_DIR"
      "Boost_ERROR_REASON"
      "ARGC"
      "ARGN"
      "ARGV"
      "CMAKE_MATCH_0"
      "CMAKE_SYSTEM_PREFIX_PATH"
      "WIN32"
      "_Boost_LIB_VERSION_REGEX"
      "_Doxygen_GRAPHVIZ_BIN_DIRS"
      "CMAKE_CXX_COMPILER_ID_PLATFORM_CONTENT")
  get_cmake_property(_variableNames VARIABLES)
  list(REMOVE_DUPLICATES _variableNames)
  foreach(_variableName ${_variableNames})
    if(_variableName MATCHES "^(CMAKE_|_)")
      continue()
    endif(_variableName MATCHES "^(CMAKE_|_)")
    foreach(g ${UNINCLUDE})
      string(FIND ${_variableName} ${g} tmp_found REVERSE)
      if(NOT ${tmp_found} EQUAL -1)
        break()
      endif(NOT ${tmp_found} EQUAL -1)
    endforeach(g ${UNINCLUDE})

    if(${tmp_found} EQUAL -1)
      string(REPLACE "\"" "\\\"" c_value "${${_variableName}}")
      file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
           "\nconst char * CMAKE_${_variableName} = \"${c_value}\";")
      file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/version.h
           "\nextern const char * CMAKE_${_variableName};")
    endif(${tmp_found} EQUAL -1)
  endforeach()

  file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/version.h "\n#endif")
endfunction(write_to_version)

write_to_version()

file(GLOB SOURCE *.cpp)
set(COMPILE_FEATURES cxx_alias_templates cxx_auto_type)

set(SOURCE ${SOURCE})

if(MSVC)
  add_library(libVersion ${SOURCE})
  set_target_properties(libVersion PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
                                              "${CMAKE_BINARY_DIR}")
  target_compile_features(libVersion PRIVATE ${COMPILE_FEATURES})
elseif(UNIX)
  add_library(Version ${SOURCE})
  set_target_properties(Version PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
                                           "${CMAKE_BINARY_DIR}")
  target_compile_features(Version PRIVATE ${COMPILE_FEATURES})
else()
  add_library(Version ${SOURCE})
  set_target_properties(Version PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
                                           "${CMAKE_BINARY_DIR}")
  target_compile_features(Version PRIVATE ${COMPILE_FEATURES})
endif(MSVC)
