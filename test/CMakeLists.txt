PROJECT(opencv_test)
# requirement of cmake version
cmake_minimum_required(VERSION 3.5)

# set the directory of executable files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${opencv_test_SOURCE_DIR})

#set source directory
aux_source_directory(./src DIR_SRCS)

# find required opencv
set(OpenCV_DIR "/usr/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
# directory of opencv headers
include_directories(${OpenCV_INCLUDE_DIRS})
# name of executable file and path of source file
add_executable(a.out ${DIR_SRCS})
# directory of opencv library
link_directories(${OpenCV_LIBRARY_DIRS})
# opencv libraries
target_link_libraries(a.out ${OpenCV_LIBS})
