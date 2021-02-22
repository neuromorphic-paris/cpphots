# check if Doxygen is installed
find_package(Doxygen REQUIRED)

# find public headers
file(GLOB_RECURSE CPPHOTS_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../include/cpphots/*.h)

# set doxygen variables
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
                   COMMENT "Generating documentation with doxygen"
                   VERBATIM)


if (WITH_SPHINX)

    find_package(Sphinx REQUIRED)

    # set sphinx variables
    set(SPHINX_SOURCE ${CMAKE_CURRENT_BINARY_DIR})
    set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
    set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py ${SPHINX_SOURCE}/conf.py COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.rst ${SPHINX_SOURCE}/index.rst COPYONLY)

    add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                    COMMAND 
                        ${SPHINX_EXECUTABLE} -b html
                        -Dbreathe_projects.cpphots=${DOXYGEN_OUTPUT_DIR}/xml  # tell Breathe where to find the doxygen output
                        ${SPHINX_SOURCE} ${SPHINX_BUILD}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    DEPENDS
                    ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
                    ${DOXYGEN_INDEX_FILE}
                    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/conf.py
                    COMMENT "Generating documentation with sphinx")

    # general docs target (with sphinx)
    add_custom_target(docs DEPENDS ${DOXYGEN_INDEX_FILE} ${SPHINX_INDEX_FILE})

else()

    # general docs target (doxygen only)
    add_custom_target(docs DEPENDS ${DOXYGEN_INDEX_FILE})

endif(WITH_SPHINX)