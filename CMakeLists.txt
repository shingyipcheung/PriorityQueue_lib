project(PriorityQueue)

set(HEADER_FILES PriorityQueue.h)

set(SOURCE_FILES)

add_library(PriorityQueue_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})
set_target_properties(PriorityQueue_lib PROPERTIES LINKER_LANGUAGE CXX)