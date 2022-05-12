set(SOURCES
    model.cpp
    )

add_library(functional STATIC ${SOURCES})

target_sources(functional PUBLIC
        decoder.h
        instruction.h
        interpreter.h
        model.h
        )

include_directories(${CMAKE_SOURCE_DIR}/utils)
include_directories(${CMAKE_SOURCE_DIR}/models/shared)
target_include_directories(functional PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
