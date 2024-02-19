cmake_minimum_required(VERSION 3.2)

project(MessageQueue)


add_compile_options(-pthread)
set(SOURCES
    RadarTestTask.cpp
)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(HEADERS
    MessageQueueWithMyHeap.h
    MessageQueueWithMyHeapRealisation.h
    MessageQueueWithSTL.h
    MessageQueueWithSTLRealisation.h
    Reader.h
    ReaderRealisation.h
    Writer.h
    WriterRealisation.h
    RetCodes.h
    StateVariables.h
)
add_executable(MessageQueue ${SOURCES} ${HEADERS})

set_target_properties(
    MessageQueue PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
