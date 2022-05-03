set(SOURCES
    decoder.cpp
    instruction.cpp
    model.cpp
    )

add_library(functional STATIC ${SOURCES})

target_sources(functional PUBLIC
        decoder.h
        instruction.h
        interpreter.h
        memory.h
        model.h
        )

include_directories(${CMAKE_SOURCE_DIR}/utils)
target_include_directories(functional PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
