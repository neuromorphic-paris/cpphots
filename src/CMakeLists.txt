find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_library(cpphots SHARED
            classification.cpp
            events_utils.cpp
            layer.cpp
            network.cpp
            run.cpp
            time_surface.cpp)

# include sepia
if(NOT SEPIA_DIR)
    message(FATAL_ERROR "The variable SEPIA_DIR must be specified.")
endif()
target_include_directories(cpphots PRIVATE ${SEPIA_DIR})

target_link_libraries(cpphots pthread Eigen3::Eigen)


# installation and packaging
install(TARGETS cpphots EXPORT cpphotsTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/cpphots)

install(FILES
          classification.h
          events_utils.h
          layer.h
          network.h
          run.h
          time_surface.h
        DESTINATION
          include/cpphots
        COMPONENT
          devel)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cpphots/cpphotsConfigVersion.cmake"
  VERSION "1.0.0"
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT cpphotsTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cpphots/cpphotsTargets.cmake"
  NAMESPACE Upstream::
)

configure_file(../cmake/cpphotsConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/cpphots/cpphotsConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/cpphots)
install(EXPORT cpphotsTargets
  FILE
    cpphotsTargets.cmake
  NAMESPACE
    Upstream::
  DESTINATION
    ${ConfigPackageLocation}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cpphots/cpphotsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cpphots/cpphotsConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)