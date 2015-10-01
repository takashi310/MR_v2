
cmake_minimum_required (VERSION 2.6)

project (PIPSEQ)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB src src/*.cpp)
file(GLOB hdr src/*.h)

add_executable (PIPSEQ ${src} ${hdr})