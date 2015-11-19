INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_SOURCE_DIR}/../src)

FIND_PACKAGE(Check REQUIRED)

SET(CMAKE_C_FLAGS "-g -Wall -std=gnu99")

# Subdirectories
ADD_SUBDIRECTORY(args)
ADD_SUBDIRECTORY(word)
ADD_SUBDIRECTORY(wordstreamer_schunks)
ADD_SUBDIRECTORY(wordstreamer_iwords)
ADD_SUBDIRECTORY(buffalloc)
ADD_SUBDIRECTORY(dictionary)
ADD_SUBDIRECTORY(mapreduce_sequential)
ADD_SUBDIRECTORY(mapreduce_parallel)
