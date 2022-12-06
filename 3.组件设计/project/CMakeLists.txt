
CMAKE_MINIMUM_REQUIRED(VERSION 3.12.1)
PROJECT(server)

add_executable(simple src/simple.cpp)

include_directories(src)
#自动查找源文件
file(GLOB_RECURSE COM_LIST 
	src/com/*.cpp
	src/com/*.cc
	src/com/*.h
	src/com/*.hpp
	src/com/*.c
)
#执行程序和源代码
add_executable(demo src/main.cpp src/Robot.h  ${COM_LIST})