set(SOURCES debug.cpp
            load_elf.cpp)

add_library(utils STATIC ${SOURCES})

target_sources(utils PUBLIC
        debug.h
        load_elf.h
        macros.h
        )

include_directories(${CMAKE_SOURCE_DIR}/models/shared)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
