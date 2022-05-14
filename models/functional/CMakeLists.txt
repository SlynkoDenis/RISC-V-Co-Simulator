set(SOURCES
    model.cpp
    int.cpp
    )

add_library(functional STATIC ${SOURCES})

target_sources(functional PUBLIC
        decoder.h
        instruction.h
        interpreter.h
        model.h
        )

if(NOT REMOVE_TRACES)
    set(REMOVE_TRACES False)
endif()
string(TOLOWER ${REMOVE_TRACES} LOWER_REMOVE_TRACES)
if(${LOWER_REMOVE_TRACES} MATCHES "true")
    add_definitions(-DREMOVE_TRACES)
endif()

add_executable(interpreter int.cpp)

include_directories(${CMAKE_SOURCE_DIR}/utils)
include_directories(${CMAKE_SOURCE_DIR}/models/shared)

target_include_directories(functional PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(interpreter PUBLIC
        functional
        utils
        )
