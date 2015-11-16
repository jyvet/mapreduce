project (mapreduce)

SET(CMAKE_C_FLAGS "-O3 -march=native")

OPTION(DEBUG "Debug mode." OFF)

# use cmake -D DEBUG:BOOL=TRUE
IF(DEBUG)
    SET(CMAKE_C_FLAGS "-g -Wall")
ENDIF(DEBUG)

# use cmake -D GPROF:BOOL=TRUE
IF(GPROF)
    SET(CMAKE_C_FLAGS "-pg -Wall")
ENDIF(GPROF)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
 
INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

ADD_EXECUTABLE(mapred main.c args.c tools.c buffalloc.c word.c wordstreamer.c wordstreamer_scatter.c wordstreamer_interleave.c dictionary.c mapreduce.c mapreduce_sequential.c mapreduce_parallel.c)

TARGET_LINK_LIBRARIES(mapred ${LIBS} pthread)

INSTALL(TARGETS mapred DESTINATION bin)
INSTALL(CODE "MESSAGE(\"MapReduce installed.\")")
