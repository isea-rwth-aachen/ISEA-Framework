# target for building all standalones
add_custom_target(standalones)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/standalones)

# copy DLLs for windows to the binary output dir when building the target
if(WIN_TARGET AND LINUX_HOST)
  file(GLOB LIBRARIES /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/64/*.dll)
  foreach(lib ${LIBRARIES})
    configure_file(${lib} ${CMAKE_BINARY_DIR}/standalones COPYONLY)
  endforeach()
endif()

find_package(MATIO REQUIRED)

file(GLOB_RECURSE SOURCEFILES standalone/*.cpp)
add_library(ISEAFrameStandalone STATIC EXCLUDE_FROM_ALL ${SOURCEFILES})
target_link_libraries(ISEAFrameStandalone ISEAFrameNumeric ${BOOST_TARGETS}
                      matio::matio)

if(BUILD_NUMERIC)
  option(BUILD_VISUALIZER "Build a quick visualizer executable" OFF)
  option(BUILD_DOT_EXPORT "Build Dot-Export" OFF)
  option(BUILD_QUICKVERIFICATION
         "Build a quick test for electrical and thermal simulation" OFF)
  option(BUILD_SVG_EXPORT "Build Standalone SVG export" OFF)
  option(BUILD_ELECTRICAL_SIMULATION
         "Build commandline tool for electrical Simulation" OFF)
  option(BUILD_THERMAL_SIMULATION
         "Builds a commandline tool for thermal only simulation" OFF)
  option(BUILD_THERMAL_ELECTRICAL_SIMULATION
         "Build a commandline tool for thermal-electrical simulation" OFF)
  if(BUILD_AGING)
    option(BUILD_AGING_SIMULATION
           "Builds a commandline tool for aging simulation" OFF)
  endif()
  option(BUILD_GETTOTALCAPACITY
         "Build a quick test for checking the total capacity of an ECM" OFF)
endif()

if(BUILD_SYMBOLIC)

  option(BUILD_EIGENWERTE "Build Standalone Eigen Values export" OFF)
  option(BUILD_SYMBOLICOPTIMIZATION
         "Build a commandline tool for symbolic optimization" OFF)
  option(BUILD_SYMBOLIC_2_TEX
         "Build a converter from symbolic representation to tex" OFF)
  option(BUILD_SYSTEM_2_TEX "Build a converter from XML representation to tex"
         OFF)
  option(BUILD_SYSTEM_2_MATLAB
         "Build a converter from XML representation to matlab" OFF)
  option(BUILD_SYSTEM_2_SYMBOLIC
         "Build a converter from XML representation to symbolic representation"
         OFF)

  if(BUILD_SYSTEM_2_TEX
     OR BUILD_SYSTEM_2_MATLAB
     OR BUILD_SYSTEM_2_SYMBOLIC)
    find_package(Boost REQUIRED COMPONENTS system thread log)
    set(BOOST_TARGETS Boost::system Boost::thread Boost::log)
  endif()

endif()

function(create_numeric_standalone condition standalone_name sourcefile)
  if(${condition})
    add_executable(${standalone_name} ${sourcefile} standalone/standalone.cpp)
    target_link_libraries(${standalone_name} ISEAFrameStandalone)
    set_target_properties(
      ${standalone_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                    ${CMAKE_BINARY_DIR}/standalones)
    add_dependencies(standalones ${standalone_name})
  else()
    file(REMOVE
         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/standalones/${standalone_name}")
  endif()
endfunction(create_numeric_standalone)

function(create_symbolic_standalone condition standalone_name sourcefile
         target_libraries)
  if(${condition})
    add_executable(${standalone_name} ${sourcefile} standalone/standalone.cpp)
    target_link_libraries(${standalone_name} ISEAFrameSymbolic
                          ${target_libraries})
    set_target_properties(
      ${standalone_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                    ${CMAKE_BINARY_DIR}/standalones)
    add_dependencies(standalones ${standalone_name})
  else()
    file(REMOVE "${CMAKE_BINARY_DIR}/standalones/${standalone_name}")
  endif()
endfunction(create_symbolic_standalone)

create_numeric_standalone(BUILD_VISUALIZER QuickVisualize QuickVisualizer.cpp)
create_numeric_standalone(BUILD_SVG_EXPORT SVGExport svgExport.cpp)
create_numeric_standalone(BUILD_DOT_EXPORT DotExport dotExport.cpp)
create_numeric_standalone(BUILD_ELECTRICAL_SIMULATION electricalSimulation
                          electricalSimulation.cpp)
create_numeric_standalone(BUILD_THERMAL_SIMULATION thermalSimulation
                          thermalSimulation.cpp)
create_numeric_standalone(
  BUILD_THERMAL_ELECTRICAL_SIMULATION thermalElectricalSimulation
  thermalElectricalSimulation.cpp)
create_numeric_standalone(BUILD_AGING_SIMULATION agingSimulation
                          agingSimulation.cpp)
create_numeric_standalone(BUILD_QUICKVERIFICATION verification verification.cpp)
create_numeric_standalone(BUILD_GETTOTALCAPACITY totalCapacity
                          totalCapacity.cpp)

create_symbolic_standalone(BUILD_EIGENWERTE eigenValues eigenValues.cpp "")
create_symbolic_standalone(BUILD_SYMBOLICOPTIMIZATION symbolicOptimization
                           symbolicOptimization.cpp "")
create_symbolic_standalone(BUILD_SYMBOLIC_2_TEX symbolic2Tex symbolic2Tex.cpp
                           "")
create_symbolic_standalone(BUILD_SYSTEM_2_TEX system2Tex system2Tex.cpp
                           "${BOOST_TARGETS}")
create_symbolic_standalone(BUILD_SYSTEM_2_MATLAB system2Matlab
                           system2Matlab.cpp "${BOOST_TARGETS}")
create_symbolic_standalone(BUILD_SYSTEM_2_SYMBOLIC system2Symbolic
                           system2Symbolic.cpp "${BOOST_TARGETS}")
