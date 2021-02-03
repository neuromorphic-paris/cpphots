add_executable(poker_dvs poker_dvs.cpp)
target_link_libraries(poker_dvs cpphots)

add_executable(ts_benchmark ts_benchmark.cpp)
target_link_libraries(ts_benchmark cpphots)

if (WITH_PEREGRINE)
    add_executable(clustering_benchmark clustering_benchmark.cpp)
    target_link_libraries(clustering_benchmark cpphots)
endif()