set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(Doxygen REQUIRED)

find_program(SPHINX_BUILD sphinx-build REQUIRED)

set(DOCS_CFG_DIR ${CMAKE_CURRENT_LIST_DIR})
set(DOCS_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(DOCS_SRC_DIR ${PROJECT_SOURCE_DIR}/docs)

set(SPHINX_OPTS "-j auto -W --keep-going -T")
separate_arguments(SPHINX_OPTS)

add_custom_target(
    docs
    COMMAND ${CMAKE_COMMAND} -E env
        ${SPHINX_BUILD}
        -b html
        -c ${DOCS_CFG_DIR}
        -w ${DOCS_BUILD_DIR}/build.log
        ${SPHINX_OPTS}
        ${DOCS_SRC_DIR}
        ${DOCS_BUILD_DIR}
    USES_TERMINAL
    COMMENT "Running Sphinx HTML build..."
)
