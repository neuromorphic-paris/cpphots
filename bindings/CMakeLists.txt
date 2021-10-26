find_package(pybind11)
pybind11_add_module(pycpphots SHARED cpphots_module.cpp)
target_link_libraries(pycpphots PUBLIC cpphots)
set_target_properties(pycpphots PROPERTIES OUTPUT_NAME cpphots)

install(TARGETS pycpphots
        COMPONENT python
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python3.9/site-packages")