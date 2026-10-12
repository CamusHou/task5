cmake_minimum_required(VERSION 3.10)
project(Task5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(COMMON_SOURCES ${CMAKE_SOURCE_DIR}/function/get_and_show_picture.cpp  ${CMAKE_SOURCE_DIR}/function/video.cpp)
#自动收集question/ 下所有Q*.cpp文件
file(GLOB QUESTION_SOURCES "${CMAKE_SOURCE_DIR}/question/Q*.cpp")
foreach(src ${QUESTION_SOURCES})
get_filename_component(name ${src} NAME_WE) #取出文件名
add_executable(${name} ${src} ${COMMON_SOURCES})
target_link_libraries(${name} ${OpenCV_LIBS})
endforeach()
#正式项目
add_executable(light_capture ${CMAKE_SOURCE_DIR}/programme/light_capture.cpp ${COMMON_SOURCES})
target_link_libraries(light_capture ${OpenCV_LIBS})

