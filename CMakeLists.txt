
cmake_minimum_required(VERSION 3.16)

project(DataWarehouseEstructuras
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(datawarehouse
    src/main.cpp
    src/olap/CuboOLAP.cpp
    src/bplus/ArbolBPlus.cpp
    src/etl/GrafoETL.cpp
)

target_include_directories(datawarehouse
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
