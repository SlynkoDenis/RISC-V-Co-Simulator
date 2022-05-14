set(SOURCES
        pipeline.cpp
        pipe.cpp)

add_library(pipeline STATIC ${SOURCES})

target_sources(pipeline PUBLIC
        control_unit.h
        hazard_unit.h
        instr_exceptions.h
        instruction.h
        pipeline.h
        )

if(NOT REMOVE_TRACES)
    set(REMOVE_TRACES False)
endif()
string(TOLOWER ${REMOVE_TRACES} LOWER_REMOVE_TRACES)
if(${LOWER_REMOVE_TRACES} MATCHES "true")
    add_definitions(-DREMOVE_TRACES)
endif()

add_executable(cycle pipe.cpp)

include_directories(${CMAKE_SOURCE_DIR}/utils)
include_directories(${CMAKE_SOURCE_DIR}/models/pipeline/modules)
include_directories(${CMAKE_SOURCE_DIR}/models/shared)

target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(cycle PUBLIC
        pipeline
        utils
        )
