set(SOURCES debug.cpp
            load_elf.cpp
            system.cpp)

add_library(utils STATIC ${SOURCES})

target_sources(utils PUBLIC
        debug.h
        file.h
        load_elf.h
        macros.h
        system.h
        )

include_directories(${CMAKE_SOURCE_DIR}/models/shared)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
