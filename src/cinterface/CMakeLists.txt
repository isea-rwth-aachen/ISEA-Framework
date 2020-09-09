file(GLOB_RECURSE SOURCEFILES *.cpp)

find_package(MATIO REQUIRED)

add_library(cinterface ${SOURCEFILES})
target_link_libraries(cinterface PUBLIC ISEAFrameNumeric matio::matio)
target_compile_definitions(cinterface PUBLIC ${COMMON_DEFINITIONS}
                                             ${NUMERIC_DEFINITIONS})
