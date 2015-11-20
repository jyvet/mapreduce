project (mapreduce)

SET(CMAKE_C_FLAGS "-O2 -march=native")

OPTION(DEBUG "Debug mode." OFF)

# use cmake -D DEBUG:BOOL=TRUE
IF(DEBUG)
    SET(CMAKE_C_FLAGS "-g -Wall -std=gnu99")
ENDIF(DEBUG)

# use cmake -D GPROF:BOOL=TRUE
IF(GPROF)
    SET(CMAKE_C_FLAGS "-pg -Wall -std=gnu99")
ENDIF(GPROF)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

ADD_EXECUTABLE(mapred main.c
                      args.c
                      tools.c
                      buffalloc.c
                      word.c
                      filereader.c
                      filereader_mmap.c
                      wordstreamer.c
                      wordstreamer_schunks.c
                      #wordstreamer_iwords.c
                      dictionary.c
                      mapreduce.c
                      mapreduce_sequential.c
                      mapreduce_parallel.c)

TARGET_LINK_LIBRARIES(mapred ${LIBS} pthread)

INSTALL(TARGETS mapred DESTINATION bin)
INSTALL(CODE "MESSAGE(\"MapReduce installed.\")")
