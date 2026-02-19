cmake_minimum_required(VERSION 3.23)
project(csv_median_calculator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include(FetchContent)

find_package(Boost 1.74 REQUIRED COMPONENTS 
    program_options
    system
)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/main_application/main_application.cpp
    src/file_handlers/config_manager.cpp
    src/file_handlers/csv_reader.cpp
    src/file_handlers/csv_writer.cpp
    src/file_handlers/file_scanner.cpp
    src/data_manager/data_manager.cpp
    src/median_calculator/median_calculator.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${tomlplusplus_SOURCE_DIR}/include
    src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::program_options
    Boost::system
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.toml
    ${CMAKE_CURRENT_BINARY_DIR}/config.toml
    COPYONLY
)

option(BUILD_TESTS "Собрать тесты" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()