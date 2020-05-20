option(BUILD_UNITTESTS "Build all unittests" OFF)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/unittest)

if(BUILD_UNITTESTS)
  find_package(CxxTest)
  add_custom_target(unittests)

  # find all header files to pass to the test generator
  file(GLOB_RECURSE UNITTEST_HEADERS *.h)

  # tell cmake how to generate the unittests
  add_custom_command(
    OUTPUT unittestNumeric.cpp
    COMMAND cxxtestgen --error-printer -o unittestNumeric.cpp
            ${UNITTEST_HEADERS} COMMAND_EXPAND_LISTS)

  # find all source files to compile for the unittests
  file(GLOB_RECURSE UNITTEST_SOURCES *.cpp)

  if(BUILD_NUMERIC)
    add_executable(unittestNumeric unittestNumeric.cpp ${UNITTEST_HEADERS}
                                   ${UNITTEST_SOURCES})
    target_link_libraries(unittestNumeric ISEAFrameNumeric ${COMMON_LIBRARIES}
                          ${NUMERIC_LIBRARIES} ${BOOST_TARGETS})
    # _CXXTEST_HAVE_EH needs to be defined for some unknown reason
    target_compile_definitions(unittestNumeric PUBLIC ${COMMON_DEFINITIONS}
                               ${NUMERIC_DEFINITIONS} _CXXTEST_HAVE_EH)
    set_target_properties(
      unittestNumeric PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                 ${CMAKE_BINARY_DIR}/unittest)
    add_dependencies(unittests unittestNumeric)
  endif(BUILD_NUMERIC)

  if(BUILD_SYMBOLIC)
    # exclude tests that only work with the numeric version
    set(SYMBOLIC_UNINCLUDE
        cinterface/
        export/
        misc/TestFastCopyMatrix
        observer/
        states/TestSurfaceSoc
        system/TestChoiceOfSystem
        system/TestDaeSystem
        system/TestLinearSystem
        thermal/TestSimulation
        unittestNumeric)

    foreach(f ${UNITTEST_SOURCES})
      foreach(g ${SYMBOLIC_UNINCLUDE})
        string(FIND ${f} ${g} tmp_found REVERSE)
        if(NOT ${tmp_found} EQUAL -1)
          break()
        endif(NOT ${tmp_found} EQUAL -1)
      endforeach(g ${UNINCLUDE})
      if(${tmp_found} EQUAL -1)
        set(UNITTEST_SYMBOLIC_SOURCES ${UNITTEST_SYMBOLIC_SOURCES} ${f})
      endif(${tmp_found} EQUAL -1)
    endforeach(f ${UNITTEST_SOURCES})

    foreach(f ${UNITTEST_HEADERS})
      foreach(g ${SYMBOLIC_UNINCLUDE})
        string(FIND ${f} ${g} tmp_found REVERSE)
        if(NOT ${tmp_found} EQUAL -1)
          break()
        endif(NOT ${tmp_found} EQUAL -1)
      endforeach(g ${UNINCLUDE})
      if(${tmp_found} EQUAL -1)
        set(UNITTEST_SYMBOLIC_HEADERS ${UNITTEST_SYMBOLIC_HEADERS} ${f})
      endif(${tmp_found} EQUAL -1)
    endforeach(f ${UNITTEST_HEADERS})

    # tell cmake how to generate the unittests
    add_custom_command(
      OUTPUT unittestSymbolic.cpp
      COMMAND cxxtestgen --error-printer -o unittestSymbolic.cpp
              ${UNITTEST_SYMBOLIC_HEADERS} COMMAND_EXPAND_LISTS)

    add_executable(
      unittestSymbolic unittestSymbolic.cpp ${UNITTEST_SYMBOLIC_HEADERS}
                       ${UNITTEST_SYMBOLIC_SOURCES})
    target_link_libraries(
      unittestSymbolic ISEAFrameSymbolic ${COMMON_LIBRARIES}
      ${SYMBOLIC_LIBRARIES} ${BOOST_TARGETS})
    # _CXXTEST_HAVE_EH needs to be defined for some unknown reason
    target_compile_definitions(unittestSymbolic PUBLIC ${COMMON_DEFINITIONS}
                               ${SYMBOLIC_DEFINITIONS} _CXXTEST_HAVE_EH)
    set_target_properties(
      unittestSymbolic PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                  ${CMAKE_BINARY_DIR}/unittest)
    add_dependencies(unittests unittestSymbolic)
  endif(BUILD_SYMBOLIC)

  # copy XML files needed for the unittests to the build dir
  set(FILES_TO_COPY
      xml/testconfig.xml
      thermal/Patch_Vertices_Save.csv
      thermal/Patch_Areas_Save.csv
      thermal/Patch_Temperatures_Save.csv
      thermal/TestPouchBag.xml
      thermal/TestPouchBagDisc.xml
      thermal/TestShowBattery.xml
      thermal/TestRectangularblock.xml
      cinterface/TestSimpleBlockRCBattery.xml
      factory/thermal/TestThermalFactories.xml
      xml/TestXmlParser.xml
      misc/TestFindSimulationTypeElectrical.xml
      misc/TestFindSimulationTypeThermal.xml
      misc/TestFindSimulationTypeThEl.xml
      misc/TestFindSimulationTypeAging.xml
      container/structWithContent.mat
      container/structWith2D.mat
      container/emptyStructs.mat)

  file(COPY ${FILES_TO_COPY} DESTINATION ${CMAKE_BINARY_DIR}/unittest)

endif(BUILD_UNITTESTS)
