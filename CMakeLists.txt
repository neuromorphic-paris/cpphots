cmake_minimum_required(VERSION 3.11...3.18)

project(cpphots VERSION 0.6.0
                LANGUAGES CXX)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# default to Release
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# default to shared library
option(BUILD_SHARED_LIBS "Build shared lib" ON)

# installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
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

# option for time surface plots
option(BUILD_PLOTS "Build plots utilities" ON)

# option for precision
option(DOUBLE_PRECISION "Enable double precision" OFF)

# option for asserts, enabled by default in debug mode
option(ENABLE_ASSERTS "Enable asserts" OFF)
message(STATUS ${CMAKE_BUILD_TYPE})
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(ENABLE_ASSERTS ON)
endif()

# option for peregrine
option(WITH_PEREGRINE "Enable GMM support" OFF)

# dependencies

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CPPHOTS_OPTIONAL_DEPS "")

# sepia
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    sepia
    GIT_REPOSITORY https://github.com/neuromorphic-paris/sepia
)
FetchContent_GetProperties(sepia) # these lines should be replaced with FetchContent_MakeAvailable(sepia), but is available only in >= 3.14
if(NOT sepia_POPULATED)
  FetchContent_Populate(sepia)
endif()

# Eigen
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# Peregrine dependencies
if (WITH_PEREGRINE)
  find_package(blaze REQUIRED)
  find_package(TBB REQUIRED)
  find_package(LAPACK REQUIRED)
  
  string(APPEND CPPHOTS_OPTIONAL_DEPS "find_dependency(blaze REQUIRED)\n")
  string(APPEND CPPHOTS_OPTIONAL_DEPS "set(CMAKE_MODULE_PATH \${CMAKE_MODULE_PATH} \"\${CMAKE_CURRENT_LIST_DIR}/modules/\")\nfind_dependency(TBB REQUIRED)\n")
  string(APPEND CPPHOTS_OPTIONAL_DEPS "find_dependency(LAPACK REQUIRED)\n")

  FetchContent_Declare(
    peregrine
    GIT_REPOSITORY git@github.com:OOub/peregrine.git
    GIT_TAG        6a0b0a15add7ae952029b6ac1565839dfdae8ae7)

  FetchContent_GetProperties(peregrine)
  if(NOT peregrine_POPULATED)
      FetchContent_Populate(peregrine)
  endif()

endif()

# python
if (BUILD_PLOTS)
    find_package(Python3 REQUIRED COMPONENTS Development Interpreter)
endif()


# main library
add_subdirectory(src)


# installing

# create the Config.cmake file
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
set(CONF_INCLUDE_DIRS "\${CPPHOTS_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(cmake/cpphotsConfig.cmake.in "${PROJECT_BINARY_DIR}/cmake/cpphotsConfig.cmake" @ONLY)

# create cpphotsConfigVersion.cmake file
configure_file(cmake/cpphotsConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/cmake/cpphotsConfigVersion.cmake" @ONLY)

# install both
install(FILES
  "${PROJECT_BINARY_DIR}/cmake/cpphotsConfig.cmake"
  "${PROJECT_BINARY_DIR}/cmake/cpphotsConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}")

# install also export (that are included by Config.cmake)
install(EXPORT cpphotsTargets
        FILE cpphotsTargets.cmake
        DESTINATION lib/cmake/cpphots)

# install peregrine dependencies finders
if (WITH_PEREGRINE)
    install(FILES
              cmake/FindTBB.cmake
            DESTINATION
              "${INSTALL_CMAKE_DIR}/modules")
endif()

# tests
option(BUILD_TEST "Build tests" OFF)
if (BUILD_TEST)
    enable_testing()
    add_subdirectory(tests)
endif (BUILD_TEST)


# examples
option(BUILD_EXAMPLES "Build examples" OFF)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)


# docs
option(BUILD_DOCS "Build documentation" OFF)
option(WITH_SPHINX "Generate documentation with sphinx" OFF)

if (BUILD_DOCS)
add_subdirectory(docs)
endif (BUILD_DOCS)
