cmake_minimum_required(VERSION 3.10)
project(LAB2)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Список всех исходных файлов
set(SOURCES
        main.cpp
        Table.cpp
        HashTable.cpp
        Scanner.cpp
)

# Создаем исполняемый файл из указанных исходных файлов
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
