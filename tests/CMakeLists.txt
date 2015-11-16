INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_SOURCE_DIR}/../src)

FIND_PACKAGE(Check REQUIRED)

# Subdirectories
ADD_SUBDIRECTORY(args)
ADD_SUBDIRECTORY(word)
ADD_SUBDIRECTORY(wordstreamer_scatter)
ADD_SUBDIRECTORY(wordstreamer_interleave)
ADD_SUBDIRECTORY(buffalloc)
ADD_SUBDIRECTORY(dictionary)
ADD_SUBDIRECTORY(mapreduce_sequential)
ADD_SUBDIRECTORY(mapreduce_parallel)
