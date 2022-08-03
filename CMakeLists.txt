cmake_minimum_required(VERSION 3.16)

execute_process (COMMAND date "+%Y%m%d-%H:%M:%S(%Z)"
    OUTPUT_VARIABLE CMAKE_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

set(MAJOR_VERSION 00)
set(MINOR_VERSION 99)
set(MICRO_VERSION 02)

set(PROJECT_NAME jar-garden)

string(APPEND PROJECT_VER
    ${MAJOR_VERSION}
    ${MINOR_VERSION}
    ${MICRO_VERSION}-
    ${CMAKE_VERSION})

set(EXTRA_COMPONENT_DIRS main app core sensors actuators drivers)


include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(jar-garden)
