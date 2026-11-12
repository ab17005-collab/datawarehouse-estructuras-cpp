cmake_minimum_required(VERSION 3.16)
project(DataWarehouseEstructuras VERSION 2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_executable(datawarehouse
    src/bplus/ArbolBPlus.cpp
    src/bplus/ModuloBPlus.cpp
    src/common/Entrada.cpp
    src/common/Modelo.cpp
    src/etl/GrafoETL.cpp
    src/etl/ModuloETL.cpp
    src/main.cpp
    src/olap/CuboOLAP.cpp
    src/olap/ModuloOLAP.cpp
)
target_include_directories(datawarehouse PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
