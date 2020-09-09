add_library(cpphots
            classification.cpp
            events_utils.cpp
            layer.cpp
            network.cpp
            run.cpp
            time_surface.cpp)

target_include_directories(cpphots PUBLIC 
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)

target_include_directories(cpphots PRIVATE "${sepia_SOURCE_DIR}/source")

target_link_libraries(cpphots pthread Eigen3::Eigen)

set_target_properties(cpphots PROPERTIES
                      PUBLIC_HEADER "../include/cpphots/classification.h;../include/cpphots/events_utils.h;../include/cpphots/layer.h;../include/cpphots/network.h;../include/cpphots/run.h;../include/cpphots/time_surface.h")

install(TARGETS cpphots
        EXPORT cpphotsTargets
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/cpphots")