Cmake_minimum_required (VERSION 2.6)

SET (NAME wordstreamer_iwords)
SET (TEST_NAME test_${NAME})
SET (SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeModules;${CMAKE_MODULE_PATH}")

SET(CMAKE_C_FLAGS "-g -Wall")

ENABLE_TESTING()

FIND_PACKAGE(Check REQUIRED)

INCLUDE_DIRECTORIES(${CHECK_INCLUDE_DIRS})
SET(LIBS ${LIBS} ${CHECK_LIBRARIES})
INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

ADD_EXECUTABLE(${TEST_NAME} ${SRC_PATH}/${NAME}.c ${TEST_NAME}.c
               ${SRC_PATH}/filereader.c
               ${SRC_PATH}/filereader_mmap.c
               ${SRC_PATH}/filereader_read.c
               ${SRC_PATH}/tools.c)

TARGET_LINK_LIBRARIES(${TEST_NAME} ${LIBS} pthread)

ADD_TEST(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
