# BASED ON https://github.com/cvjena/nice-core/blob/master/cmake/FindMATIO.cmake
# Find the MATIO headers and library.
#
# MATIO_INCLUDE_DIRS - where to find matio.h, etc. MATIO_LIBRARIES    - List of
# libraries. MATIO_FOUND        - True if matio found.

find_path(
  MATIO_INCLUDE_DIR
  NAMES matio.h
  PATHS ${FRAMEWORK_EXTENSION_FOLDER}/include/
        ${FRAMEWORK_EXTENSION_FOLDER}/include/matio $ENV{HOME}/include)
find_library(
  MATIO_LIBRARY
  NAMES matio${ARCH_TYPE} libmatio${ARCH_TYPE} matio libmatio
  PATHS ${FRAMEWORK_EXTENSION_FOLDER}/lib/ $ENV{HOME}/lib)

mark_as_advanced(MATIO_INCLUDE_DIR)
mark_as_advanced(MATIO_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set MATIO_FOUND to TRUE if all
# listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MATIO DEFAULT_MSG MATIO_LIBRARY
                                  MATIO_INCLUDE_DIR)

if(MATIO_FOUND)
  set(MATIO_LIBRARIES ${MATIO_LIBRARY} ${HDF5_LIBRARIES})
  set(MATIO_INCLUDE_DIRS ${MATIO_INCLUDE_DIR})
  add_library(matio::matio UNKNOWN IMPORTED)
  set_target_properties(
    matio::matio
    PROPERTIES IMPORTED_LOCATION "${MATIO_LIBRARY}"
               INTERFACE_INCLUDE_DIRECTORIES "${MATIO_INCLUDE_DIRS}")
else(MATIO_FOUND)
  set(MATIO_LIBRARIES)
  set(MATIO_INCLUDE_DIRS)
endif(MATIO_FOUND)
