cmake_minimum_required(VERSION 3.3)
project(ErrorLib)

set(ERROR_LANG
    EN
    CACHE STRING "Choose error Language: DE, EN(default)")
set_property(CACHE ERROR_LANG PROPERTY STRINGS EN DE)

function(findLastTag currentIndex maxLength)
  string(SUBSTRING "${xmlData}" ${currentIndex} ${maxLength} sstr)
  string(FIND "${sstr}" ">" c_counter REVERSE)
  math(EXPR c_counter "${c_counter} + 1")
  set(INDEX
      ${c_counter}
      PARENT_SCOPE)
endfunction(findLastTag currentIndex maxLength)

function(ConvertXMLToCString)
  file(READ errors/${ERROR_LANG}.xml xmlData)
  string(REGEX REPLACE "\n\n" "\n" xmlData "${xmlData}")
  string(REGEX REPLACE "\n\n" "\n" xmlData "${xmlData}")
  string(REGEX REPLACE "\n\n" "\n" xmlData "${xmlData}")
  string(REGEX REPLACE "\n\n" "\n" xmlData "${xmlData}")
  string(REGEX REPLACE "  " " " xmlData "${xmlData}")
  string(REGEX REPLACE "\\\\([^;])" "\\\\\\\\\\1" xmlData "${xmlData}")
  string(REPLACE "\"" "\\\"" xmlData "${xmlData}")
  string(REPLACE "\n" " \\\n" xmlData "${xmlData}")
  string(LENGTH "${xmlData}" length)

  file(WRITE ${PROJECT_SOURCE_DIR}/xmlData.cpp "#include <string>\n")

  set(MAX_LENGTH 1024)
  set(STARTINDEX 0)
  set(counter 1)

  while(TRUE)
    math(EXPR tmp_counter "${STARTINDEX} + ${MAX_LENGTH}")
    if(tmp_counter GREATER length)
      set(INDEX 1024)
    else(tmp_counter GREATER length)
      findlasttag(${STARTINDEX} ${MAX_LENGTH})
    endif(tmp_counter GREATER length)

    string(SUBSTRING "${xmlData}" ${STARTINDEX} ${INDEX} substr)

    file(APPEND ${PROJECT_SOURCE_DIR}/xmlData.cpp
         " \n const std::string XML_ERRORS${counter} = \"${substr}\";")
    math(EXPR STARTINDEX "${STARTINDEX} + ${INDEX} + 1")
    math(EXPR counter "${counter} + 1")

    if(STARTINDEX GREATER length OR STARTINDEX EQUAL length)
      break()
    endif(STARTINDEX GREATER length OR STARTINDEX EQUAL length)
  endwhile(TRUE)

  file(APPEND ${PROJECT_SOURCE_DIR}/xmlData.cpp
       " \nstd::string XML_ERRORS = XML_ERRORS1")

  math(EXPR counter "${counter} - 1")
  foreach(loop_var RANGE 2 ${counter} 1)
    file(APPEND ${PROJECT_SOURCE_DIR}/xmlData.cpp " + XML_ERRORS${loop_var}")
  endforeach(loop_var RANGE 2 ${counter} 1)

  file(APPEND ${PROJECT_SOURCE_DIR}/xmlData.cpp ";")

endfunction(ConvertXMLToCString)

convertxmltocstring()
