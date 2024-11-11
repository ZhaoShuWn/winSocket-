cmake_minimum_required(VERSION 3.10)

# 项目名称（可以自定义）
project(MySocket)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(Server)
add_subdirectory(Client)
