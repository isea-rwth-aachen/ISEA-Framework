option(BUILD_DOCS "Build documentation with doxygen" OFF)

if(BUILD_DOCS)
  find_package(Doxygen REQUIRED)
  configure_file(Doxyfile.in ${CMAKE_BINARY_DIR}/doc/Doxyfile @ONLY)
  configure_file(Doxyfile_release.in ${CMAKE_BINARY_DIR}/docRelease/Doxyfile @ONLY)
  add_custom_target(
    doc ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc
    COMMENT "Generating API documentation with Doxygen")
  add_custom_target(
    docRelease ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/docRelease/Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/docRelease
    COMMENT "Generating release documentation with Doxygen")
endif(BUILD_DOCS)
