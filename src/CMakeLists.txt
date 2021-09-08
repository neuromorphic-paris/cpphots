# define file lists
set(CPPHOTS_SOURCES
    types.cpp
    interfaces/streamable.cpp
    classification.cpp
    events_utils.cpp
    layer.cpp
    network.cpp
    run.cpp
    time_surface.cpp
    clustering/utils.cpp
    clustering/cosine.cpp
    layer_modifiers.cpp
    load.cpp
    assert.cpp)

if (BUILD_PLOTS)
    file(READ ../python/tsplot.py PYTHON_PLOT_CODE)
    configure_file(plots.cpp.in ${PROJECT_BINARY_DIR}/src/plots.cpp @ONLY)
    list(APPEND CPPHOTS_SOURCES ${PROJECT_BINARY_DIR}/src/plots.cpp)
    set(PLOTS_EXCLUDE "")
else()
    set(PLOTS_EXCLUDE "plots.h")
endif()

if (WITH_PEREGRINE)
    list(APPEND CPPHOTS_SOURCES clustering/gmm.cpp)
    set(PEREGRINE_EXCLUDE "")
else()
    set(PEREGRINE_EXCLUDE "*gmm.h")
endif()

# add library and link to libraries
add_library(cpphots
            ${CPPHOTS_SOURCES})

target_include_directories(cpphots PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)

target_include_directories(cpphots PRIVATE "${sepia_SOURCE_DIR}/source")

target_link_libraries(cpphots pthread Eigen3::Eigen)

# add flags
if (DOUBLE_PRECISION)
    target_compile_definitions(cpphots PUBLIC CPPHOTS_DOUBLE_PRECISION)
endif()

if (ENABLE_ASSERTS)
    target_compile_definitions(cpphots PUBLIC CPPHOTS_ASSERTS)
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
install(TARGETS cpphots
        EXPORT cpphotsTargets
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")

# need to install headers like this to preserve the structure
install(DIRECTORY "../include/cpphots"
        DESTINATION "${INSTALL_INCLUDE_DIR}"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "${PLOTS_EXCLUDE}" EXCLUDE
        PATTERN "${PEREGRINE_EXCLUDE}" EXCLUDE)