cmake_minimum_required(VERSION 3.6)
project(rune)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
find_package(OpenCV REQUIRED)

add_executable(rune ${SOURCE_FILES})
link_directories(/usr/local/Cellar/hiredis/0.13.3/lib)
target_link_libraries(rune libhiredis.dylib libhiredis.0.13.dylib libhiredis.a)
TARGET_LINK_LIBRARIES(rune ${OpenCV_LIBS})