cmake_minimum_required(VERSION 2.8)
project(ENGI9875 C)

add_subdirectory(A1)

add_executable(${PROJECT_NAME} main.c)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -pedantic -Wno-error)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        A1)

target_link_libraries(${PROJECT_NAME}
        A1)
