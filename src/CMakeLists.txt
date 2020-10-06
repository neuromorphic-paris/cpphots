# define file lists
set(CPPHOTS_SOURCES
    classification.cpp
    events_utils.cpp
    layer.cpp
    network.cpp
    run.cpp
    time_surface.cpp)

set(CPPHOTS_HEADERS
    "../include/cpphots/classification.h"
    "../include/cpphots/events_utils.h"
    "../include/cpphots/layer.h"
    "../include/cpphots/network.h"
    "../include/cpphots/run.h"
    "../include/cpphots/time_surface.h")

if (BUILD_PLOTS)
    file(READ ../python/tsplot.py PYTHON_PLOT_CODE)
    configure_file(plots.cpp.in ${PROJECT_BINARY_DIR}/src/plots.cpp @ONLY)
    list(APPEND CPPHOTS_SOURCES ${PROJECT_BINARY_DIR}/src/plots.cpp)
    list(APPEND CPPHOTS_HEADERS "../include/cpphots/plots.h")
endif()

# add library and link to libraries
add_library(cpphots
            ${CPPHOTS_SOURCES})

target_include_directories(cpphots PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)

target_include_directories(cpphots PRIVATE "${sepia_SOURCE_DIR}/source")

target_link_libraries(cpphots pthread Eigen3::Eigen)

if (BUILD_PLOTS)
    target_include_directories(cpphots PRIVATE ${Python3_INCLUDE_DIRS})
    target_link_libraries(cpphots ${Python3_LIBRARIES})
endif()

# installing
set_target_properties(cpphots PROPERTIES
                      PUBLIC_HEADER "${CPPHOTS_HEADERS}")

install(TARGETS cpphots
        EXPORT cpphotsTargets
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/cpphots")