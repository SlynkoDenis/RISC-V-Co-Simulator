set(SOURCES pipeline.cpp)

add_library(pipeline STATIC ${SOURCES})

target_sources(pipeline PUBLIC
        control_unit.h
        hazard_unit.h
        instr_exceptions.h
        instruction.h
        pipeline.h
        )

include_directories(${CMAKE_SOURCE_DIR}/utils)
include_directories(${CMAKE_SOURCE_DIR}/models/pipeline/modules)
include_directories(${CMAKE_SOURCE_DIR}/models/shared)
target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
