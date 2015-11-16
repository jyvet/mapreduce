Cmake_minimum_required (VERSION 2.6)

SET (NAME mapreduce_parallel) 
SET (TEST_NAME test_${NAME})
SET (SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeModules;${CMAKE_MODULE_PATH}")

ENABLE_TESTING ()

FIND_PACKAGE (Check REQUIRED)

INCLUDE_DIRECTORIES (${CHECK_INCLUDE_DIRS})
SET (LIBS ${LIBS} ${CHECK_LIBRARIES})
INCLUDE_DIRECTORIES (. ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

ADD_EXECUTABLE (${TEST_NAME} ${SRC_PATH}/${NAME}.c ${SRC_PATH}/word.c ${SRC_PATH}/tools.c ${SRC_PATH}/dictionary.c ${SRC_PATH}/buffalloc.c ${SRC_PATH}/buffalloc.c ${SRC_PATH}/wordstreamer.c ${SRC_PATH}/wordstreamer_scatter.c ${SRC_PATH}/wordstreamer_interleave.c ${SRC_PATH}/mapreduce.c ${SRC_PATH}/mapreduce_sequential.c ${TEST_NAME}.c)

TARGET_LINK_LIBRARIES (${TEST_NAME} ${LIBS} pthread)

ADD_TEST (NAME ${TEST_NAME} COMMAND ${TEST_NAME})
