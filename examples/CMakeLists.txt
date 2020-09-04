add_executable(poker_dvs poker_dvs.cpp)

find_package(cpphots CONFIG REQUIRED)

target_include_directories(poker_dvs PRIVATE ${CPPHOTS_INCLUDE_DIRS})
target_link_libraries(poker_dvs ${CPPHOTS_LIBRARIES})