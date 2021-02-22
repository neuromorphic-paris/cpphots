# check if Doxygen is installed
find_package(Doxygen REQUIRED)

# find public headers
file(GLOB_RECURSE CPPHOTS_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../include/cpphots/*.h)

# set Doxygen variables
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.html)

# correct macros
if (DOUBLE_PRECISION)
    set(CPPHOTS_DOXYGEN_DOUBLE_PRECISION CPPHOTS_DOUBLE_PRECISION)
endif()

# configure Doxyfile
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# doxygen won't create this folder
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# add command and target
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${CPPHOTS_PUBLIC_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_IN}
                   COMMENT "Generating docs with doxygen"
                   VERBATIM)

add_custom_target(docs DEPENDS ${DOXYGEN_INDEX_FILE})
