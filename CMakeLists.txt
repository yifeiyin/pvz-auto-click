cmake_minimum_required(VERSION 2.8)

project(auto-click-improved)
add_executable(${PROJECT_NAME} "main.cpp")

INCLUDE_DIRECTORIES("execcommand.h")
INCLUDE_DIRECTORIES("pvzsunautocollect.h")
INCLUDE_DIRECTORIES("pvzsunautocollect.cpp")

find_package( OpenCV REQUIRED )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

add_compile_options("-Wgnu-static-float-init")
