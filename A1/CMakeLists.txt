 cmake_minimum_required(VERSION 2.8)
 set(TARGET_NAME A1)

 add_library(${TARGET_NAME}
         STATIC
         aircraft.c
         )

 target_include_directories(${TARGET_NAME}
         PUBLIC
         ${CMAKE_SOURCE_DIR})