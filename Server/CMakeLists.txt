
cmake_minimum_required(VERSION 3.10)

# 项目名称（可以自定义）
project(MySocketServer)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加可执行文件（MySocketServer是可执行文件的名字，它最终会生成MySocketServer.exe，后面跟着的是生成所需要的文件）
add_executable(MySocketServer Server.cpp)
