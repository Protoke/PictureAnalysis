cmake_minimum_required(VERSION 3.6)
project(PictureAnalysis)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp filter.cpp gradient.cpp gradient.h threshold.cpp)
add_executable(PictureAnalysis ${SOURCE_FILES})

# include opencv
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(PictureAnalysis ${OpenCV_LIBS} ${SDL2_LIBRARIES})