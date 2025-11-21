cmake_minimum_required(VERSION 3.29)
# 明确声明支持C和C++
project(untitled4 C CXX)

# 设置C和C++标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 为项目添加编译选项（可选）
add_compile_options(-Wall -Wextra -pedantic)

# 定义源文件路径变量，提高可维护性
set(SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(HARDWARE_DIR ${SOURCE_DIR}/Hardware)
set(LIBRARY_DIR ${SOURCE_DIR}/Library)
set(START_DIR ${SOURCE_DIR}/Start)
set(USER_DIR ${SOURCE_DIR}/User)
set(TOOLS_DIR ${SOURCE_DIR}/Tools)
set(UI_DIR ${SOURCE_DIR}/UI)

# 收集C源文件
file(GLOB_RECURSE C_SOURCES
        ${HARDWARE_DIR}/*.c
        ${LIBRARY_DIR}/*.c
        ${START_DIR}/*.c
        ${USER_DIR}/*.c
        ${TOOLS_DIR}/*.c
        ${UI_DIR}/*.c
)

# 收集C++源文件
file(GLOB_RECURSE CPP_SOURCES
        ${USER_DIR}/*.cpp
)

# 为了调试，打印收集的源文件
#message(STATUS "C sources: ${C_SOURCES}")
#message(STATUS "C++ sources: ${CPP_SOURCES}")

# 创建一个静态库，包含所有通用源文件
add_library(lib STATIC ${C_SOURCES}
        UI/config.h
        User/main.c

)

# 设置库的包含目录（现代CMake风格）
target_include_directories(lib PUBLIC
        ${HARDWARE_DIR}
        ${LIBRARY_DIR}
        ${START_DIR}
        ${USER_DIR}
        ${TOOLS_DIR}
        ${UI_DIR}
)

# 创建可执行文件 untitled4
add_executable(untitled4
        ${CPP_SOURCES}
        # 只需要添加C++源文件，因为C源文件已经在lib中
)

# 链接库到可执行文件
target_link_libraries(untitled4 PRIVATE lib)

# 创建第二个可执行文件 my_target
add_executable(my_target ${USER_DIR}/main.cpp)
target_link_libraries(my_target PRIVATE lib)

# 为可执行文件设置C++标准（如果需要）
set_target_properties(untitled4 my_target PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
)