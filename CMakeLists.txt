cmake_minimum_required(VERSION 3.6)
project(PictureAnalysis)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp window.cpp window.h filter.cpp threshold.cpp)
add_executable(PictureAnalysis ${SOURCE_FILES})

# include opencv
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# include SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

target_link_libraries(PictureAnalysis ${OpenCV_LIBS} ${SDL2_LIBRARIES})