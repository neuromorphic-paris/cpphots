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

set_target_properties(cpphots PROPERTIES
  PUBLIC_HEADER "classification.h;events_utils.h;layer.h;network.h;run.h;time_surface.h")

# installation
install(TARGETS cpphots
  # IMPORTANT: Add the cpphots library to the "export-set"
  EXPORT cpphotsTargets
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/cpphots"
    COMPONENT dev)
