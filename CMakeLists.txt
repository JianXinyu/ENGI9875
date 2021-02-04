cmake_minimum_required(VERSION 3.13)
project(ENGI9875 C)

add_subdirectory(A2)

add_executable(${PROJECT_NAME} main.c)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -pedantic -Wno-error)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        A2)

target_link_libraries(${PROJECT_NAME}
        A2)
