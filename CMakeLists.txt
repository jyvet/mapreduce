Cmake_minimum_required (VERSION 2.6)

SET (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

ADD_SUBDIRECTORY(src)

OPTION(BUILD_TESTS "Build tests." OFF)

# use cmake -D BUILD_TESTS:BOOL=TRUE
IF(BUILD_TESTS)

    ENABLE_TESTING()

    ADD_SUBDIRECTORY(tests)

    ADD_TEST(NAME test_args COMMAND test_args)
    ADD_TEST(NAME test_word COMMAND test_word)
    ADD_TEST(NAME test_buffalloc COMMAND test_buffalloc)
    ADD_TEST(NAME test_wordstreamer_scatter COMMAND test_wordstreamer_scatter)
    ADD_TEST(NAME test_dictionary COMMAND test_dictionary)
    ADD_TEST(NAME test_mapreduce_sequential COMMAND test_mapreduce_sequential)

    MESSAGE( STATUS "Tests folder added." )
ENDIF(BUILD_TESTS)
