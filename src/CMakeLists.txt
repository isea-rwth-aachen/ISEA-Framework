add_custom_target(framework)

# list of directories inside src that should not be included
set(UNINCLUDE
    ${UNINCLUDE}
    ${PROJECT_SOURCE_DIR}/src/unittest
    ${PROJECT_SOURCE_DIR}/src/simulink_mock/
    ${PROJECT_SOURCE_DIR}/src/thermal/executable
    ${PROJECT_SOURCE_DIR}/src/export/mex
    ${PROJECT_SOURCE_DIR}/src/export/circuit_simulink_model
    ${PROJECT_SOURCE_DIR}/src/export/matrix_simulink_model
    ${PROJECT_SOURCE_DIR}/src/export/CreateSimulinkModelFunction
    ${PROJECT_SOURCE_DIR}/src/export/LoadInitialStateFunction
    ${PROJECT_SOURCE_DIR}/src/export/LoadObjectValuesFunction)

set(NUMERIC_UNINCLUDE ${NUMERIC_UNINCLUDE})

set(SYMBOLIC_UNINCLUDE
    ${SYMBOLIC_UNINCLUDE}
    ${PROJECT_SOURCE_DIR}/src/cinterface/
    ${PROJECT_SOURCE_DIR}/src/export/baseExport
    ${PROJECT_SOURCE_DIR}/src/export/electrical_simulink_model
    ${PROJECT_SOURCE_DIR}/src/export/esbVisualizer
    ${PROJECT_SOURCE_DIR}/src/export/spiceExport
    ${PROJECT_SOURCE_DIR}/src/factory/electricalEquationSystem/chooseElectricSystemType
    ${PROJECT_SOURCE_DIR}/src/system/constantstepdglsystemsolver
    ${PROJECT_SOURCE_DIR}/src/system/linAlg_sys
    ${PROJECT_SOURCE_DIR}/src/system/linearsystemsolver
    ${PROJECT_SOURCE_DIR}/src/system/system
    ${PROJECT_SOURCE_DIR}/src/system/systemSolver
    ${PROJECT_SOURCE_DIR}/src/system/variablestepdglsystemsolver
    ${PROJECT_SOURCE_DIR}/src/thermal/block_observer
    ${PROJECT_SOURCE_DIR}/src/observer/)

# automatically find all needed source and header files
file(GLOB_RECURSE TMP_SOURCE *.cpp)
foreach(f ${TMP_SOURCE})
  # if part of uninclude then drop it
  foreach(g ${UNINCLUDE})
    string(FIND ${f} ${g} tmp_found REVERSE)
    if(NOT ${tmp_found} EQUAL -1)
      break()
    endif(NOT ${tmp_found} EQUAL -1)
  endforeach(g ${UNINCLUDE})

  # else add it to sources
  if(${tmp_found} EQUAL -1)

    # check if part of numeric uninclude
    foreach(g ${NUMERIC_UNINCLUDE})
      string(FIND ${f} ${g} tmp_found REVERSE)
      if(NOT ${tmp_found} EQUAL -1)
        break()
      endif(NOT ${tmp_found} EQUAL -1)
    endforeach(g ${NUMERIC_UNINCLUDE})

    # else add to numeric sources
    if(${tmp_found} EQUAL -1)
      set(NUMERIC_SOURCE ${NUMERIC_SOURCE} ${f})
    endif(${tmp_found} EQUAL -1)

    # check if part of symbolic uninclude
    foreach(g ${SYMBOLIC_UNINCLUDE})
      string(FIND ${f} ${g} tmp_found REVERSE)
      if(NOT ${tmp_found} EQUAL -1)
        break()
      endif(NOT ${tmp_found} EQUAL -1)
    endforeach(g ${SYMBOLIC_UNINCLUDE})

    # else add to symbolic sources
    if(${tmp_found} EQUAL -1)
      set(SYMBOLIC_SOURCE ${SYMBOLIC_SOURCE} ${f})
    endif(${tmp_found} EQUAL -1)

  endif(${tmp_found} EQUAL -1)
endforeach(f ${TMP_SOURCE})

file(GLOB_RECURSE HEADERS *.h)
foreach(f ${TMP_SOURCE})

  # if part of uninclude then drop it
  foreach(g ${UNINCLUDE})

    string(FIND ${f} ${g} tmp_found REVERSE)
    if(NOT ${tmp_found} EQUAL -1)
      break()
    endif(NOT ${tmp_found} EQUAL -1)

  endforeach(g ${UNINCLUDE})

  # else add it to headers
  if(${tmp_found} EQUAL -1)

    # check if part of numeric uninclude
    foreach(g ${NUMERIC_UNINCLUDE})
      string(FIND ${f} ${g} tmp_found REVERSE)
      if(NOT ${tmp_found} EQUAL -1)
        break()
      endif(NOT ${tmp_found} EQUAL -1)
    endforeach(g ${NUMERIC_UNINCLUDE})

    # else add to numeric headers
    if(${tmp_found} EQUAL -1)
      set(NUMERIC_HEADERS ${NUMERIC_HEADERS} ${f})
    endif(${tmp_found} EQUAL -1)

    # check if part of symbolic uninclude
    foreach(g ${SYMBOLIC_UNINCLUDE})
      string(FIND ${f} ${g} tmp_found REVERSE)
      if(NOT ${tmp_found} EQUAL -1)
        break()
      endif(NOT ${tmp_found} EQUAL -1)
    endforeach(g ${SYMBOLIC_UNINCLUDE})

    # else add to symbolic headers
    if(${tmp_found} EQUAL -1)
      set(SYMBOLIC_HEADERS ${SYMBOLIC_HEADERS} ${f})
    endif(${tmp_found} EQUAL -1)

  endif(${tmp_found} EQUAL -1)
endforeach(f ${TMP_SOURCE})

if(BUILD_NUMERIC)
  set(libname_NUMERIC ISEAFrameNumeric)
  add_library(${libname_NUMERIC} ${NUMERIC_SOURCE} ${NUMERIC_HEADERS})
  target_link_libraries(${libname_NUMERIC} ${COMMON_LIBRARIES}
                        ${NUMERIC_LIBRARIES})
  target_compile_definitions(${libname_NUMERIC} PUBLIC ${COMMON_DEFINITIONS}
                             ${NUMERIC_DEFINITIONS})
  add_dependencies(framework ${libname_NUMERIC})
endif()

if(BUILD_SYMBOLIC)
  set(libname_SYMBOLIC ISEAFrameSymbolic)
  add_library(${libname_SYMBOLIC} ${SYMBOLIC_SOURCE} ${SYMBOLIC_HEADERS})
  target_link_libraries(${libname_SYMBOLIC} ${COMMON_LIBRARIES}
                        ${SYMBOLIC_LIBRARIES})
  target_compile_definitions(${libname_SYMBOLIC} PUBLIC ${COMMON_DEFINITIONS}
                             ${SYMBOLIC_DEFINITIONS})
  add_dependencies(framework ${libname_SYMBOLIC})
endif()
