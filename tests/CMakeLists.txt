function(add_new_test target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} cpphots)

    add_test(${target} ${target})

endfunction()


add_new_test(test_classification classification.test.cpp)
add_new_test(test_timesurface timesurface.test.cpp)
add_new_test(test_layer layer.test.cpp)

# copy files for test
# we don't want to copy if we're building in the source dir
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/trcl0.es"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../data/trcl0.es" "${CMAKE_CURRENT_BINARY_DIR}/trcl0.es"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../data/trcl0.es"
        )
endif()

# files are only copied if a target depends on them
add_custom_target(data-test ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/trcl0.es")