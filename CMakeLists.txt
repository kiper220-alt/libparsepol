cmake_minimum_required(VERSION 3.14)

project(libparsepol)

find_package(Iconv REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_library(parsepol STATIC src/parser.cpp src/binary.cpp)
target_include_directories(parsepol PUBLIC inc PRIVATE ${Iconv_INCLUDE_DIRS})

add_executable(test test/main.cpp test/binary.h test/endian.h test/testcases.h test/generatecase.h)
target_link_libraries(test parsepol ${Iconv_LIBRARIES})
