cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 14)

project(A2)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
        test.cpp
        rtos-alloc.c
#        passthrough.c
#        passthrough-internal.c
        )

add_executable(${PROJECT_NAME}  ${SOURCES})

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -pedantic -Wno-error )

target_include_directories(${PROJECT_NAME}
        PUBLIC)

target_link_libraries(${PROJECT_NAME}
        /usr/lib/libgrading.so
        )
