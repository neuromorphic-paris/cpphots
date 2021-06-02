# define file lists
set(CPPHOTS_SOURCES
    types.cpp
    interfaces/streamable.cpp
    classification.cpp
    events_utils.cpp
    network.cpp
    run.cpp
    time_surface.cpp
    clustering.cpp
    layer_modifiers.cpp
    load.cpp)

set(CPPHOTS_HEADERS
    "../include/cpphots/types.h"
    "../include/cpphots/interfaces/streamable.h"
    "../include/cpphots/classification.h"
    "../include/cpphots/events_utils.h"
    "../include/cpphots/layer.h"
    "../include/cpphots/layer_traits.h"
    "../include/cpphots/layer_modifiers.h"
    "../include/cpphots/network.h"
    "../include/cpphots/run.h"
    "../include/cpphots/time_surface.h"
    "../include/cpphots/clustering.h"
    "../include/cpphots/load.h")

if (BUILD_PLOTS)
    file(READ ../python/tsplot.py PYTHON_PLOT_CODE)
    configure_file(plots.cpp.in ${PROJECT_BINARY_DIR}/src/plots.cpp @ONLY)
    list(APPEND CPPHOTS_SOURCES ${PROJECT_BINARY_DIR}/src/plots.cpp)
    list(APPEND CPPHOTS_HEADERS "../include/cpphots/plots.h")
endif()

if (WITH_PEREGRINE)
    list(APPEND CPPHOTS_SOURCES gmm_clustering.cpp)
    list(APPEND CPPHOTS_HEADERS "../include/cpphots/gmm_clustering.h")
endif()

# add library and link to libraries
add_library(cpphots
            ${CPPHOTS_SOURCES})

target_include_directories(cpphots PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)

target_include_directories(cpphots PRIVATE "${sepia_SOURCE_DIR}/source")

target_link_libraries(cpphots pthread Eigen3::Eigen)

if (DOUBLE_PRECISION)
    target_compile_definitions(cpphots PUBLIC CPPHOTS_DOUBLE_PRECISION)
endif()

if (BUILD_PLOTS)
    target_include_directories(cpphots PRIVATE ${Python3_INCLUDE_DIRS})
    target_link_libraries(cpphots ${Python3_LIBRARIES})
endif()

if (WITH_PEREGRINE)
    target_compile_definitions(cpphots PUBLIC CPPHOTS_WITH_PEREGRINE)
    target_include_directories(cpphots PRIVATE ${peregrine_SOURCE_DIR}/source )
    if(${CMAKE_VERSION} VERSION_LESS "3.18.0")
        target_link_libraries(cpphots blaze::blaze TBB::tbb ${LAPACK_LIBRARIES})
    else()
        target_link_libraries(cpphots blaze::blaze TBB::tbb LAPACK::LAPACK)
    endif()
endif()

# installing
set_target_properties(cpphots PROPERTIES
                      PUBLIC_HEADER "${CPPHOTS_HEADERS}")

install(TARGETS cpphots
        EXPORT cpphotsTargets
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/cpphots")