set(SOURCES co_simulator.cpp)

add_library(co_simulator STATIC ${SOURCES})

target_sources(co_simulator PUBLIC
        co_simulator.h
        )

# TODO: make correct CMakeLists files
include_directories(${CMAKE_SOURCE_DIR}/models/functional)
include_directories(${CMAKE_SOURCE_DIR}/models/pipeline/pipeline)
include_directories(${CMAKE_SOURCE_DIR}/utils)
target_include_directories(co_simulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
