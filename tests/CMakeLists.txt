include(GoogleTest)

function(add_new_test target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} cpphots gtest gmock gtest_main)
    gtest_discover_tests(${target}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
endfunction()

add_new_test(test_classification classification.test.cpp)
add_new_test(test_timesurface timesurface.test.cpp)
add_new_test(test_layer layer.test.cpp)

if (BUILD_PLOTS)
    add_test(NAME test_plots_py COMMAND Python3::Interpreter -m unittest tsplot_test.py WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/python)
endif()