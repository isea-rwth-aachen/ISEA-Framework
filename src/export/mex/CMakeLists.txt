add_custom_target(simulinkConverter)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/simulinkConverter)
set_target_properties(
  simulinkConverter PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                               ${CMAKE_BINARY_DIR}/simulinkConverter)
copy_windows_files_to_dir(simulinkConverter)
add_release_files(simulinkConverter ${CMAKE_SOURCE_DIR}/simulink_model_creator)

if(BUILD_SYMBOLIC)
  option(BUILD_SIMULINK_MATRIX_MODEL
         "Build mex files to create a matrix model in Simulink" OFF)
endif()

if(LINUX_TARGET)
  set(MEX_SUFFIX ".mexa64")
elseif(WIN_TARGET)
  set(MEX_SUFFIX ".mexw64")
endif()

if(BUILD_SIMULINK_MATRIX_MODEL)
  if(NOT Matlab_LIBRARIES)
    find_package(Matlab REQUIRED COMPONENTS MAT_LIBRARY)
  endif()

  function(create_mex mex_name sourcefile)
    add_library(${mex_name} SHARED ${sourcefile} simulink_export_symbols.def)
    target_link_libraries(${mex_name} ISEAFrameSymbolic ${Matlab_LIBRARIES})
    target_include_directories(${mex_name} PUBLIC ${Matlab_INCLUDE_DIRS})
    set_target_properties(
      ${mex_name}
      PROPERTIES PREFIX "" SUFFIX ${MEX_SUFFIX}
                 RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/simulinkConverter
                 LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/simulinkConverter)

    add_dependencies(simulinkConverter ${mex_name})
  endfunction(create_mex)

  create_mex(CreateSimulinkModel
             "CreateSimulinkModel.cpp;../CreateSimulinkModelFunction.cpp")
  create_mex(LoadInitialState
             "LoadInitialState.cpp;../LoadInitialStateFunction.cpp")
  create_mex(LoadObjectValues
             "LoadObjectValues.cpp;../LoadObjectValuesFunction.cpp")

endif(BUILD_SIMULINK_MATRIX_MODEL)
