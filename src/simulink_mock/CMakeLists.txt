cmake_minimum_required(VERSION 3.0)
project(SimulinkMock)

option(BUILD_SIMULINK_MOCK "Build the mock simulink interface" OFF)

if(BUILD_SIMULINK_MOCK)
  find_package(matio REQUIRED)
  file(GLOB SOURCE *.cpp) # RELATIVE ${PROJECT_SOURCE_DIR}/src/*.cpp)
  add_library(Simumock ${SOURCE})
  target_link_libraries(Simumock PUBLIC matio::matio)
endif(BUILD_SIMULINK_MOCK)
