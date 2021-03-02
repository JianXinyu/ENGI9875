cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 14)

project(lab2)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
        thread.c
        )

find_package( Threads )
add_executable(${PROJECT_NAME}  ${SOURCES})

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -pedantic -Wno-error -pthread)

target_include_directories(${PROJECT_NAME}
        PUBLIC)

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_THREAD_LIBS_INIT}
        )
