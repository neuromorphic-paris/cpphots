# disable installing of googletest
option(INSTALL_GTEST "" OFF)
set(INSTALL_GTEST OFF)

# download googletest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        main)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

include(GoogleTest)

# function to add new tests
function(add_new_test target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} cpphots gtest gmock gtest_main)
    gtest_discover_tests(${target}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
endfunction()

# list of tests
add_new_test(test_commons commons.test.cpp)
add_new_test(test_streamable streamable.test.cpp)
add_new_test(test_classification classification.test.cpp)
add_new_test(test_timesurface timesurface.test.cpp)
add_new_test(test_layer layer.test.cpp)
add_new_test(test_network network.test.cpp)
add_new_test(test_saveload saveload.test.cpp)
add_new_test(test_layer_modifiers layer_modifiers.test.cpp)
add_new_test(test_run run.test.cpp)
add_new_test(test_kmeans kmeans.test.cpp)

if(WITH_PEREGRINE)
    add_new_test(test_gmm gmm.test.cpp)
endif()

# python test for plotting functions
if (BUILD_PLOTS)
    add_test(NAME test_plots_py COMMAND Python3::Interpreter -m unittest tsplot_test.py WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/python)
endif()