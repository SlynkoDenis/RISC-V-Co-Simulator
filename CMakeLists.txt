cmake_minimum_required(VERSION 3.17)
project(HW_CO_SIMULATION)

set(CMAKE_CXX_STANDARD 17)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} LOWER_BUILD_TYPE)
if(${LOWER_BUILD_TYPE} MATCHES "debug")
    add_definitions(-DDEBUG)
endif()


set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


add_executable(sim main.cpp)
add_subdirectory(models/functional)
add_subdirectory(utils)

target_link_libraries(sim PUBLIC
        functional
        utils
        )
