cmake_minimum_required(VERSION 3.28)

#set(CMAKE_CXX_EXTENSIONS ON) 
#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

project(modules-example)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD OFF) 

#file(GLOB SOURCES "*.cpp")

add_executable(demo)
target_compile_options(demo PRIVATE -stdlib=libc++)
target_link_options(demo PRIVATE -stdlib=libc++)
target_sources(demo
    PUBLIC
    TpUtlCommon.cpp
)

#target_sources(demo
#  PUBLIC
#    FILE_SET all_my_modules TYPE CXX_MODULES FILES
#    TpAlgoAStar.cpp
#    TpUtlTypeTemplates.cpp
#    TpUtlTransforms.cpp
#    TpUtlUnsortedSet.cpp
#)