cmake_minimum_required(VERSION 3.17)
project(HW_CO_SIMULATION)

set(CMAKE_CXX_STANDARD 17)

function(add_git_submodule dir)
    # add a Git submodule directory to CMake, assuming the
    # Git submodule directory is a CMake project.
    #
    # Usage: in CMakeLists.txt
    #
    # include(AddGitSubmodule.cmake)
    # add_git_submodule(mysubmod_dir)

    find_package(Git REQUIRED)

    if(NOT EXISTS ${dir}/CMakeLists.txt)
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${dir}
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif()

    add_subdirectory(${dir})

endfunction(add_git_submodule)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
string(TOLOWER ${CMAKE_BUILD_TYPE} LOWER_BUILD_TYPE)
if(${LOWER_BUILD_TYPE} MATCHES "debug")
    add_definitions(-DDEBUG)
endif()

if(NOT WILL_TRACE)
    set(WILL_TRACE True)
endif()
string(TOLOWER ${WILL_TRACE} LOWER_WILL_TRACE)
if(${LOWER_WILL_TRACE} MATCHES "true")
    add_definitions(-DWILL_TRACE)
endif()


set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


add_executable(sim main.cpp)
include_directories(models/shared)
add_subdirectory(models/functional)
include_directories(models/pipeline/modules)
add_subdirectory(models/pipeline/pipeline)
add_subdirectory(co_simulator)
add_subdirectory(utils)


if(NOT WITH_TESTS)
    set(WITH_TESTS False)
endif()
string(TOLOWER ${WITH_TESTS} LOWER_WITH_TESTS)
if(${LOWER_WITH_TESTS} MATCHES "true" OR ${LOWER_BUILD_TYPE} MATCHES "debug")
    add_git_submodule(third_party/googletest)
    add_subdirectory(tests)
endif()


target_link_libraries(sim PUBLIC
        co_simulator
        functional
        pipeline
        utils
        )
