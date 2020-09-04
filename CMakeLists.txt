cmake_minimum_required(VERSION 3.0.0)
project(cpphots)

set(CMAKE_CXX_STANDARD 17)

# versioning
set(CPPHOTS_MAJOR_VERSION 1)
set(CPPHOTS_MINOR_VERSION 0)
set(CPPHOTS_PATCH_VERSION 1)
set(CPPHOTS_VERSION ${CPPHOTS_MAJOR_VERSION}.${CPPHOTS_MINOR_VERSION}.${CPPHOTS_PATCH_VERSION})

# installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/cpphots)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()


# main lib
add_subdirectory(src)


# docs
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND AND BUILD_DOC)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
elseif (BUILD_DOC)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND AND BUILD_DOC)


# packaging

# add all targets to the build-tree export set
export(TARGETS cpphots
  FILE "${PROJECT_BINARY_DIR}/cpphotsTargets.cmake")

# export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE cpphots)

# create the cpphotsConfig.cmake file
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
configure_file(cmake/cpphotsConfig.cmake.in "${PROJECT_BINARY_DIR}/cpphotsConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${CPPHOTS_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(cmake/cpphotsConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cpphotsConfig.cmake" @ONLY)

# create cpphotsConfigVersion.cmake file
configure_file(cmake/cpphotsConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/cpphotsConfigVersion.cmake" @ONLY)

# install the cpphotsConfig.cmake and cpphotsConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cpphotsConfig.cmake"
  "${PROJECT_BINARY_DIR}/cpphotsConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# install the export set for use with the install-tree
install(EXPORT cpphotsTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)


# tests
option(BUILD_TEST "Build tests" ON)
if (BUILD_TEST)
    enable_testing()

    add_subdirectory(tests)
endif (BUILD_TEST)


# examples
option(BUILD_EXAMPLES "Build examples" ON)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)